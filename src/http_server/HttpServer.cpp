#include "HttpServer.h"
#include "HttpRequest.h"
#include "../common/memmem.h"
#include "html.cpp"
#include <string.h>
#include "ApSetup.h"

#define MAIN_PAGE main_html

#define HTTP_DEBUG_MODE

HttpServer::HttpServer(Esp8266_Base* esp, PersistedSettings* settings, int maxConnections)
{
   _esp = esp;
   _flashSettings = settings;
   _running = false;
   _initialized = false;
   _maxConnections = maxConnections;
   _currentBufPos = 0;
   _eofHeaderPos = 0;
   _linkId = -1;
}

void HttpServer::Stop()
{
    _running = false;
}

bool HttpServer::Init()
{
    _esp->Reset();
    if (!_esp->WaitBoot())
    {
        printf("WaitBoot error\n");
        return false;
    }

    Esp8266::MODE mode = Esp8266::MODE::MODE_AP;
    char* ssid = _flashSettings->GetSsid();
    char* pass = _flashSettings->GetPassword();
    if (ssid != NULL)
    {
        mode = Esp8266::MODE::MODE_STATION_AP;
    }

    if (!_esp->SetMode(mode))
    {
        printf("SetMode error\n");
        return false;
    }

    if (mode == Esp8266::MODE::MODE_STATION || mode == Esp8266::MODE::MODE_STATION_AP)
    {
        if (!_esp->ConnectToAP(ssid, pass, false, 5000) || !_esp->IsConnectedToAP())
        {
            printf("ConnectToAP error\n");
            return false;
        }

        printf("Connected to AP\n");
        if (!_esp->SetStationIpAddress("192.168.0.254"))
        {
            printf("SetStationIP error\n");
            return false;
        }
    }
    
    if (!_esp->SetMux(true))
    {
        printf("SetMux error\n");
        return false;
    }

    if (!_esp->StartServer(HTTP_PORT, _maxConnections, SOCKET_TIMEOUT_SEC))
    {
        printf("StartServer error\n");
        return false;
    }

    printf("Server ready\n");
    

    _initialized = true;
    return true;
}

bool HttpServer::ReadData(int timeoutSec)
{
    int size;
#ifdef HTTP_DEBUG_MODE
    if (_esp->IsConnectionClosed(_linkId))
    {
        fprintf(stderr, "ReadData: Connection closed\n");
    }
#endif

    if (!_esp->ReceiveData(&_buf[_currentBufPos], size, _linkId, timeoutSec, false))
    {
        return false;
    }
#ifdef HTTP_DEBUG_MODE
    fprintf(stderr, "ReadersHeaders: received %d bytes, id: %d, pos: %d: <%s>\n",
        size, _linkId, _currentBufPos, &_buf[_currentBufPos]);
#endif
    _currentBufPos += size;

    return true;
}

bool HttpServer::ReadHeaders()
{
    _currentBufPos = 0;
    _eofHeaderPos = 0;

    memset(_buf, 0, sizeof _buf);

    while (_running)
    {
        if (!ReadData(10000000))
        {
            continue;
        }
        char* eofHeaderPtr = strstr(_buf, "\r\n\r\n");
        if (eofHeaderPtr)
        {
#ifdef HTTP_DEBUG_MODE
            fprintf(stderr, "Got eof headers\n");
#endif
            _eofHeaderPos = (int)(eofHeaderPtr - _buf + 4);
            break;
        }
#ifdef HTTP_DEBUG_MODE
        fprintf(stderr, "No eof headers, keep reading\n");
#endif
    }

    return true;
}

bool HttpServer::ReadBody()
{
    int contentLength = _req.GetContentLength();
    if (contentLength == 0)
    {
        return true;
    }

    bool rc = true;
    while (_running)
    {
        int remainder = _currentBufPos - _eofHeaderPos;
#ifdef HTTP_DEBUG_MODE
        fprintf(stderr, "Read body: remainder: %d\n", remainder);
#endif
        if (remainder >= contentLength)
        {
            break;
        }

        if (!ReadData(1000))
        {
            rc = false;
            break;
        }
    }

#ifdef HTTP_DEBUG_MODE
    fprintf(stderr, "body: <%s>\n", &_buf[_eofHeaderPos]);
#endif
 
    return rc;
}

static int cnt;
void HttpServer::ProcessSetupAPConfing()
{
    Response200("text/html", 0);
    _esp->SendData(_buf, (int)strlen(_buf), 10000, _linkId, true);
    ApSetup apSetup (_esp, &_buf[_eofHeaderPos], &_req);
    apSetup.ParsePostRequest(_flashSettings);
}

void HttpServer::QueryAPConfig()
{
    char body[32];
    char* ssid = _flashSettings->GetSsid();
    if (ssid != NULL)
    {
        strcpy (body, ssid);
    }
    else
    {
        strcpy(body, "None");
    }

    int len = (int)strlen((char*)body);
    Response200("text/html", len);
    _esp->SendData(_buf, (int)strlen(_buf), 10000, _linkId, true);
    _esp->SendData(body, len, 10000, _linkId, true);
}


void HttpServer::Serve(const char* body, const char* contentType)
{
    int len = (int)strlen(body);
    Response200(contentType, len);
    _esp->SendData(_buf, (int)strlen(_buf), 10000, _linkId, true);
    _esp->SendData(body, len, 10000, _linkId, true);
}

void HttpServer::Response200(const char* contentType, int len)
{
    const char* fmt = "HTTP/1.1 200 OK\r\nContent-type: %s\r\nContent-Length: %d\r\nConnection: close\r\n\r\n";
    sprintf(_buf, fmt, contentType, len);
}

void HttpServer::Response400(const char* text)
{
    const char* fmt = "HTTP/1.1 400 Bad Request\r\nContent-type: text/html\r\nContent-Length: %d\r\nConnection: close\r\n\r\n";
    sprintf(_buf, fmt, text ? strlen(text) : 0);
    if (text)
    {
        strcat(_buf, text);
    }
}

void HttpServer::Response404(const char* text)
{
    const char* fmt = "HTTP/1.1 404 Not Found\r\nContent-type: text/html\r\nContent-Length: %d\r\nConnection: close\r\n\r\n";
    sprintf(_buf, fmt, text ? strlen(text) : 0);
    if (text)
    {
        strcat(_buf, text);
    }
}

void HttpServer::ProcessRequest()
{
    ReadHeaders();
    if (_req.Parse(_buf, _currentBufPos))
    {
        fprintf(stderr, "headers read, content-length: %d\n", _req.GetContentLength());
        fprintf(stderr, "method: %d, uri: %s\n", _req.GetMethod(), _req.GetURI());
        if (!ReadBody())
        {
            Response400();
            _esp->SendData(_buf, (int)strlen(_buf), 10000, _linkId, true);
        }
        else  if (!strcmp(_req.GetURI(), "/"))
        {
            printf("serving main page\n");
            Serve(MAIN_PAGE, "text/html; charset=utf-8");
        }
        else  if (!strcmp(_req.GetURI(), "/set_ap_config"))
        {
            //Ajax request to configure SSID;password for router
            fprintf(stderr, "process set_ap_config: %d\n", cnt);
            ProcessSetupAPConfing();
        }
        else  if (strstr(_req.GetURI(), "/query_ap_config"))
        {
            //Ajax request to check if router has been configured 
            //check saved settings in FLASH, if ssid is set, return it 
            fprintf(stderr, "process query_ap_config: %d\n", cnt);
            QueryAPConfig();
        }
        else
        {
            fprintf(stderr, "sending 404 for: %s", _req.GetURI());
            Response404();
            _esp->SendData(_buf, (int)strlen(_buf), 10000, _linkId, true);
        }

#ifdef HTTP_DEBUG_MODE
        if (_esp->IsConnectionClosed(_linkId))
        {
            fprintf(stderr, "After sendResponse: Connection closed\n");
        }
#endif
    }
    else
    {
        fprintf(stderr, "Error parsing headers\n");
    }
}

void HttpServer::Run()
{
    _running = true;

    if (!_initialized && !Init())
    {
        _running = false;
        printf("HttpSever: initialize failed, exiting\n");
        return;
    }
    while (_running)
    {
        ProcessRequest();
    }
}
