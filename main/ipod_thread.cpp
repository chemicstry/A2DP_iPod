#include "ipod_thread.h"
#include "iPod.h"
#include <thread>

HardwareSerial ipod_ser(2);
iPod ipod(ipod_ser);
std::thread ipod_thread;

void ipod_thread_func()
{
    ipod_ser.begin(57600);

    while(1)
    {
        ipod.update();

        // yield
        delay(1);
    }
}

extern "C" void start_ipod_thread()
{
    ipod_thread = std::thread(ipod_thread_func);
}
