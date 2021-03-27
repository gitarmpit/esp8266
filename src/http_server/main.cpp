#include "../common/ft232.h"
#include "../common/Esp8266.h"
#include <stdio.h>
#include "HttpServer.h"
#include "../common/PersistedSettings.h"

static void http_server(int argc, char** argv)
{
    Ft232 ft232;
    if (!ft232.Initialize(115200))
    {
        printf("init error\n");
        return;
    }

    ft232.SetTimeout(30000);

    PersistedSettings ps;

    if (argc == 3)
    {
        char* ssid = argv[1];
        char* pass = argv[2];
        printf("passed from cmd line: %s %s\n", ssid, pass);
        ps.SetApSettings(ssid, pass);
    }

    bool echoOff = true;
    bool autoConnect = false;
    Esp8266 esp(&ft232, autoConnect, echoOff);
    //esp.SetAP_SSID_Password("Nash_1", "427215427215");

    HttpServer httpServer(&esp, &ps, 2);
    httpServer.Run();
}

int main(int argc, char**argv) 
{
    http_server(argc, argv);
}