#include "iPod.h"
#include "esp_log.h"
#include <climits>

template <typename T>
T swap_endian(T u)
{
    static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");

    union
    {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (size_t k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];

    return dest.u;
}

const char TAG[] = "IPOD";

uint8_t iPod::checksum(const uint8_t* data, uint32_t len)
{
    uint32_t sum = len;
    for (uint32_t i = 0; i < len; ++i)
        sum += data[i];

    return 0x100 - (sum & 0xFF);
}

iPod::iPod(HardwareSerial& ser): _ser(ser), _playStatusNotifications(false), _playStatusNotificationTimer(0), _recvItr(0), _recvSize(0)
{
    _name = "iPepe";
}

void iPod::handlePacket(const uint8_t* data, uint32_t len)
{
    uint8_t lingo = data[0];

    // exclude lingo
    const uint8_t* payload = data+1;
    uint32_t payload_len = len-1;

    switch(lingo)
    {
        case IPOD_LINGO_GENERAL:
            handleLingoGeneral(payload, payload_len);
            break;
        case IPOD_LINGO_DISPLAY_REMOTE:
            handleLingoDisplayRemote(payload, payload_len);
            break;
        case IPOD_LINGO_EXTENDED_INTERFACE:
            handleLingoExtendedInterface(payload, payload_len);
            break;
        default:
            ESP_LOGE(TAG, "Unhandled lingo: 0x%02X", lingo);
    }
}

void iPod::handleLingoGeneral(const uint8_t* data, uint32_t len)
{
    uint8_t cmd = data[0];

    switch(cmd)
    {
        case IPOD_CMD_GENERAL_ENTER_REMOTE_UI_MODE:
        {
            const uint8_t resp[] = {
                IPOD_LINGO_GENERAL,
                IPOD_CMD_GENERAL_ACK,
                IPOD_ERROR_OK,
                cmd
            };

            send(resp, sizeof(resp));
            break;
        }
        case IPOD_CMD_GENERAL_REQUEST_IPOD_MODEL_NUM:
        {
            const uint8_t resp[] = {
                IPOD_LINGO_GENERAL,
                IPOD_CMD_GENERAL_RETURN_IPOD_MODEL_NUM,
                0x00, 0x19, 0x00, 0x00, 0x4D, 0x43, 0x30, 0x38, 0x36, 0x52, 0x50, 0x00 // iPod 2G
            };

            send(resp, sizeof(resp));
            break;
        }
        case IPOD_CMD_GENERAL_IDENTIFY_DEVICE_LINGOES:
        {
            const uint8_t resp[] = {
                IPOD_LINGO_GENERAL,
                IPOD_CMD_GENERAL_ACK,
                IPOD_ERROR_OK,
                cmd
            };

            send(resp, sizeof(resp));

            // iPod also sends IPOD_CMD_GENERAL_GET_ACCESSORY_INFO

            break;
        }
        default:
            ESP_LOGE(TAG, "Unhandled general lingo cmd: 0x%02X", cmd);
    }
}

void iPod::handleLingoDisplayRemote(const uint8_t* data, uint32_t len)
{
    uint8_t cmd = data[0];

    switch(cmd)
    {
        case IPOD_CMD_DISPLAY_REMOTE_SET_CURRENT_EQ_PROFILE_INDEX:
        {
            const uint8_t resp[] = {
                IPOD_LINGO_DISPLAY_REMOTE,
                IPOD_CMD_DISPLAY_REMOTE_ACK,
                IPOD_ERROR_OK,
                cmd
            };

            send(resp, sizeof(resp));
            break;
        }
        default:
            ESP_LOGE(TAG, "Unhandled display remote lingo cmd: 0x%02X", cmd);
    }
}

void iPod::handleLingoExtendedInterface(const uint8_t* data, uint32_t len)
{
    // Commands are uint16_t, but higher byte is always 0
    uint8_t cmd = data[1];

    switch(cmd)
    {
        case IPOD_CMD_EXTENDED_INTERFACE_REQUEST_PROTOCOL_VERSION:
        {
            const uint8_t resp[] = {
                IPOD_LINGO_EXTENDED_INTERFACE,
                0x00, IPOD_CMD_EXTENDED_INTERFACE_RETURN_PROTOCOL_VERSION,
                0x01, // major version
                0x1C, // minor version
            };

            send(resp, sizeof(resp));
            break;
        }
        case IPOD_CMD_EXTENDED_INTERFACE_REQUEST_IPOD_NAME:
        {
            uint8_t resp[255] = {
                IPOD_LINGO_EXTENDED_INTERFACE,
                0x00, IPOD_CMD_EXTENDED_INTERFACE_RETURN_IPOD_NAME
            };

            strcpy((char*)resp+3, _name.c_str());

            send(resp, 3+_name.size()+1);
            break;
        }
        case IPOD_CMD_EXTENDED_INTERFACE_RESET_DB_SELECTION:
        {
            sendExtendedInterfaceACK(IPOD_ERROR_OK, cmd);
            break;
        }
        case IPOD_CMD_EXTENDED_INTERFACE_SELECT_DB_RECORD:
        {
            sendExtendedInterfaceACK(IPOD_ERROR_OK, cmd);
            break;
        }
        case IPOD_CMD_EXTENDED_INTERFACE_GET_NUMBER_CATEGORIZED_DB_RECORDS:
        {
            const uint8_t resp[] = {
                IPOD_LINGO_EXTENDED_INTERFACE,
                0x00, IPOD_CMD_EXTENDED_INTERFACE_RETURN_NUMBER_CATEGORIZED_DB_RECORDS,
                0x00, 0x00, 0x00, 0x01 // 1 record
            };

            send(resp, sizeof(resp));
            break;
        }
        case IPOD_CMD_EXTENDED_INTERFACE_RETRIEVE_CATEGORIZED_DB_RECORDS:
        {
            uint8_t category = data[2];
            uint32_t start_index = swap_endian(*((uint32_t*)data+3));
            uint32_t read_count = swap_endian(*((uint32_t*)data+7));

            ESP_LOGD(TAG, "RetrieveCategorizedDBRecords: Cat: 0x%02X, Start: %u, Count: %u", category, start_index, read_count);

            // repeated for each record
            uint8_t resp[255] = {
                IPOD_LINGO_EXTENDED_INTERFACE,
                0x00, IPOD_CMD_EXTENDED_INTERFACE_RETURN_CATEGORIZED_DB_RECORD,
                0x00, 0x00, 0x00, 0x00 // index
            };

            const char name[] = "Unknown";
            strcpy((char*)resp+7, name);

            send(resp, 7+sizeof(name));
            break;
        }
        case IPOD_CMD_EXTENDED_INTERFACE_GET_PLAY_STATUS:
        {
            const uint8_t resp[] = {
                IPOD_LINGO_EXTENDED_INTERFACE,
                0x00, IPOD_CMD_EXTENDED_INTERFACE_RETURN_PLAY_STATUS,
                0x00, 0x00, 0x08, 0x00, // length in ms
                0x00, 0x00, 0x05, 0x00, // position in ms
                IPOD_PLAYER_STATE_PLAYING
            };

            send(resp, sizeof(resp));
            break;
        }
        case IPOD_CMD_EXTENDED_INTERFACE_GET_CURRENT_PLAYING_TRACK_INDEX:
        {
            const uint8_t resp[] = {
                IPOD_LINGO_EXTENDED_INTERFACE,
                0x00, IPOD_CMD_EXTENDED_INTERFACE_RETURN_CURRENT_PLAYING_TRACK_INDEX,
                0x00, 0x00, 0x00, 0x00, // index
            };

            send(resp, sizeof(resp));
            break;
        }
        case IPOD_CMD_EXTENDED_INTERFACE_GET_INDEXED_PLAYING_TRACK_TITLE:
        {
            uint32_t index = swap_endian(*((uint32_t*)data+3));

            ESP_LOGD(TAG, "GetIndexedPlayingTrackTitle: Index: %u", index);

            uint8_t resp[255] = {
                IPOD_LINGO_EXTENDED_INTERFACE,
                0x00, IPOD_CMD_EXTENDED_INTERFACE_RETURN_INDEXED_PLAYING_TRACK_TITLE
            };

            const char name[] = "KAPPA TITLE";
            strcpy((char*)resp+3, name);

            send(resp, 3+sizeof(name));
            break;
        }
        case IPOD_CMD_EXTENDED_INTERFACE_GET_INDEXED_PLAYING_TRACK_ARTIST:
        {
            uint32_t index = swap_endian(*((uint32_t*)data+3));

            ESP_LOGD(TAG, "GetIndexedPlayingTrackArtist: Index: %u", index);

            uint8_t resp[255] = {
                IPOD_LINGO_EXTENDED_INTERFACE,
                0x00, IPOD_CMD_EXTENDED_INTERFACE_RETURN_INDEXED_PLAYING_TRACK_ARTIST
            };

            const char name[] = "KAPPA ARTIST";
            strcpy((char*)resp+3, name);

            send(resp, 3+sizeof(name));
            break;
        }
        case IPOD_CMD_EXTENDED_INTERFACE_GET_INDEXED_PLAYING_TRACK_ALBUM:
        {
            uint32_t index = swap_endian(*((uint32_t*)data+3));

            ESP_LOGD(TAG, "GetIndexedPlayingTrackArtist: Album: %u", index);

            uint8_t resp[255] = {
                IPOD_LINGO_EXTENDED_INTERFACE,
                0x00, IPOD_CMD_EXTENDED_INTERFACE_RETURN_INDEXED_PLAYING_TRACK_ALBUM
            };

            const char name[] = "KAPPA ALBUM";
            strcpy((char*)resp+3, name);

            send(resp, 3+sizeof(name));
            break;
        }
        case IPOD_CMD_EXTENDED_INTERFACE_SET_PLAY_STATUS_CHANGE_NOTIFICATION:
        {
            // this command can be 4 byte (event mask). not handled
            uint8_t set = data[2];

            ESP_LOGD(TAG, "SetPlayStatusChangeNotification: 0x%02X", set);

            if (set == 0x00)
                _playStatusNotifications = false;
            else if (set == 0x01)
                _playStatusNotifications = true;
            else
                ESP_LOGE(TAG, "Unknown SetPlayStatusChangeNotification value: 0x%02X", set);
        }
        case IPOD_CMD_EXTENDED_INTERFACE_PLAY_CURRENT_SELECTION:
        {
            sendExtendedInterfaceACK(IPOD_ERROR_OK, cmd);
            break;
        }
        case IPOD_CMD_EXTENDED_INTERFACE_PLAY_CONTROL:
        {
            uint8_t code = data[2];

            ESP_LOGD(TAG, "PlayControl: 0x%02X", code);

            sendExtendedInterfaceACK(IPOD_ERROR_OK, cmd);
            break;
        }
        case IPOD_CMD_EXTENDED_INTERFACE_SET_SHUFFLE:
        {
            uint8_t mode = data[2];
            uint8_t restoreOnExit = data[3];

            ESP_LOGD(TAG, "SetShuffle: Mode: 0x%02X, Restore: 0x%02X", mode, restoreOnExit);

            sendExtendedInterfaceACK(IPOD_ERROR_OK, cmd);
            break;
        }
        case IPOD_CMD_EXTENDED_INTERFACE_SET_REPEAT:
        {
            uint8_t repeat = data[2];
            uint8_t restoreOnExit = data[3];

            ESP_LOGD(TAG, "SetRepeat: Repeat: 0x%02X, Restore: 0x%02X", repeat, restoreOnExit);

            sendExtendedInterfaceACK(IPOD_ERROR_OK, cmd);
            break;
        }
        case IPOD_CMD_EXTENDED_INTERFACE_SET_DISPLAY_IMAGE:
        {
            sendExtendedInterfaceACK(IPOD_ERROR_OK, cmd);
            break;
        }
        case IPOD_CMD_EXTENDED_INTERFACE_GET_MONO_DISPLAY_IMAGE_LIMITS:
        {
            const uint8_t resp[] = {
                IPOD_LINGO_EXTENDED_INTERFACE,
                0x00, IPOD_CMD_EXTENDED_INTERFACE_RETURN_MONO_DISPLAY_IMAGE_LIMITS,
                0x00, 0xA6, // max width
                0x00, 0x4C, // max height
                0x01 // pixel format
            };

            send(resp, sizeof(resp));
            break;
        }
        case IPOD_CMD_EXTENDED_INTERFACE_GET_NUM_PLAYING_TRACKS:
        {
            const uint8_t resp[] = {
                IPOD_LINGO_EXTENDED_INTERFACE,
                0x00, IPOD_CMD_EXTENDED_INTERFACE_RETURN_NUM_PLAYING_TRACKS,
                0x00, 0x00, 0x00, 0x01
            };

            send(resp, sizeof(resp));
            break;
        }
        case IPOD_CMD_EXTENDED_INTERFACE_SET_CURRENT_PLAYING_TRACK:
        {
            uint32_t index = swap_endian(*((uint32_t*)data+3));

            ESP_LOGD(TAG, "SetCurrentPlayingTrack: %u", index);

            sendExtendedInterfaceACK(IPOD_ERROR_OK, cmd);
            break;
        }
        case IPOD_CMD_EXTENDED_INTERFACE_GET_COLOR_DISPLAY_IMAGE_LIMITS:
        {
            const uint8_t resp[] = {
                IPOD_LINGO_EXTENDED_INTERFACE,
                0x00, IPOD_CMD_EXTENDED_INTERFACE_RETURN_COLOR_DISPLAY_IMAGE_LIMITS,
                0x00, 0xA6, // max width
                0x00, 0x4C, // max height
                0x02, // pixel format
                0x00, 0xA6, // max width2
                0x00, 0x4C, // max height2
                0x03 // pixel format2
            };

            send(resp, sizeof(resp));
            break;
        }
        default:
            ESP_LOGE(TAG, "Unhandled extended interface lingo cmd: 0x%02X", cmd);
    }
}

void iPod::sendTrackIndex(uint32_t index)
{
    uint8_t resp[20] = {
        IPOD_LINGO_EXTENDED_INTERFACE,
        0x00, IPOD_CMD_EXTENDED_INTERFACE_PLAY_STATUS_CHANGE_NOTIFICATION,
        IPOD_PLAY_STATUS_NOTIFICATION_TRACK_INDEX,
    };

    *(resp+3) = swap_endian<uint32_t>(index);

    send(resp, 4+4);
}

void iPod::sendTrackTimeOffsetMS(uint32_t offset)
{
    uint8_t resp[20] = {
        IPOD_LINGO_EXTENDED_INTERFACE,
        0x00, IPOD_CMD_EXTENDED_INTERFACE_PLAY_STATUS_CHANGE_NOTIFICATION,
        IPOD_PLAY_STATUS_NOTIFICATION_TRACK_TIME_OFFSET_MS,
    };

    *(resp+3) = swap_endian<uint32_t>(offset);

    send(resp, 4+4);
}

void iPod::update()
{
    // process serial data
    while (_ser.available())
    {
        char c = _ser.read();

        if (_lastChar == 0xFF && c == 0x55)
        {
            _recvItr = 0;
            _recvSize = 0;
        }
        else if (_recvItr == 0 && _recvSize == 0)
        {
            _recvSize = c;
            ESP_LOGD(TAG, "Begin receiving. Len: %u", _recvSize);
        }
        else
        {
            _recv[_recvItr++] = c;

            // plus checksum
            if (_recvItr == _recvSize+1)
            {
                ESP_LOGD(TAG, "Received: ");

                uint8_t sum = iPod::checksum(_recv, _recvSize);

                if (sum == _recv[_recvSize])
                    handlePacket(_recv, _recvSize);
                else
                    ESP_LOGE(TAG, "Checksum failed. Local: 0x%02X, Remote: 0x%02X", sum, _recv[_recvSize]);
            }
        }

        _lastChar = c;
    }

    // notifications
    if (_playStatusNotificationTimer < millis() && _playStatusNotifications)
    {
        // postpone timer
        _playStatusNotificationTimer = millis() + PLAY_STATUS_NOTIFICATION_INTERVAL;

        sendTrackTimeOffsetMS(2500);
    }
}

void iPod::send(const uint8_t* data, uint32_t len)
{
    _ser.write(0xFF); // sync
    _ser.write(0x55); // header
    _ser.write(uint8_t(len));
    _ser.write(data, len);
    _ser.write(iPod::checksum(data, len));
}

void iPod::sendExtendedInterfaceACK(uint8_t error, uint8_t cmd)
{
    const uint8_t resp[] = {
        IPOD_LINGO_EXTENDED_INTERFACE,
        0x00, IPOD_CMD_EXTENDED_INTERFACE_ACK,
        error,
        0x00, cmd
    };

    send(resp, sizeof(resp));
}
