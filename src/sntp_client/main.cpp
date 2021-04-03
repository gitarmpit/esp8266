#include "SntpClient.h"
#include "../common/ft232.h"
#include "../common/Esp8266.h"
#include "../common/Timer.h"

#include <stdio.h>

static void start_sntp()
{
    Ft232 ft232;
    if (!ft232.Initialize(115200))
    {
        printf("init error\n");
        return;
    }

    ft232.SetTimeout(30000);

    bool echoOff = true;
    bool autoConnect = false;
    Timer timer;
    Esp8266 esp(&ft232, &timer, autoConnect, echoOff);

    PersistedSettings settings;
    settings.SetApSettings("Nash_1", "427215427215");
    int US_timeZone = 4;
    SntpClient sntp(&esp, &settings, US_timeZone, 1);
    sntp.Run();

    int hr, min, sec;
    sntp.Init();
    if (sntp.GetTime(hr, min, sec))
    {
        printf("%02d:%02d:%02d\n", hr, min, sec);
    }

    int hl, hh, ml, mh, sl, sh;
    if (sntp.GetTime(hh, hl, mh, ml, sh, sl))
    {
        printf("%d%d:%d%d:%d%d\n", hh, hl, mh, ml, sh, sl);
    }
}

int main()
{
    start_sntp();
}