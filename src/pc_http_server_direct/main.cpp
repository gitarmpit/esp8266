#include <string.h>
#include <stdio.h>
#include "../pc_http_server/WinSockIo.h"
#include "../common/Esp8266.h"
#include "../http_server/HttpServer.h"

Esp8266::Esp8266(Serial* serial, bool autoConnect, bool echoOff)
{
    _serial = serial;
    UNUSED(autoConnect);
    UNUSED(echoOff);
}

bool Esp8266::SendCommand(const char* buf, int size)
{
    UNUSED(buf);
    UNUSED(size);

    return true;
}

bool Esp8266::WaitBoot()
{
    return true;
}
void Esp8266::Reset()
{
}
bool Esp8266::EchoOff()
{
    return true;
}

bool Esp8266::ConnectToAP(const char* ssid, const char* password, bool saveToFlash, int timeoutMs)
{
    UNUSED(ssid);
    UNUSED(password);
    UNUSED(saveToFlash);
    UNUSED(timeoutMs);

    return true;
}

bool Esp8266::DisconnectFromAP()
{
    return true;
}

bool Esp8266::SetMode(MODE mode)
{
    UNUSED(mode);
    return true;
}

bool Esp8266::SetAutoConnect(bool on)
{
    UNUSED(on);
    return true;
}

bool Esp8266::IsConnectedToAP()
{
    return true;
}

bool Esp8266::SetMux(bool isMux)
{
    UNUSED(isMux);
    return true;
}

bool Esp8266::StopServer()
{
    return true;
}

bool Esp8266::StartServer(int port, int maxConnections, int timeoutSec)
{
    UNUSED(port);
    UNUSED(maxConnections);
    UNUSED(timeoutSec);

    return true;
}
bool Esp8266::ConnectTCP(const char* hostname, int port, int timeoutMs, int linkId)
{
    UNUSED(hostname);
    UNUSED(port);
    UNUSED(timeoutMs);
    UNUSED(linkId);

    return true;
}
bool Esp8266::ConnectUDP(const char* hostname, int port, int timeoutMs, int linkId)
{
    UNUSED(hostname);
    UNUSED(port);
    UNUSED(timeoutMs);
    UNUSED(linkId);
    return true;
}
bool Esp8266::CloseConnection(int linkId)
{
    UNUSED(linkId);
    return true;
}
bool Esp8266::PassthroughMode(bool on)
{
    UNUSED(on);
    return true;
}

bool Esp8266::IsConnectionClosed(int linkId)
{
    UNUSED(linkId);
    return false;
}

bool Esp8266::SetStationIpAddress(const char* ipAddress)
{
    UNUSED(ipAddress);
    return true;
}

// Never called
bool Esp8266::Expect(const char* expect, int timeoutMs, bool resetPointer)
{
    UNUSED(expect);
    UNUSED(timeoutMs);
    UNUSED(resetPointer);
    return false;
}

Timer& Esp8266::CreateTimer()
{
    static Timer timer;
    return timer;
}

void Esp8266::Log(const char* format, ...)
{
    UNUSED(format);
}

void Esp8266::_Log(const char* str)
{
    UNUSED(str);
}

bool Esp8266::SendData(const char* buf, const int size, int timeoutMs, int linkId, bool waitAck)
{
    UNUSED(waitAck);
    UNUSED(linkId);
    UNUSED(timeoutMs);
    return _serial->Send(buf, size);
}

bool Esp8266::ReceiveData(char* buf, int& bytesRead, int& linkId, int timeoutMs, bool waitForOK)
{
    UNUSED(waitForOK);
    linkId = 0;
    return _serial->Read(buf, bytesRead, timeoutMs);
}


int main(int argc, char**argv)
{
    bool echoOff = true;
    bool autoConnect = false;
    WinSockIo ws (80, 4);
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

}
