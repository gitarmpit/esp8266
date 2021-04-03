#include "Esp8266.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include "memmem.h"

Esp8266::Esp8266(Serial* serial, Timer* timer, bool autoConnect, bool echoOff)
{ 
    _serial = serial; 
    _timer = timer;
    _dataBytesRead = 0;
    _databufPos = 0;
    _echoOff = echoOff;
    _autoConnect = autoConnect;
    _chunkSize = DEFAULT_CHUNK_SIZE;
    memset(_databuf, 0, sizeof _databuf);
    memset(_cmdbuf, 0, sizeof _cmdbuf);
}

bool Esp8266::SendCommand(const char* buf, int size)
{
    _dataBytesRead = 0;
    _databufPos = 0;
#ifdef DEBUG_MODE 
    Log("SendCommand: <%s>\n", buf);
#endif
    return _serial->Send(buf, size);
}

bool Esp8266::WaitBoot()
{
    if (!Expect("ready\r\n", 30000))
    {
        return false;
    }

    if (_echoOff && !EchoOff())
    {
        return false;
    }

    return SetAutoConnect (_autoConnect);
}

void Esp8266::Reset()
{
    SendCommand(AT_RST);
}

bool Esp8266::EchoOff()
{
    SendCommand(AT_ATE0);
    return Expect(AT_OK);
}

bool Esp8266::FactoryRestore()
{
    SendCommand(AT_RESTORE);
    if (!Expect(AT_OK, 10000))
    {
        return false;
    }
    return WaitBoot();
}

bool Esp8266::DisconnectFromAP()
{
    SendCommand(AT_CWQAP);
    return Expect(AT_OK);
}

bool Esp8266::ConnectToAP(const char* ssid, const char* password, bool saveToFlash, int timeoutMs)
{
    bool hasCreds = false;
    if (ssid != NULL && password != NULL)
    {
        hasCreds = true;
        if (saveToFlash)
        {
            sprintf(_cmdbuf, "%s=\"%s\",\"%s\"\r\n", AT_CWJAP_DEF, ssid, password);
        }
        else
        {
            sprintf(_cmdbuf, "%s=\"%s\",\"%s\"\r\n", AT_CWJAP_CUR, ssid, password);
        }
        SendCommand(_cmdbuf);
    }

    bool gotIP = false;
    if (hasCreds || _autoConnect)
    {
        if (!Expect(WIFI_CONNECTED, timeoutMs))
        {
            return false;
        }
        if (!Expect(WIFI_GOT_IP, timeoutMs))
        {
            return false;
        }
        gotIP = true;
    }

    return hasCreds ? Expect(AT_OK, timeoutMs) : gotIP;
}

bool Esp8266::SetMode(MODE mode)
{
    sprintf(_cmdbuf, AT_CWMODE_CUR"=%d\r\n", (int)mode);
    SendCommand(_cmdbuf);
    return Expect(AT_OK);
}

bool Esp8266::SetAutoConnect(bool on)
{
    sprintf(_cmdbuf, AT_CWAUTOCONN"=%d\r\n", on ? 1: 0);
    SendCommand(_cmdbuf);
    return Expect(AT_OK);
}

bool Esp8266::IsConnectedToAP()
{
    SendCommand(AT_CWJAP_CUR_GET);
    Expect("OK\r\n");
    const char search[] = CWJAP_CUR_PATTERN;
    return (MemMem(_databuf, search, _dataBytesRead, (int)strlen((const char*)search))) != -1;
}

bool Esp8266::SetMux(bool isMux)
{
    sprintf(_cmdbuf, AT_CIPMUX"=%d\r\n", isMux ? 1 : 0);
    SendCommand(_cmdbuf);
    return Expect(AT_OK);
}

bool Esp8266::StopServer()
{
    SendCommand(AT_CIPSERVER"=0\r\n");
    return Expect(AT_OK);
}

bool Esp8266::StartServer(int port, int maxConnections, int timeoutSec)
{
    UNUSED(maxConnections);
    UNUSED(timeoutSec);

    /*
    // not supported in the old chip
    sprintf(_cmdbuf, "AT+CIPSERVERMAXCONN=%d\r\n", maxConnections);
    SendCommand(_cmdbuf);
    if (!Expect("OK\r\n", _databuf))
    {
        return false;
    }
    */

    sprintf(_cmdbuf, AT_CIPSERVER"=1,%d\r\n", port);
    SendCommand(_cmdbuf);
    
    //if (!Expect(AT_OK), 10000)
    //{
    //    return false;
    //}

    // Not supported in the old chip
    //sprintf(_cmdbuf, "AT+CIPSTO=%d\r\n", timeoutSec);
    //SendCommand(_cmdbuf);
    return Expect(AT_OK);
}

bool Esp8266::ConnectTCP(const char* hostname, int port, int timeoutMs, int linkId)
{
    if (linkId == -1)
    {
        sprintf(_cmdbuf, "%s,\"%s\",%d\r\n", AT_CIPSTART_TCP, hostname, port);
    }
    else
    {
        sprintf(_cmdbuf, "%s=%d,\"TCP\",\"%s\",%d\r\n", AT_CIPSTART, linkId, hostname, port);
    }
    SendCommand(_cmdbuf);
    return Expect(AT_OK, timeoutMs);
}

bool Esp8266::ConnectUDP(const char* hostname, int port, int timeoutMs, int linkId)
{
    if (linkId == -1)
    {
        sprintf(_cmdbuf, "%s,\"%s\",%d\r\n", AT_CIPSTART_UDP, hostname, port);
    }
    else
    {
        sprintf(_cmdbuf, "%s=%d,\"UDP\",\"%s\",%d\r\n", AT_CIPSTART, linkId, hostname, port);
    }
    SendCommand(_cmdbuf);
    return Expect(AT_OK, timeoutMs);
}

#if 0
bool Esp8266::WaitForClientConnection(int* linkId, int timeoutMs)
{
    if (*linkId != -1)
    {
        return true;
    }

    strcpy(_cmdbuf, ",CONNECT\r\n");
    if (Expect(_cmdbuf, timeoutMs))
    {
#ifdef DEBUG_MODE 
        Log("CONNECT received\n");
#endif 
        int linkIdPos = _databufPos - (int)strlen(_cmdbuf) - 1;
        if (linkIdPos >= 0)
        {
            char c = _databuf[linkIdPos];
            if (isdigit(c))
            {
                *linkId = c - '0';
            }
        }
        return true;
    }
    return false;
}
#endif

bool Esp8266::CloseConnection(int linkId)
{
    if (linkId == -1)
    {
        SendCommand(AT_CIPCLOSE"\r\n");
    }
    else
    {
        sprintf(_cmdbuf, "%s=%d\r\n", AT_CIPCLOSE, linkId);
        SendCommand(_cmdbuf);
    }
    return Expect(AT_OK);
}

bool Esp8266::PassthroughMode(bool on)
{
    if (on)
    {
        SendCommand("AT+CIPMODE=1\r\n");
        if (!Expect(AT_OK))
        {
            return false;
        }

        SendCommand("AT+CIPSEND\r\n");
        return Expect(AT_OK"> ");
    }
    else
    {
        SendCommand("+++");
        Sleep(2000);
        SendCommand("AT+CIPMODE=0\r\n");
        return Expect(AT_OK);
    }
}

bool Esp8266::SendData(const char* buf, const int size, int timeoutMs, int linkId, bool waitAck)
{
    if (size == 0)
    {
        return true;
    }

    int sent = 0;
    int chunkSize = size;

    while (true)
    {
        if (chunkSize > _chunkSize)
        {
            chunkSize = _chunkSize;
        }

        if (sent + chunkSize > size)
        {
            chunkSize = size - sent;
        }

        if (!SendChunk(&buf[sent], chunkSize, timeoutMs, linkId, waitAck))
        {
            return false;
        }
        sent += chunkSize; 
        if (sent == size)
        {
            break;
        }
    }

    return true;
}

bool Esp8266::SendChunk(const char* buf, const int size, int timeoutMs, int linkId, bool waitAck)
{
    if (linkId == -1)
    {
        sprintf(_cmdbuf, "%s=%d\r\n", AT_CIPSEND, size);
    }
    else
    {
        sprintf(_cmdbuf, "%s=%d,%d\r\n", AT_CIPSEND, linkId, size);
    }
    SendCommand(_cmdbuf);
    if (!Expect(AT_OK"> ", timeoutMs))
    {
        return false;
    }

    if (!_serial->Send(buf, size))
    {
        return false;
    }

    return waitAck ? Expect(SEND_OK, timeoutMs) : true;
}

bool Esp8266::ParseIPD(int& linkId, int& bytesToRead)
{
    char tmp1[32];
    char tmp2[32];
    int nfields = 0;
    int pos1 = 0;
    int pos2 = 0;
    bool rc = false;
    memset(tmp1, 0, sizeof tmp1);
    memset(tmp2, 0, sizeof tmp2);
    int len = 20; 
    if (len > (_dataBytesRead - _databufPos))
    {
        len = _dataBytesRead - _databufPos;
    }
    
    for (int i = 0; i < len; ++i)
    {
        char c = (char)_databuf[_databufPos++];
        if (c == ':')
        {
            if (strlen(tmp1) == 0)
            {
                rc = false;
            }
            else if (nfields == 1 && strlen(tmp2) == 0)
            {
                rc = false;
            }
            else
            {
                rc = true;
            }
            break;
        }
        else if (c == ',')
        {
            ++nfields;
            if (nfields > 1)
            {
                break;
            }
        }
        else if (!::isdigit(c))
        {
            rc = false;
            break;
        }
        else if (nfields == 0)
        {
            tmp1[pos1++] = c;
        }
        else
        {
            tmp2[pos2++] = c;
        }
    }

    if (rc)
    {
        if (nfields == 0)
        {
            linkId = -1;
            bytesToRead = atoi(tmp1);
        }
        else
        {
            linkId = atoi(tmp1);
            bytesToRead = atoi(tmp2);
        }
    }

    return rc;
}

bool Esp8266::IsConnectionClosed(int linkId)
{
    bool rc = false;
    if (linkId >= 0)
    {
        sprintf(_cmdbuf, "%d,CLOSED\r\n", linkId);
        int pos = MemMem(_databuf, _cmdbuf, _dataBytesRead, (int)strlen(_cmdbuf));
        if (pos != -1)
        {
            _databufPos = pos + (int)strlen(_cmdbuf);
            rc = true;
        }
    }

    return rc;
}

void Esp8266::SetChunkSize(int chunkSize)
{
    if (chunkSize > 1 && chunkSize <= 1024)
    {
        _chunkSize = chunkSize;
    }
}

bool Esp8266::Expect(const char* expect, int timeoutMs, bool resetPointer)
{
    if (timeoutMs < 0)
    {
        timeoutMs = 0;
    }

#ifdef DEBUG_MODE
    Log("expecting: %s\n", expect);
#endif
    int expectedLength = (int)strlen(expect);
    int remainder = 0;

    remainder = _dataBytesRead - _databufPos;

    if (remainder > 0)
    {
        int idx = MemMem(&_databuf[_databufPos], expect, remainder, expectedLength);

        if (idx >= 0)
        {
            _databufPos += (idx + expectedLength);
#ifdef DEBUG_MODE
            Log("found <%s> in remainder, new pos: %d\n", expect, _databufPos);
#endif
            return true;
        }
    }
    else if (resetPointer)
    {
        _databufPos = 0;
        _dataBytesRead = 0;
#ifdef DEBUG_MODE
        memset(_databuf, 0, sizeof _databuf);
#endif
    }

    if (timeoutMs == 0)
    {
        return false;
    }   

    bool rc = false;
    int bytesReceived = 0;

    _timer->Reset();

    while (true)
    {
        if (!_serial->Read(&_databuf[_dataBytesRead], bytesReceived, timeoutMs))
        {
            return false;
        }

#ifdef DEBUG_MODE
        Log("bytes received: %d, pos: %d\n", bytesReceived, _dataBytesRead);
        Log("{");
        for (int i = 0; i < (int)bytesReceived; ++i)
        {
            char c = (char)_databuf[_dataBytesRead + i];
            Log("%c", (c > 9 && c != 13) ? c : '.');
        }
        Log("}\n");
#endif        
        _dataBytesRead += bytesReceived;

        int idx = MemMem(&_databuf[_databufPos], expect, _dataBytesRead, expectedLength);

        if (idx >= 0)
        {
            _databufPos += (idx + expectedLength);
            rc = true;
            break;
        }

        auto elapsedMs = _timer->ElapsedMs();
        if ( elapsedMs > timeoutMs)
        {
#ifdef DEBUG_MODE 
            Log("expect: timeout\n");
#endif
            break;
        }
        timeoutMs -= elapsedMs;
    }

    return rc;
}

bool Esp8266::ReceiveData(char* buf, int& bytesRead, int& linkId, int timeoutMs, bool waitForOK)
{
    bytesRead = 0;
    
#ifdef DEBUG_MODE
    Log("ReceiveFromClient: expecting +IPD,\n");
#endif
    if (!Expect(IPD, timeoutMs))
    {
#ifdef DEBUG_MODE
        Log("Failed to receive IPD\n");
#endif
        return false;
    }

    static char ipd[32];

    int ipdMatchPos = _databufPos;
    if (!Expect(":", timeoutMs, false))
    {
#ifdef DEBUG_MODE
        Log("Failed to receive \":\"\n");
#endif
        return false;
    }

    _databufPos = ipdMatchPos;

    if (_databufPos == -1 || !ParseIPD(linkId, bytesRead))
    {
#ifdef DEBUG_MODE
        Log("Failed to parse IPD\n");
#endif
        return false;
    }

    int dataOffset = _databufPos; //point where data starts

#ifdef DEBUG_MODE
    Log("linkId: %d, bytesToRead: %d, bufsize: %d, data offset: %d\n",
            linkId, bytesRead, _dataBytesRead, dataOffset);
#endif

    if (bytesRead == 0 || bytesRead > DATABUFSIZE)
    {
        return false;
    }

    while (true)
    {
        if (_dataBytesRead - dataOffset >= bytesRead)
        {
            break;
        }
        int bytesReceived;
#ifdef DEBUG_MODE
        Log("Received data: attempt to read additional data\n");
#endif
        if (!_serial->Read(&_databuf[_dataBytesRead], bytesReceived, timeoutMs))
        {
            return false;
        }

#ifdef DEBUG_MODE
        Log("{");
        for (int i = 0; i < (int)bytesReceived; ++i)
        {
            char c = (char)_databuf[_dataBytesRead + i];
            Log("%c", (c > 9 && c != 13) ? c : '.');
        }
        Log("}\n");
#endif
        
        _dataBytesRead += bytesReceived;
        
#ifdef DEBUG_MODE
        Log("read another block, total bytes: %d\n", _dataBytesRead);
#endif
    }

    memcpy(buf, &_databuf[dataOffset], (size_t)(bytesRead));
    _databufPos += bytesRead;
#ifdef DEBUG_MODE
    Log("remainder after receivedata: <%s>\n", &_databuf[_databufPos]);
#endif
    return waitForOK ? Expect("OK\r\n") : true;
}

#ifdef ESP8266_NEW
bool Esp8266::SetTCPReceiveMode(bool isActive)
{
    sprintf(_cmdbuf, "AT+CIPRECVMODE=%d\r\n", isActive ? 0 : 1);
    SendCommand(_cmdbuf);
    return Expect(AT_OK);
}

bool Esp8266::ReceiveTCPData(char* buf, int bytesToRead, int* bytesRead, int linkId)
{
    if (linkId == -1)
    {
        sprintf(_cmdbuf, "AT+CIPRECVDATA=%d\r\n", bytesToRead);
    }
    else
    {
        sprintf(_cmdbuf, "AT+CIPRECVDATA=%d,%d\r\n", linkId, bytesToRead);
    }

    SendCommand(_cmdbuf);

    *bytesRead = 0;
    //TODO  +CIPRECVDATA:<actual_len>,<data>
    return Expect(AT_OK);
}

bool Esp8266::SetupSNTP(const char* server, int timeZone)
{
    sprintf(_cmdbuf, "AT+CIPSNTPCFG=1,%d,\"%s\"\r\n", timeZone, server);
    SendCommand(_cmdbuf);
    return Expect(AT_OK);
}

bool Esp8266::GetSNTPTime(char* buf, int timeoutMs)
{
    SendCommand("AT+CIPSNTPTIME?\r\n");
    bool rc = Expect(AT_OK, timeoutMs);
    if (rc)
    {
        strncpy(buf, (char*)&_databuf[strlen("+CIPSNTPTIME:")], 24);
    }
    return rc;
}
#endif


bool Esp8266::SetStationIpAddress(const char* ipAddress)
{
    sprintf(_cmdbuf, AT_CIPSTA_CUR"=\"%s\"\r\n", ipAddress);
    if (SendCommand((const char*)_cmdbuf))
    {
        return Expect(AT_OK);
    }
    else
    {
        return false;
    }
}

void Esp8266::Log(const char* format, ...)
{
    static char buffer[1024];
    if (format != NULL)
    {
        va_list args;
        va_start(args, format);

        int size = vsnprintf(NULL, 0, format, args);
        if (size > 1022)
        {
            size = 1022;
        }

        vsnprintf(buffer, size + 1, format, args);

        va_end(args);
        _Log(buffer);
    }
}

void Esp8266::_Log(const char* message)
{
    fprintf(stderr, message);
}

bool Esp8266::GetListOfAps(char* buf, int buflen)
{
    SendCommand("AT+CWLAP\r\n");
    bool rc = false;
    if (Expect(AT_OK))
    {
        int size = _databufPos - 6;
        if (size > 0 && size < buflen)
        {
            strncpy(buf, (char*)_databuf, size);
            buf[size] = '\0';
            rc = true;
        }
    }
    return rc;
}