#pragma once 
#include "HttpRequest.h"
#include "../common/Esp8266.h"
#include "../common/PersistedSettings.h"

class ApSetup
{
public:
    ApSetup(Esp8266* esp, const char* body, HttpRequest* req);
    bool ParsePostRequest(PersistedSettings* settings);
private:
    Esp8266*     _esp;
    HttpRequest* _req;
    const char*  _body;
    char _ssid[32];
    char _pass[32];

};