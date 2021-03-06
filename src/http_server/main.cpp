#include "../common/ft232.h"
#include "../common/Esp8266.h"
#include <stdio.h>
#include "HttpServer.h"
#include "../common/PersistedSettings.h"
#include "../common/Timer.h"
#include "html.cpp"


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
    Timer timer;
    Esp8266 esp(&ft232, &timer, autoConnect, echoOff);
    //esp.SetAP_SSID_Password("Nash_1", "427215427215");

    Timer timer2;
    HttpServer httpServer(&esp, main_html, &ps, &timer2, 2);
    httpServer.Run();
}

int main(int argc, char**argv) 
{
    http_server(argc, argv);
}