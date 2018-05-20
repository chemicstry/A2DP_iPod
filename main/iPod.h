#ifndef _IPOD_H_
#define _IPOD_H_

#include "Arduino.h"
#include <string>

#define MAX_PACKET_SIZE 300

#define PLAY_STATUS_NOTIFICATION_INTERVAL 500

enum IPOD_LINGO : uint8_t
{
    IPOD_LINGO_GENERAL              = 0x00,
    IPOD_LINGO_MICROPHONE           = 0x01,
    IPOD_LINGO_SIMPLE_REMOTE        = 0x02,
    IPOD_LINGO_DISPLAY_REMOTE       = 0x03,
    IPOD_LINGO_EXTENDED_INTERFACE   = 0x04,
    IPOD_LINGO_ACCESSORY_POWER      = 0x05,
    IPOD_LINGO_RES1                 = 0x06, // unknown
    IPOD_LINGO_RF_TUNER             = 0x07,
    IPOD_LINGO_ACCESSORY_EQUALIZER  = 0x08,
    IPOD_LINGO_SPORTS               = 0x09,
    IPOD_LINGO_DIGITAL_AUDIO        = 0x0A,
    IPOD_LINGO_STORAGE              = 0x0C,
    IPOD_LINGO_LOCATION             = 0x0E,
};

enum IPOD_CMD_GENERAL : uint8_t
{
    IPOD_CMD_GENERAL_ACK                        = 0x02,
    IPOD_CMD_GENERAL_ENTER_REMOTE_UI_MODE       = 0x05,
    IPOD_CMD_GENERAL_REQUEST_IPOD_MODEL_NUM     = 0x0D,
    IPOD_CMD_GENERAL_RETURN_IPOD_MODEL_NUM      = 0x0E,
    IPOD_CMD_GENERAL_IDENTIFY_DEVICE_LINGOES    = 0x13,
    IPOD_CMD_GENERAL_GET_ACCESSORY_INFO         = 0x27,
    IPOD_CMD_GENERAL_RET_ACCESSORY_INFO         = 0x28
};

enum IPOD_CMD_DISPLAY_REMOTE : uint8_t
{
    IPOD_CMD_DISPLAY_REMOTE_ACK                             = 0x00,
    IPOD_CMD_DISPLAY_REMOTE_SET_CURRENT_EQ_PROFILE_INDEX    = 0x03,
};

enum IPOD_CMD_EXTENDED_INTERFACE : uint8_t
{
    IPOD_CMD_EXTENDED_INTERFACE_ACK                                     = 0x01,
    IPOD_CMD_EXTENDED_INTERFACE_REQUEST_PROTOCOL_VERSION                = 0x12,
    IPOD_CMD_EXTENDED_INTERFACE_RETURN_PROTOCOL_VERSION                 = 0x13,
    IPOD_CMD_EXTENDED_INTERFACE_REQUEST_IPOD_NAME                       = 0x14,
    IPOD_CMD_EXTENDED_INTERFACE_RETURN_IPOD_NAME                        = 0x15,
    IPOD_CMD_EXTENDED_INTERFACE_RESET_DB_SELECTION                      = 0x16,
    IPOD_CMD_EXTENDED_INTERFACE_SELECT_DB_RECORD                        = 0x17,
    IPOD_CMD_EXTENDED_INTERFACE_GET_NUMBER_CATEGORIZED_DB_RECORDS       = 0x18,
    IPOD_CMD_EXTENDED_INTERFACE_RETURN_NUMBER_CATEGORIZED_DB_RECORDS    = 0x19,
    IPOD_CMD_EXTENDED_INTERFACE_RETRIEVE_CATEGORIZED_DB_RECORDS         = 0x1A,
    IPOD_CMD_EXTENDED_INTERFACE_RETURN_CATEGORIZED_DB_RECORD            = 0x1B,
    IPOD_CMD_EXTENDED_INTERFACE_GET_PLAY_STATUS                         = 0x1C,
    IPOD_CMD_EXTENDED_INTERFACE_RETURN_PLAY_STATUS                      = 0x1D,
    IPOD_CMD_EXTENDED_INTERFACE_GET_CURRENT_PLAYING_TRACK_INDEX         = 0x1E,
    IPOD_CMD_EXTENDED_INTERFACE_RETURN_CURRENT_PLAYING_TRACK_INDEX      = 0x1F,
    IPOD_CMD_EXTENDED_INTERFACE_GET_INDEXED_PLAYING_TRACK_TITLE         = 0x20,
    IPOD_CMD_EXTENDED_INTERFACE_RETURN_INDEXED_PLAYING_TRACK_TITLE      = 0x21,
    IPOD_CMD_EXTENDED_INTERFACE_GET_INDEXED_PLAYING_TRACK_ARTIST        = 0x22,
    IPOD_CMD_EXTENDED_INTERFACE_RETURN_INDEXED_PLAYING_TRACK_ARTIST     = 0x23,
    IPOD_CMD_EXTENDED_INTERFACE_GET_INDEXED_PLAYING_TRACK_ALBUM         = 0x24,
    IPOD_CMD_EXTENDED_INTERFACE_RETURN_INDEXED_PLAYING_TRACK_ALBUM      = 0x25,
    IPOD_CMD_EXTENDED_INTERFACE_SET_PLAY_STATUS_CHANGE_NOTIFICATION     = 0x26,
    IPOD_CMD_EXTENDED_INTERFACE_PLAY_STATUS_CHANGE_NOTIFICATION         = 0x27,
    IPOD_CMD_EXTENDED_INTERFACE_PLAY_CURRENT_SELECTION                  = 0x28,
    IPOD_CMD_EXTENDED_INTERFACE_PLAY_CONTROL                            = 0x29,
    IPOD_CMD_EXTENDED_INTERFACE_GET_SHUFFLE                             = 0x2C,
    IPOD_CMD_EXTENDED_INTERFACE_RETURN_SHUFFLE                          = 0x2D,
    IPOD_CMD_EXTENDED_INTERFACE_SET_SHUFFLE                             = 0x2E,
    IPOD_CMD_EXTENDED_INTERFACE_GET_REPEAT                              = 0x2F,
    IPOD_CMD_EXTENDED_INTERFACE_RETURN_REPEAT                           = 0x30,
    IPOD_CMD_EXTENDED_INTERFACE_SET_REPEAT                              = 0x31,
    IPOD_CMD_EXTENDED_INTERFACE_SET_DISPLAY_IMAGE                       = 0x32,
    IPOD_CMD_EXTENDED_INTERFACE_GET_MONO_DISPLAY_IMAGE_LIMITS           = 0x33,
    IPOD_CMD_EXTENDED_INTERFACE_RETURN_MONO_DISPLAY_IMAGE_LIMITS        = 0x34,
    IPOD_CMD_EXTENDED_INTERFACE_GET_NUM_PLAYING_TRACKS                  = 0x35,
    IPOD_CMD_EXTENDED_INTERFACE_RETURN_NUM_PLAYING_TRACKS               = 0x36,
    IPOD_CMD_EXTENDED_INTERFACE_SET_CURRENT_PLAYING_TRACK               = 0x37,
    IPOD_CMD_EXTENDED_INTERFACE_SELECT_SORT_DB_RECORD                   = 0x38,
    IPOD_CMD_EXTENDED_INTERFACE_GET_COLOR_DISPLAY_IMAGE_LIMITS          = 0x39,
    IPOD_CMD_EXTENDED_INTERFACE_RETURN_COLOR_DISPLAY_IMAGE_LIMITS       = 0x3A,
};

enum IPOD_PLAYER_STATE : uint8_t
{
    IPOD_PLAYER_STATE_STOPPED       = 0x00,
    IPOD_PLAYER_STATE_PLAYING       = 0x01,
    IPOD_PLAYER_STATE_PAUSED        = 0x02,
    IPOD_PLAYER_STATE_ERROR         = 0xFF
};

enum IPOD_PLAY_STATUS_NOTIFICATION : uint8_t
{
    IPOD_PLAY_STATUS_NOTIFICATION_PLAYBACK_STOPPED                  = 0x00,
    IPOD_PLAY_STATUS_NOTIFICATION_TRACK_INDEX                       = 0x01,
    IPOD_PLAY_STATUS_NOTIFICATION_PLAYBACK_FFW_SEEK_STOP            = 0x02,
    IPOD_PLAY_STATUS_NOTIFICATION_PLAYBACK_REW_SEEK_STOP            = 0x03,
    IPOD_PLAY_STATUS_NOTIFICATION_TRACK_TIME_OFFSET_MS              = 0x04,
    IPOD_PLAY_STATUS_NOTIFICATION_CHAPTER_INDEX                     = 0x05,
    IPOD_PLAY_STATUS_NOTIFICATION_PLAYBACK_STATUS_EXTENDED          = 0x06,
    IPOD_PLAY_STATUS_NOTIFICATION_TRACK_TIME_OFFSET_SEC             = 0x07,
    IPOD_PLAY_STATUS_NOTIFICATION_CHAPTER_TIME_OFFSET_MS            = 0x08,
    IPOD_PLAY_STATUS_NOTIFICATION_CHAPTER_TIME_OFFSET_SEC           = 0x09,
    IPOD_PLAY_STATUS_NOTIFICATION_TRACK_UID                         = 0x0A,
    IPOD_PLAY_STATUS_NOTIFICATION_TRACK_PLAYBACK_MODE               = 0x0B,
    IPOD_PLAY_STATUS_NOTIFICATION_TRACK_LYRICS_READY                = 0x0C,
};

enum IPOD_PLAY_CONTROL : uint8_t
{
    IPOD_PLAY_CONTROL_RESERVED                  = 0x00,
    IPOD_PLAY_CONTROL_TOGGLE_PLAY_PAUSE         = 0x01,
    IPOD_PLAY_CONTROL_STOP                      = 0x02,
    IPOD_PLAY_CONTROL_NEXT_TRACK                = 0x03,
    IPOD_PLAY_CONTROL_PREVIOUS_TRACK            = 0x04,
    IPOD_PLAY_CONTROL_START_FF                  = 0x05,
    IPOD_PLAY_CONTROL_START_REW                 = 0x06,
    IPOD_PLAY_CONTROL_END_FF_REW                = 0x07,
    IPOD_PLAY_CONTROL_NEXT                      = 0x08, // changes chapter
    IPOD_PLAY_CONTROL_PREVIOUS                  = 0x09, // changes chapter
    IPOD_PLAY_CONTROL_PLAY                      = 0x0A,
    IPOD_PLAY_CONTROL_PAUSE                     = 0x0B,
    IPOD_PLAY_CONTROL_NEXT_CHAPTER              = 0x0C,
    IPOD_PLAY_CONTROL_PREVIOUS_CHAPTER          = 0x0D
}

enum IPOD_ERROR : uint8_t
{
    IPOD_ERROR_OK                   = 0x00,
    IPOD_ERROR_UNKNOWN_DB_CATEGORY  = 0x01,
    IPOD_ERROR_COMMAND_FAILED       = 0x02,
    IPOD_ERROR_OUT_OF_RESOURCES     = 0x03,
    IPOD_ERROR_BAD_PARAMETER        = 0x04,
    IPOD_ERROR_UNKNOWN_ID           = 0x05,
    IPOD_ERROR_COMMAND_PENDING      = 0x06
    // more errors exist
};

class iPod
{
public:
    iPod(HardwareSerial& ser);

    // Calculates checksum. Length is not included in data
    static uint8_t checksum(const uint8_t* data, uint32_t len);

    void handlePacket(const uint8_t* data, uint32_t len);
    void handleLingoGeneral(const uint8_t* data, uint32_t len);
    void handleLingoDisplayRemote(const uint8_t* data, uint32_t len);
    void handleLingoExtendedInterface(const uint8_t* data, uint32_t len);

    // events
    void sendTrackIndex(uint32_t index);
    void sendTrackTimeOffsetMS(uint32_t offset);

    void update();

    void send(const uint8_t* data, uint32_t len);
    void sendExtendedInterfaceACK(uint8_t error, uint8_t cmd);

private:
    HardwareSerial& _ser;

    // iPod constants
    std::string _name;

    // state
    bool _playStatusNotifications;
    uint32_t _playStatusNotificationTimer;

    // Handle serial recv
    uint8_t _recv[MAX_PACKET_SIZE];
    uint8_t _lastChar;
    uint32_t _recvItr;
    uint32_t _recvSize;
};

#endif
