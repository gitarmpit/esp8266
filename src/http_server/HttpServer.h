#include "../common/Esp8266.h"
#include <stdio.h>
#include <chrono>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#define HTTP_PORT 80
#define SOCKET_TIMEOUT_SEC 180
#include "HttpRequest.h"
#include "../common/PersistedSettings.h"
#include "../common/Timer.h"

#define BUFFERSIZE 4096
#define CONTENT_TYPE_TEXT_HTML   "text;html"
#define HTTP_EP_SET_AP_CONFIG    "/set_ap_config"
#define HTTP_EP_QUERY_AP_CONFIG  "/query_ap_config"
#define HTTP_EP_GET_AP_LIST      "/get_ap_list"
#define HTTP_EP_SET_BRIGHTNESS   "/set_brightness"
#define AP_LIST_TAG    "AP_LIST"
#define AP_STATUS_TAG  "AP_STATUS"

extern const char* main_html;

class HttpServer
{
#ifdef UNIT_TESTS 
    friend class HttpServerTests;
#endif
public:
    HttpServer(Esp8266_Base* esp, const char* mainPage, PersistedSettings* settings, Timer* timer, int maxConnections);
    bool Run();
    bool Init();
    void Stop();

private:
    bool ReadData(int timeoutMs);
    bool ProcessRequest();
    bool ReadHeaders(int timeoutMs);
    bool ReadBody();
    void ServeMainPage(const char* body, const char* contentType);
    void Response200(const char* contentType, int len);
    void Response400();
    void Response404();
    void Response500(const char* error = NULL);
    void ProcessSetupAPConfing();
    void ProcessSetBrightness();
    void ProcessGetApList();
    void QueryAPConfig();
    void GetSsid(char* buf);
    Esp8266_Base* _esp;
    int           _maxConnections;
    bool          _running;
    bool          _initialized;
    char          _buf[BUFFERSIZE];
    int           _currentBufPos;
    int           _eofHeaderPos;
    int           _linkId;
    HttpRequest   _req;
    PersistedSettings* _flashSettings;
    Timer*        _timer;
    const char*   _mainPage;
    static const char* _format200;
    static const char* _format400;
    static const char* _format404;
    static const char* _format500;
};