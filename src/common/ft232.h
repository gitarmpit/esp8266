#pragma once 
#include "ftd2xx.h"
#include "Serial.h"

class Ft232 : public Serial
{
public:
    Ft232();
    virtual bool      Initialize(int baudRate);
    virtual uint32_t  WaitRead(int timeoutMs);
    virtual bool      Read(char* buf, uint32_t& bytesReceived, int timeoutMs);
    virtual bool      Read(char* buf, uint32_t bytesToRead);
    virtual bool      Send(const char* buf, int size = 0);
    virtual bool      SetTimeout(int timeoutMs);
    virtual ~Ft232() { FT_Close(_ftHandle); }
private:
    bool _initialized;
    FT_HANDLE _ftHandle;
};