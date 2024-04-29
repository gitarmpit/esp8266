#pragma once 
#include <stdint.h>

#define UNUSED(x) (void)(x)

// Base interfaces 
// Implementing classes: FT232 or WinSock on PC, UART on uC 

class Serial 
{
public:
    virtual bool      Initialize(int baudRate) = 0;
    // Returns the number of bytes in the queue
    virtual uint32_t  WaitRead(int timeoutMs) = 0;
    virtual bool      Read(char* buf, int& bytesReceived, int timeoutMs) = 0;
    // Read exact number of bytes, or block (can be used with SetTimeout)
    //virtual bool      Read(char* buf, uint32_t bytesToRead) = 0;
    virtual bool      Send(const char* buf, int size = 0) = 0;
    virtual bool      SetTimeout(int timeoutMs) = 0;
    virtual ~Serial() {}
};