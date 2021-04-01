#include <string.h>
#include <stdio.h>
#include "../pc_http_server/WinSockIo.h"
#include "../common/Esp8266.h"
#include "../http_server/HttpServer.h"

class Esp8266_Direct : public Esp8266_Base
{
public:
#ifdef UNIT_TESTS
    friend class HybridMockEsp8266;
#endif

    Esp8266_Direct(Serial* serial, bool autoConnect = false, bool echoOff = false);
    virtual bool SendCommand(const char* buf, int len = 0);
    virtual bool Expect(const char* expect, int timeoutMs = 1000, bool resetPointer = true);
    virtual bool WaitBoot();
    virtual bool EchoOff();
    virtual void Reset();
    virtual bool FactoryRestore() { return true;  }
    virtual bool DisconnectFromAP();
    virtual bool ConnectToAP(const char* ssid, const char* password, bool saveToFlash, int timeoutMs = 10000);
    virtual bool SetMode(MODE mode);
    virtual bool SetAutoConnect(bool on);
    virtual bool IsConnectedToAP();
    virtual bool SetStationIpAddress(const char* ipAddress);
    virtual bool StartServer(int port, int maxConnections, int timeoutSec);
    virtual bool StopServer();
    virtual bool PassthroughMode(bool on);
    virtual bool ConnectTCP(const char* hostname, int port, int timeoutMs, int linkId = -1);
    virtual bool ConnectUDP(const char* hostname, int port, int timeoutMs, int linkId = -1);
    virtual bool SendData(const char* buf, const int size, int timeoutMs, int linkId = -1, bool waitAck = false);
    virtual bool ReceiveData(char* buf, int& size, int& linkId, int timeoutMs, bool waitForOK);
    //virtual bool WaitForClientConnection(int* linkId, int timeoutMs);
    virtual bool CloseConnection(int linkId);
#ifdef ESP8266_NEW
    virtual bool SetTCPReceiveMode(bool isActive);
    virtual bool ReceiveTCPData(char* buf, int bytesToRead, int* bytesRead, int linkId = -1);
    // Doesnt work with old firmware, need to update first 
    // but AT+CIUPDATE doesn't work either
    bool SetupSNTP(const char* server, int timeZone);
    bool GetSNTPTime(char* buf, int timeoutMs);
#endif
    virtual bool IsConnectionClosed(int linkId);
    virtual void SetChunkSize(int chunkSize);


    // Multiple connection mode
    virtual bool SetMux(bool isMux);
    virtual bool GetListOfAps(char* buf, int buflen);

    virtual Timer& CreateTimer();
    virtual ~Esp8266_Direct() {}
private:
    Serial* _serial;

};

Esp8266_Direct::Esp8266_Direct(Serial* serial, bool autoConnect, bool echoOff)
{
    _serial = serial;
    UNUSED(autoConnect);
    UNUSED(echoOff);
}

bool Esp8266_Direct::SendCommand(const char* buf, int size)
{
    UNUSED(buf);
    UNUSED(size);

    return true;
}

bool Esp8266_Direct::WaitBoot()
{
    return true;
}
void Esp8266_Direct::Reset()
{
}
bool Esp8266_Direct::EchoOff()
{
    return true;
}

void Esp8266_Direct::SetChunkSize(int chunkSize)
{
    UNUSED(chunkSize);
}

bool Esp8266_Direct::GetListOfAps(char* buf, int buflen)
{
    UNUSED(buf);
    UNUSED(buflen);
    return true;
}

bool Esp8266_Direct::ConnectToAP(const char* ssid, const char* password, bool saveToFlash, int timeoutMs)
{
    UNUSED(ssid);
    UNUSED(password);
    UNUSED(saveToFlash);
    UNUSED(timeoutMs);

    return true;
}

bool Esp8266_Direct::DisconnectFromAP()
{
    return true;
}

bool Esp8266_Direct::SetMode(MODE mode)
{
    UNUSED(mode);
    return true;
}

bool Esp8266_Direct::SetAutoConnect(bool on)
{
    UNUSED(on);
    return true;
}

bool Esp8266_Direct::IsConnectedToAP()
{
    return true;
}

bool Esp8266_Direct::SetMux(bool isMux)
{
    UNUSED(isMux);
    return true;
}

bool Esp8266_Direct::StopServer()
{
    return true;
}

bool Esp8266_Direct::StartServer(int port, int maxConnections, int timeoutSec)
{
    UNUSED(port);
    UNUSED(maxConnections);
    UNUSED(timeoutSec);

    return true;
}
bool Esp8266_Direct::ConnectTCP(const char* hostname, int port, int timeoutMs, int linkId)
{
    UNUSED(hostname);
    UNUSED(port);
    UNUSED(timeoutMs);
    UNUSED(linkId);

    return true;
}
bool Esp8266_Direct::ConnectUDP(const char* hostname, int port, int timeoutMs, int linkId)
{
    UNUSED(hostname);
    UNUSED(port);
    UNUSED(timeoutMs);
    UNUSED(linkId);
    return true;
}
bool Esp8266_Direct::CloseConnection(int linkId)
{
    UNUSED(linkId);
    return true;
}
bool Esp8266_Direct::PassthroughMode(bool on)
{
    UNUSED(on);
    return true;
}

bool Esp8266_Direct::IsConnectionClosed(int linkId)
{
    UNUSED(linkId);
    return false;
}

bool Esp8266_Direct::SetStationIpAddress(const char* ipAddress)
{
    UNUSED(ipAddress);
    return true;
}

// Never called
bool Esp8266_Direct::Expect(const char* expect, int timeoutMs, bool resetPointer)
{
    UNUSED(expect);
    UNUSED(timeoutMs);
    UNUSED(resetPointer);
    return false;
}

Timer& Esp8266_Direct::CreateTimer()
{
    static Timer timer;
    return timer;
}

bool Esp8266_Direct::SendData(const char* buf, const int size, int timeoutMs, int linkId, bool waitAck)
{
    UNUSED(waitAck);
    UNUSED(linkId);
    UNUSED(timeoutMs);
    return _serial->Send(buf, size);
}

bool Esp8266_Direct::ReceiveData(char* buf, int& bytesRead, int& linkId, int timeoutMs, bool waitForOK)
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

    Esp8266_Direct esp(&ws, autoConnect, echoOff);

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
