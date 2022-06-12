#include "HttpServer.h"
#include "HttpRequest.h"
#include "../common/memmem.h"
#include <string.h>
#include "ApSetup.h"

#define HTTP_DEBUG_MODE

const char* HttpServer::_format200 = 
    "HTTP/1.1 200 OK\r\nContent-type: %s\r\nContent-Length: %d\r\nConnection: close\r\n\r\n";
const char* HttpServer::_format400 =
    "HTTP/1.1 400 Bad Request\r\nContent-type: text/html\r\nContent-Length: %d\r\nConnection: close\r\n\r\n";
const char* HttpServer::_format404 = 
    "HTTP/1.1 404 Not Found\r\nContent-type: text/html\r\nContent-Length: %d\r\nConnection: close\r\n\r\n";
const char* HttpServer::_format500 = 
    "HTTP/1.1 500 Internal Server Error\r\nContent-type: text/html\r\nContent-Length: %d\r\nConnection: close\r\n\r\n";

HttpServer::HttpServer(Esp8266_Base* esp, const char* mainPage, PersistedSettings* settings, Timer* timer, int maxConnections)
{
   _esp = esp;
   _flashSettings = settings;
   _mainPage = mainPage;
   _running = false;
   _initialized = false;
   _maxConnections = maxConnections;
   _currentBufPos = 0;
   _eofHeaderPos = 0;
   _linkId = -1;
   _timer = timer;
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

    Esp8266::MODE mode = Esp8266::MODE::MODE_STATION_AP;
    if (!_esp->SetMode(mode))
    {
        printf("SetMode error\n");
        return false;
    }

    /*
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

    if (mode == Esp8266::MODE::MODE_STATION_AP)
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
    */
    
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

bool HttpServer::ReadData(int timeoutMs)
{
    int size;
#ifdef HTTP_DEBUG_MODE
    if (_esp->IsConnectionClosed(_linkId))
    {
        printf("ReadData: Connection closed\n");
    }
#endif

    if (!_esp->ReceiveData(&_buf[_currentBufPos], size, _linkId, timeoutMs, false))
    {
        return false;
    }
#ifdef HTTP_DEBUG_MODE
    printf("ReadersHeaders: received %d bytes, id: %d, pos: %d: <%s>\n",
        size, _linkId, _currentBufPos, &_buf[_currentBufPos]);
#endif
    _currentBufPos += size;

    return true;
}

bool HttpServer::ReadHeaders(int timeoutMs)
{
    _currentBufPos = 0;
    _eofHeaderPos = 0;

    memset(_buf, 0, sizeof _buf);

    _timer->Reset();
    bool rc = true;
    while (_running)
    {
        if (ReadData(timeoutMs))
        {
            char* eofHeaderPtr = strstr(_buf, "\r\n\r\n");
            if (eofHeaderPtr)
            {
#ifdef HTTP_DEBUG_MODE
                printf("Got eof headers\n");
#endif
                _eofHeaderPos = (int)(eofHeaderPtr - _buf + 4);
                break;
            }
#ifdef HTTP_DEBUG_MODE
            printf("No eof headers, keep reading\n");
#endif
        }
        else
        {
            rc = false;
            break;
        }

        auto elapsedMs = _timer->ElapsedMs();
        if (elapsedMs > timeoutMs)
        {
            printf("Timeout reading headers\n");
            rc = false;
            break;
        }

        timeoutMs -= elapsedMs;
    }

    return rc;
}

bool HttpServer::ReadBody()
{
    int contentLength = _req.GetContentLength();
    if (contentLength == 0)
    {
        return true;
    }

    if (contentLength >= (BUFFERSIZE - _currentBufPos))
    {
        printf("buffer overflow\n");
        return false;
    }

    bool rc = true;
    while (_running)
    {
        int bodyBytesRead = _currentBufPos - _eofHeaderPos;
#ifdef HTTP_DEBUG_MODE
        printf("Read body: remainder: %d\n", bodyBytesRead);
#endif
        if (bodyBytesRead >= contentLength)
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
    printf("body: <%s>\n", &_buf[_eofHeaderPos]);
#endif
 
    return rc;
}

void HttpServer::ProcessSetupAPConfing()
{
    Response200(CONTENT_TYPE_TEXT_HTML, 0);
    ApSetup apSetup (_esp, &_buf[_eofHeaderPos], &_req);
    apSetup.ParsePostRequest(_flashSettings);
}

void HttpServer::ProcessSetBrightness()
{
    if (_req.GetMethod() != HTTP_METHOD::POST || _req.GetContentLength() == 0)
    {
        Response400();
    }

    const char* body = &_buf[_eofHeaderPos];
    fprintf(stderr, "Set brightness: %s\n", body);
}


void HttpServer::ProcessGetApList()
{
    char buf[2048] = { 0 };
    if (!_esp->GetListOfAps(buf, sizeof buf - 1))
    {
        Response500();
    }
    else
    {
        int len = (int)strlen(buf);
        Response200(CONTENT_TYPE_TEXT_HTML, len);
        _esp->SendData(buf, len, 10000, _linkId, true);
    }
}

void HttpServer::GetSsid(char* buf)
{
    char* ssid = _flashSettings->GetSsid();
    if (ssid != NULL)
    {
        strcpy(buf, ssid);
    }
}

void HttpServer::QueryAPConfig()
{
    char body[32] = { 0 };
    GetSsid(body);

    int len = (int)strlen((char*)body);
    Response200(CONTENT_TYPE_TEXT_HTML, len);
    _esp->SendData(body, len, 10000, _linkId, true);
}

void HttpServer::ServeMainPage(const char* body, const char* contentType)
{
    int in_len = (int)strlen(body);

    char apList[2048] = { 0 };
    _esp->GetListOfAps(apList, sizeof apList - 1);

    char sid[32] = { 0 };
    GetSsid(sid);

    int malloc_len = in_len + (int)strlen(sid) + (int)strlen(apList) + 1;
    char* buf = (char*)(malloc(malloc_len));
    memset(buf, 0, malloc_len);
    int j = 0;
    for (int i = 0; i < in_len; )
    {
        bool tag_found = false;
        if (body[i] == '$')
        {
            const char* p = strchr(&body[i+1], '$');
            if (p)
            {
                int taglen = (int)(p - &body[i]);
                if (taglen > 1 && !strncmp(&body[i+1], AP_LIST_TAG, taglen-1))
                {
                    strcpy(&buf[j], apList);
                    j += (int)strlen(apList);
                    i += taglen+1;
                    tag_found = true;
                }
                else if (taglen > 1 && !strncmp(&body[i + 1], AP_STATUS_TAG, taglen - 1))
                {
                    strcpy(&buf[j], sid);
                    j += (int)strlen(sid);
                    i += taglen+1;
                    tag_found = true;
                }
            }
        }
        if (!tag_found)
        {
            buf[j++] = body[i];
            ++i;
        }
    }
    Response200(contentType, j);
    _esp->SendData(buf, j, 10000, _linkId, true);
    free(buf);
}

void HttpServer::Response200(const char* contentType, int len)
{
    char buf[256];
    sprintf(buf, _format200, contentType, len);
    _esp->SendData(buf, (int)strlen(buf), 10000, _linkId, true);
}

void HttpServer::Response400()
{
    char buf[256];
    sprintf(buf, _format400, 0);
    _esp->SendData(buf, (int)strlen(buf), 10000, _linkId, true);
}

void HttpServer::Response404()
{
    char buf[256];
    sprintf(buf, _format404, 0);
    _esp->SendData(buf, (int)strlen(buf), 10000, _linkId, true);
}

void HttpServer::Response500(const char* error)
{
    char buf[256];
    int len = (error != NULL) ? (int)strlen(error) : 0;
    sprintf(buf, _format500, len);
    _esp->SendData(buf, (int)strlen(buf), 10000, _linkId, true);
    if (len > 0)
    {
        _esp->SendData(error, len, 10000, _linkId, true);
    }
}


bool HttpServer::ProcessRequest()
{
    if (!ReadHeaders(100000))
    {
        return false;
    }

    bool rc = false;

    if (_req.Parse(_buf, _currentBufPos))
    {
        printf("headers read, content-length: %d\n", _req.GetContentLength());
        printf("method: %d, uri: %s\n", _req.GetMethod(), _req.GetURI());
        if (!ReadBody())
        {
            Response400();
        }
        else  if (!strcmp(_req.GetURI(), "/"))
        {
            printf("serving main page\n");
            //ServeMainPage(_mainPage, "text/html; charset=utf-8");
            ServeMainPage(_mainPage, CONTENT_TYPE_TEXT_HTML);
        }
        else  if (!strcmp(_req.GetURI(), HTTP_EP_GET_AP_LIST))
        {
            //Ajax request to get AP list
            printf("process get_ap_list\n");
            ProcessGetApList();
        }
        else  if (!strcmp(_req.GetURI(), HTTP_EP_SET_AP_CONFIG))
        {
            //Ajax request to configure SSID;password for router
            printf("process set_ap_config\n");
            ProcessSetupAPConfing();
        }
        else  if (strstr(_req.GetURI(), HTTP_EP_QUERY_AP_CONFIG))
        {
            //Ajax request to check if router has been configured 
            //check saved settings in FLASH, if ssid is set, return it 
            printf("process query_ap_config\n");
            QueryAPConfig();
        }
        else  if (strstr(_req.GetURI(), HTTP_EP_SET_BRIGHTNESS))
        {
            fprintf(stderr, "received set brightness req\n");
            ProcessSetBrightness();
        }
        else
        {
            printf("sending 404 for: %s", _req.GetURI());
            Response404();
        }

#ifdef HTTP_DEBUG_MODE
        if (_esp->IsConnectionClosed(_linkId))
        {
            printf("After sendResponse: Connection closed\n");
        }
        rc = true;
#endif
    }
    else
    {
        rc = false;
        printf("Error parsing headers\n");
    }
    return rc;
}

bool HttpServer::Run()
{
    _running = true;

    if (!_initialized && !Init())
    {
        _running = false;
        printf("HttpSever: initialize failed, exiting\n");
        return false;
    }
    while (_running)
    {
        ProcessRequest();
    }
    return true;
}
