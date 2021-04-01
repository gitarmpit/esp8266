#include <string.h>
#include <stdio.h>
#include "WinSockIo.h"
#include "../common/Esp8266.h"
#include "../http_server/HttpServer.h"

int main(int argc, char**argv)
{
    bool echoOff = true;
    bool autoConnect = false;
    WinSock_Esp8266 ws (80, 4);
    if (!ws.Initialize(0))
    {
        printf("Initialize error\n");
        exit(1);
    }

    Esp8266 esp(&ws, autoConnect, echoOff);

    PersistedSettings ps;

    if (argc == 3)
    {
        char* ssid = argv[1];
        char* pass = argv[2];
        printf("passed from cmd line: %s %s\n", ssid, pass);
        ps.SetApSettings(ssid, pass);
    }

    HttpServer httpServer(&esp, &ps, 2);
    httpServer.Run();
    ws.Stop();

}
