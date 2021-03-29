#pragma once 
#include "../common/Serial.h"
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include <errno.h>

#define _close_socket closesocket
#define poll WSAPoll

#pragma comment(lib, "Ws2_32.lib")

#define MAX_CONN 4

struct buf_t
{
    buf_t()
    {
        data = 0;
        length = 0;
    }
    char* data;
    int length;
};

struct connection_entry
{
    connection_entry()
    {
        memset(&buf, 0, sizeof buf);
        connection_id = -1;
        socket = INVALID_SOCKET;
    }
    buf_t buf;
    int connection_id;
    SOCKET socket;
};

// Raw winsock
// For Direct Http server, no Esp8266 layer
class WinSockIo : public Serial
{
public:
    WinSockIo(int port, int maxConn);
    virtual bool      Initialize(int baudRate);
    // Returns the number of bytes in the queue
    virtual uint32_t  WaitRead(int timeoutMs);
    virtual bool      Read(char* buf, int& bytesReceived, int timeoutMs);

    virtual bool      Send(const char* buf, int size = 0);
    virtual bool      SetTimeout(int timeoutMs);

private:
    bool _initialized;
    int _port;
    int _max_conn;
    SOCKET  _clientSocket;
    struct pollfd _fds[MAX_CONN + 1];
    int    _nfds;
    int _lastReadConn;

    connection_entry _connections[MAX_CONN];

    void Run();

    void removeClient(SOCKET client_s, int connectionId);

    void processAccept(SOCKET accept_s);
    void processClient(SOCKET client_s, int connectionId);

    bool readFromClient(SOCKET client_s, buf_t& msg);
    bool sendReply(SOCKET client_s, const buf_t& reply) const;
    static DWORD WINAPI main_thread_handler(void* arg);

};

//Wrapper around WinSockIo, implements Esp8266 commands
//Esp8266 calls this layer in Http Server running on PC without chip connected
class WinSock_Esp8266 : public WinSockIo
{
public:
    WinSock_Esp8266(int port, int maxConn) : WinSockIo(port, maxConn)
    {
        _isSending = false;

        // Simulates booting message
        char* ready = "ready\r\n";
        strcpy(_response, ready);
        _responseSize = strlen(ready);
    }
    virtual bool Read(char* buf, int& bytesReceived, int timeoutMs);
    virtual bool Send(const char* buf, int size = 0);
private:
    bool _isSending;
    char _response[256];
    int  _responseSize;
};