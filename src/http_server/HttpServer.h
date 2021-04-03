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

#define BUFFERSIZE 4096

class HttpServer
{
#ifdef UNIT_TESTS 
    friend class HttpServerTests;
#endif
public:
    HttpServer(Esp8266_Base* esp, PersistedSettings* settings, int maxConnections);
    bool Run();
    bool Init();
    void Stop();

private:
    bool ReadData(int timeoutSec);
    void ProcessRequest();
    bool ReadHeaders();
    bool ReadBody();
    void Serve(const char* body, const char* contentType);
    void Response200(const char* contentType, int len);
    void Response400(const char* text = NULL);
    void Response404(const char* text = NULL);
    void ProcessSetupAPConfing();
    void QueryAPConfig();
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

};