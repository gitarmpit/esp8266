#pragma once 
#include "HttpRequest.h"
#include "../common/Esp8266.h"
#include "../common/PersistedSettings.h"

class ApSetup
{
public:
    ApSetup(Esp8266_Base* esp, const char* body, HttpRequest* req);
    bool ParsePostRequest(PersistedSettings* settings);
private:
    Esp8266_Base*     _esp;
    HttpRequest* _req;
    const char*  _body;
};