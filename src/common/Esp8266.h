#pragma once 
#include "Serial.h"
#include "Timer.h"

#define DATABUFSIZE 8192
#define CMDBUFSIZE 128
#define IPD "+IPD,"

#define DEFAULT_CHUNK_SIZE 1024

#define AT_OK         "\r\nOK\r\n"
#define AT_CIPSTA_CUR "AT+CIPSTA_CUR"
#define AT_CRLF       "\r\n"
#define AT_RST        "AT+RST\r\n"
#define AT_ATE0       "ATE0\r\n"
#define AT_RESTORE    "AT+RESTORE\r\n"
#define AT_CWQAP      "AT+CWQAP\r\n"
#define AT_CWMODE_CUR "AT+CWMODE_CUR"
#define AT_CWAUTOCONN "AT+CWAUTOCONN"
#define AT_CWJAP_CUR_GET  "AT+CWJAP_CUR?\r\n"
#define AT_CIPMUX     "AT+CIPMUX"
#define AT_CIPSERVER  "AT+CIPSERVER"
#define AT_CIPSTART   "AT+CIPSTART"
#define AT_CIPSTART_TCP "AT+CIPSTART=\"TCP\""
#define AT_CIPSTART_UDP "AT+CIPSTART=\"UDP\""
#define AT_CIPCLOSE   "AT+CIPCLOSE"
#define AT_CIPSEND    "AT+CIPSEND"
#define AT_CWJAP_DEF  "AT+CWJAP_DEF"
#define AT_CWJAP_CUR  "AT+CWJAP_CUR"

#define SEND_OK        "SEND OK\r\n"
#define WIFI_CONNECTED "WIFI CONNECTED\r\n"
#define WIFI_GOT_IP    "WIFI GOT IP\r\n"
#define CWJAP_CUR_PATTERN "+CWJAP_CUR:\""

#define DEBUG_MODE

class Esp8266_Base
{
public:
    enum class MODE
    {
        MODE_STATION = 1,
        MODE_AP = 2,
        MODE_STATION_AP = 3
    };

    virtual ~Esp8266_Base() {}
    virtual bool SendCommand(const char* buf, int len = 0) = 0;
    virtual bool Expect(const char* expect, int timeoutMs = 1000, bool resetPointer = true) = 0;
    virtual bool WaitBoot() = 0;
    virtual bool EchoOff() = 0;
    virtual void Reset() = 0;
    virtual bool FactoryRestore() = 0;
    virtual bool DisconnectFromAP() = 0;
    virtual bool ConnectToAP(const char* ssid, const char* password, bool saveToFlash, int timeoutMs = 10000) = 0;
    virtual bool SetMode(MODE mode) = 0;
    virtual bool SetAutoConnect(bool on) = 0;
    virtual bool IsConnectedToAP() = 0;
    virtual bool SetStationIpAddress(const char* ipAddress) = 0;
    virtual bool StartServer(int port, int maxConnections, int timeoutSec) = 0;
    virtual bool StopServer() = 0;
    virtual bool PassthroughMode(bool on) = 0;
    virtual bool ConnectTCP(const char* hostname, int port, int timeoutMs, int linkId = -1) = 0;
    virtual bool ConnectUDP(const char* hostname, int port, int timeoutMs, int linkId = -1) = 0;
    virtual bool SendData(const char* buf, const int size, int timeoutMs, int linkId = -1, bool waitAck = false) = 0;
    virtual bool ReceiveData(char* buf, int& size, int& linkId, int timeoutMs, bool waitForOK) = 0;
    // virtual bool WaitForClientConnection(int* linkId, int timeoutMs) = 0;
    virtual bool CloseConnection(int linkId) = 0;
#ifdef ESP8266_NEW
    virtual bool SetTCPReceiveMode(bool isActive) = 0;
    virtual bool ReceiveTCPData(char* buf, int bytesToRead, int* bytesRead, int linkId = -1) = 0;
#endif
    virtual bool IsConnectionClosed(int linkId) = 0;
    virtual void SetChunkSize(int chunkSize) = 0;
    virtual bool SetMux(bool isMux) = 0;
    virtual bool GetListOfAps(char* buf, int buflen) = 0;
};

class Esp8266 : public Esp8266_Base
{
public:
#ifdef UNIT_TESTS
    friend class HybridMockEsp8266;
#endif

    Esp8266(Serial* serial, Timer* timer, bool autoConnect = false, bool echoOff = false);
    virtual bool SendCommand(const char* buf, int len = 0);
    virtual bool Expect(const char* expect, int timeoutMs = 1000, bool resetPointer = true);
    virtual bool WaitBoot();
    virtual bool EchoOff();
    virtual void Reset();
    virtual bool FactoryRestore();
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
    virtual bool SendData(const char* buf, const int size, int timeoutMs, int linkId = -1, bool waitAck=false);
    virtual bool ReceiveData(char* buf, int& size, int& linkId, int timeoutMs, bool waitForOK);
    //virtual bool WaitForClientConnection(int* linkId, int timeoutMs);
    virtual bool CloseConnection(int linkId);
#ifdef ESP8266_NEW
    virtual bool SetTCPReceiveMode(bool isActive);
    virtual bool ReceiveTCPData(char* buf, int bytesToRead, int* bytesRead, int linkId = -1);
#endif
    virtual bool IsConnectionClosed(int linkId);
    virtual void SetChunkSize(int chunkSize);

    // Doesnt work with old firmware, need to update first 
    // but AT+CIUPDATE doesn't work either
    bool SetupSNTP(const char* server, int timeZone);
    bool GetSNTPTime(char *buf, int timeoutMs);

    // Multiple connection mode
    virtual bool SetMux(bool isMux);
    virtual bool GetListOfAps(char* buf, int buflen);

    virtual ~Esp8266() {}
private:

    bool ParseIPD(int& linkId, int& bytesToRead);
    bool SendChunk(const char* buf, const int size, int timeoutMs, int linkId, bool waitAck);
    void Log(const char* format, ...);
    virtual void _Log(const char* message);

    Serial* _serial;
    char    _databuf[DATABUFSIZE];
    char    _cmdbuf[CMDBUFSIZE];
    int     _dataBytesRead; //total bytes in _databuf after Expect 
    int     _databufPos;  // current position after Expect/Read etc
    bool    _autoConnect;
    bool    _echoOff;
    int     _chunkSize;
    Timer* _timer;
};