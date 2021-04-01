#include "WinSockIo.h"
#include "../common/Timer.h"
#include "../common/memmem.h"
#include "../common/Esp8266.h"

static bool stopping = false;
BOOL WINAPI int_handler(DWORD signal)
{

    if (signal == CTRL_C_EVENT)
    {
        stopping = true;
        return TRUE;
    }
    else
        return FALSE;
}

static void error(const char* what, bool fatal)
{
    printf("%s error: %d\n", what, WSAGetLastError());
    if (fatal)
        exit(1);
}

static void setup(int s)
{
    int on = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0)
    {
        error("setsockopt", true);
    }

    ULONG on2 = 1;
    if (ioctlsocket(s, FIONBIO, &on2) < 0)
    {
        error("ioctl", true);
    }
}

DWORD WINAPI WinSockIo::main_thread_handler(void* arg)
{
    WinSockIo* ws = (WinSockIo*)arg;
    ws->Run();
    return NULL;
}

WinSockIo::WinSockIo(int port, int maxConn)
{
    _initialized = false;
    _lastReadConn = 0;
    _clientSocket = INVALID_SOCKET;
    _max_conn = maxConn;
    if (_max_conn > MAX_CONN)
    {
        _max_conn = MAX_CONN;
    }
    _port = port;
    _nfds = 0;
    for (int i = 0; i < _max_conn; ++i)
    {
        _connections[i].connection_id = i;
    }
    //if (!SetConsoleCtrlHandler(int_handler, TRUE))
    //{
    //    printf("Could not set control handler\n");
    //}
}

bool      WinSockIo::Initialize(int baudRate)
{
    UNUSED(baudRate);

    if (_initialized)
    {
        return true;
    }

    WSADATA wsaData;
    bool rc = true;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", iResult);
        rc = false;
    }

    _thread = CreateThread(NULL, 0, WinSockIo::main_thread_handler, this, 0, NULL);

    return rc;
}

void WinSockIo::Stop()
{
    printf("Waiting for WinSockIo to stop...\n");
    stopping = true;
    WaitForSingleObject(_thread, INFINITE);
    CloseHandle(_thread);
    printf("WinSockIo stopped\n");
}

void WinSockIo::Run()
{
    SOCKET accept_s = socket(AF_INET, SOCK_STREAM, 0);

    printf("accept fd: %d\n", accept_s);
    setup(accept_s);

    struct sockaddr_in serv_addr;
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons((u_short)_port);

    if (bind(accept_s, (struct sockaddr*) & serv_addr, sizeof(serv_addr)))
    {
        error("bind", true);
    }

    if (listen(accept_s, 10))
    {
        error("listen", true);
    }

    printf("server started\n");

    memset(_fds, 0, sizeof(_fds));

    _fds[0].fd = accept_s;
    _fds[0].events = POLLIN;

    int timeout = 10; //ms

    while (!stopping)
    {
        int rc = poll(_fds, _max_conn + 1, timeout);
        if (rc <= 0)
        {
            if (rc == -1)
            {
                error("poll", true);
            }
            continue;
        }

        printf("poll returned: rc=%d\n", rc);
        for (int i = 0; i <= _max_conn; i++)
        {
            printf("%d: fd=%d revents=%d\n", i, _fds[i].fd, _fds[i].revents);
        }

        for (int i = 1; i <= _max_conn; i++)
        {
            if (_fds[i].fd != 0)
            {
                bool error = false;
                if (_fds[i].revents & POLLHUP)
                {
                    error = true;
                    printf("pollhup\n");
                }
                if (_fds[i].revents & POLLERR)
                {
                    error = true;
                    printf("pollherr\n");
                }
                //if (_fds[i].revents & POLLIN)
                //{
                //    printf("pollin: %d\n", i);
                //}
                if (error)
                {
                    removeClient(_fds[i].fd, i);
                }
                else if (_fds[i].revents & POLLIN)
                {
                    processClient(_fds[i].fd, i);
                }
            }
        }

        if ((_fds[0].revents & POLLIN) && _fds[0].fd == accept_s)
        {
            processAccept(accept_s);
        }

    }

    printf("WinSockIo Terminating...");
}

void WinSockIo::processAccept(SOCKET accept_s)
{
    printf("accept: total: %d\n", _nfds);
    SOCKET client_s = accept(accept_s, NULL, NULL);
    if (client_s < 0 && WSAGetLastError() != WSAEWOULDBLOCK)
    {
        error("accept", true);
    }

    if (_nfds == _max_conn)
    {
        printf("too many connections: %d, accept ignored\n", _nfds);
        _close_socket(client_s);
        return;
    }

    int id = -1;
    for (int i = 1; i <= _max_conn; ++i)
    {
        if (_fds[i].fd == 0)
        {
            id = i;
            break;
        }
    }

    if (id == -1)
    {
        printf("no  available connection slots\n");
        _close_socket(client_s);
        return;
    }

    ULONG on = 1;
    if (ioctlsocket(client_s, FIONBIO, &on) < 0)
    {
        error("ioctl", true);
    }

    printf("new incoming connection: %d, connection id %d:\n", client_s, id);
    _fds[id].fd = client_s;
    _fds[id].events = POLLIN;
    _nfds++;
}

void WinSockIo::processClient(SOCKET client_s, int connectionId)
{
    static int cnt;
    std::lock_guard<std::mutex> lock_guard(_mtx);

    printf("\n===================\n");
    printf("%d\n", ++cnt);

    printf("processClient: conn from: sock: %d, conn id: %d\n", client_s, connectionId);
    connection_entry& conn = _connections[connectionId - 1];
    conn.socket = client_s;
    buf_t& raw_in = conn.buf; 
    printf("before read, buf len: %d\n", raw_in.length);

    bool rc = readFromClient(client_s, raw_in);
    if (rc)
    {
        printf("Raw request len: %d bytes, conn id: %d\n", raw_in.length, connectionId - 1);
    }
    else
    {
        printf("processClient: readFromClient returned false, remove client: %d\n", client_s);
        removeClient(client_s, connectionId);
    }


    printf("\n===================\n");

}

// Return false on client close
bool WinSockIo::readFromClient(SOCKET s, buf_t& msg)
{
    static char buf[256];
    ULONG bytes_available = 0;
    if (::ioctlsocket(s, FIONREAD, &bytes_available) == 0)
    {
        printf("readFromClient: available:  %d\n", bytes_available);
    }
    else
    {
        error("readFromClient: ioctl", false);
    }
    bool rc;
    while (true)
    {
        int bytesRead = ::recv(s, buf, sizeof(buf), 0);
        if (bytesRead < 0)
        {
            if (WSAGetLastError() == WSAEWOULDBLOCK)
            {
                printf("readFromClient: would block\n");
                rc = true;
            }
            else
            {
                printf("readFromClient: cli closed\n");
                rc = true;
            }
            break;
        }
        else if (bytesRead == 0)
        {
            printf("readFromClient: 0, cli closed\n");
            rc = true;
            break;
        }

        int len = bytesRead;
        printf("readFromClient: %d bytes read\n", len);
        msg.data = (char*)realloc(msg.data, msg.length + len);
        memcpy(&msg.data[msg.length], buf, len);
        msg.length += len;
    }

    return rc;
}

bool WinSockIo::sendReply(SOCKET s, const buf_t& reply) const
{
    printf("send reply: sock: %d, msg: <%s>\n", s, reply.data);
    if (s == INVALID_SOCKET)
    {
        printf("sendReply: invalid socket\n");
    }
    bool rc = true;

    int total_written = 0;
    while (total_written < reply.length)
    {
        int written = ::send(s, (char*)&reply.data[total_written], reply.length - total_written, 0);
        if (errno == EWOULDBLOCK)
        {
            printf("sendReply: wouldblock\n");
        }
        if (written == -1)
        {
            printf("sendReply: socket write error\n");
            rc = false;
            break;
        }
        else if (written == 0)
        {
            printf("sendReply: written 0 bytes\n");
            break;
        }
        printf("sendReply: written %d bytes\n", written);
        total_written += written;
    }

    return rc;
}


void WinSockIo::removeClient(SOCKET client_s, int connectionId)
{
    printf("remove client socket %d from connection id: %d\n", client_s, connectionId);
    
    connection_entry& conn = _connections[connectionId - 1];
    free(conn.buf.data);
    conn.buf.data = 0;
    conn.buf.length = 0;
    conn.socket = INVALID_SOCKET;

    _fds[connectionId].fd = 0;
    _fds[connectionId].events = 0;
    _close_socket(client_s);
    _nfds--;
}


///////////////////////////////

uint32_t  WinSockIo::WaitRead(int timeoutMs)
{
    UNUSED(timeoutMs);
    return 0;
}


bool  WinSockIo::Read(char* buf, int& bytesReceived, int timeoutMs)
{
    bool rc = false;
    Timer timer;
    bytesReceived = 0;
    while (true)
    {
        _mtx.lock();
        for (int i = 0; i < _max_conn; ++i)
        {
            if (_connections[i].buf.length > 0)
            {
                memcpy(buf, _connections[i].buf.data, _connections[i].buf.length);
                bytesReceived = _connections[i].buf.length;
                fprintf(stderr, "WinSockIo:Read: got %d bytes from conn %d\n", bytesReceived, i);
                free(_connections[i].buf.data);
                _connections[i].buf.length = 0;
                _connections[i].buf.data = NULL;
                _lastReadConn = i;
                rc = true;
                break;
            }
        }
        _mtx.unlock();
        if (rc)
        {
            break;
        }
        Sleep(100);
        if (timer.ElapsedMs() > timeoutMs)
        {
            break;
        }
    }
    return rc;
}

bool WinSockIo::Send(const char* buf, int size)
{
    SOCKET cli_s = _connections[_lastReadConn].socket;
    buf_t msg;
    msg.data = (char*)buf;
    msg.length = size;
    fprintf(stderr, "sending reply on conn: %d\n", _lastReadConn);
    return sendReply(cli_s, msg);
}
bool      WinSockIo::SetTimeout(int timeoutMs)
{
    UNUSED(timeoutMs);
    return true;
}

////////////////////////////
bool  WinSock_Esp8266::Read(char* buf, int& bytesReceived, int timeoutMs)
{
    if (_responseSize > 0)
    {
        memcpy(buf, _response, _responseSize);
        bytesReceived = _responseSize;
        _responseSize = 0;
        return true;
    }
    else
    {
        int dataBytesReceived;
        static char databuf[DATABUFSIZE];
        bool rc = WinSockIo::Read(databuf, dataBytesReceived, timeoutMs);
        if (rc)
        {
            int cnt = sprintf(buf, "+IPD,0,%d:", dataBytesReceived);
            memcpy(&buf[cnt], databuf, dataBytesReceived);
            bytesReceived = dataBytesReceived + cnt;
        }
        else
        {
            bytesReceived = 0;
        }
        return rc;
    }
}

bool WinSock_Esp8266::Send(const char* buf, int size)
{
    if (size == 0)
    {
        size = strlen(buf);
    }

    if (_isSending)
    {
        _isSending = false;
        strcpy(_response, SEND_OK);
        _responseSize = strlen(SEND_OK);
        return WinSockIo::Send(buf, size);
    }
    else if (MemMem(buf, AT_CIPSEND, size, strlen(AT_CIPSEND)) != -1)
    {
        _isSending = true;
        const char* response = AT_OK"> ";
        strcpy(_response, response);
        _responseSize = strlen(response);
    }
    else if (
            MemMem(buf, AT_ATE0, size, strlen(AT_ATE0)) != -1 ||
            MemMem(buf, AT_CWQAP, size, strlen(AT_CWQAP)) != -1 ||
            MemMem(buf, AT_CWMODE_CUR, size, strlen(AT_CWMODE_CUR)) != -1 ||
            MemMem(buf, AT_CWAUTOCONN, size, strlen(AT_CWAUTOCONN)) != -1 ||
            MemMem(buf, AT_CIPMUX, size, strlen(AT_CIPMUX)) != -1 ||
            MemMem(buf, AT_CIPSERVER, size, strlen(AT_CIPSERVER)) != -1 ||
            MemMem(buf, AT_CIPCLOSE, size, strlen(AT_CIPCLOSE)) != -1 ||
            MemMem(buf, AT_CIPSTA_CUR, size, strlen(AT_CIPSTA_CUR)) != -1 ||
            MemMem(buf, AT_CWQAP, size, strlen(AT_CWQAP)) != -1
        )
    {
        strcpy(_response, AT_OK);
        _responseSize = strlen(AT_OK);
    }
    else if (MemMem(buf, AT_CWJAP_CUR_GET, size, strlen(AT_CWJAP_CUR_GET)) != -1)
    {
        strcpy(_response, CWJAP_CUR_PATTERN);
        strcat(_response, AT_OK);
        _responseSize = strlen(CWJAP_CUR_PATTERN) + strlen(AT_OK);
    }
    else if (
        MemMem(buf, AT_CWJAP_DEF, size, strlen(AT_CWJAP_DEF)) != -1||
        MemMem(buf, AT_CWJAP_CUR, size, strlen(AT_CWJAP_CUR)) != -1
        )
    {
        strcpy(_response, WIFI_CONNECTED);
        strcat(_response, WIFI_GOT_IP);
        strcat(_response, AT_OK);
        _responseSize = strlen(WIFI_GOT_IP) + strlen(WIFI_CONNECTED) + strlen(AT_OK);
    }
    return true;
}
