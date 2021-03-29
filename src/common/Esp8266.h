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

class Esp8266
{
public:
#ifdef UNIT_TESTS
    friend class MockEsp8266;
#endif
    enum class MODE
    {
        MODE_STATION = 1,
        MODE_AP = 2,
        MODE_STATION_AP = 3
    };

    Esp8266(Serial* serial, bool autoConnect = false, bool echoOff = false);
    bool SendCommand(const char* buf, int len = 0);
    bool Expect(const char* expect, int timeoutMs = 1000, bool resetPointer = true);
    bool WaitBoot();
    bool EchoOff();
    void Reset();
    bool FactoryRestore();
    bool DisconnectFromAP();
    bool ConnectToAP(const char* ssid, const char* password, bool saveToFlash, int timeoutMs = 10000);
    bool SetMode(MODE mode);
    bool SetAutoConnect(bool on);
    bool IsConnectedToAP();
    bool SetStationIpAddress(const char* ipAddress);
    bool StartServer(int port, int maxConnections, int timeoutSec);
    bool StopServer();
    bool PassthroughMode(bool on);
    bool ConnectTCP(const char* hostname, int port, int timeoutMs, int linkId = -1);
    bool ConnectUDP(const char* hostname, int port, int timeoutMs, int linkId = -1);
    bool SendData(const char* buf, const int size, int timeoutMs, int linkId = -1, bool waitAck=false);
    bool ReceiveData(char* buf, int& size, int& linkId, int timeoutMs, bool waitForOK);
    bool WaitForClientConnection(int* linkId, int timeoutMs);
    bool CloseConnection(int linkId);
    bool SetTCPReceiveMode(bool isActive);
    bool ReceiveTCPData(char* buf, int bytesToRead, int* bytesRead, int linkId = -1);
    bool IsConnectionClosed(int linkId);
    void SetChunkSize(int chunkSize);

    // Doesnt work with old firmware, need to update first 
    // but AT+CIUPDATE doesn't work either
    bool SetupSNTP(const char* server, int timeZone);
    bool GetSNTPTime(char *buf, int timeoutMs);

    // Multiple connection mode
    bool SetMux(bool isMux);
    bool GetListOfAps(char* buf, int buflen);

    virtual Timer& CreateTimer();

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
};