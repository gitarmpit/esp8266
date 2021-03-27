#include "ft232.h"
#include <stdio.h>
#include <chrono>

Ft232::Ft232()
{
    _initialized = false;
    _ftHandle = NULL;
}

bool Ft232::Initialize(int baudRate)
{
    if (FT_Open(0, &_ftHandle) != FT_OK)
    {
        return false;
    }

    //115200
    if (FT_SetBaudRate(_ftHandle, (ULONG)baudRate) != FT_OK)
    {
        return false;
    }

    DWORD status = FT_SetDataCharacteristics(_ftHandle, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE);
    return status == FT_OK;
}

uint32_t Ft232::WaitRead(int timeoutMs)
{
    if (timeoutMs < 0)
    {
        timeoutMs = 0;
    }

    DWORD rxBytes = 0;

    auto start = std::chrono::steady_clock::now();
    while (true)
    {
        rxBytes = 0;
        if ((FT_GetQueueStatus(_ftHandle, &rxBytes) != FT_OK) ||
            (rxBytes > 0))
        {
            break;
        }

        if (timeoutMs != 0)
        {
            auto now = std::chrono::steady_clock::now();
            auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(now - start);
            if (dur.count() > timeoutMs)
            {
                break;
            }
        }

        Sleep(1);
    }

    return rxBytes;
}

bool Ft232::Read(char* buf, uint32_t& bytesReceived, int timeoutMs)
{
    bytesReceived = 0;
    bool rc = true;
    DWORD rxBytes = WaitRead(timeoutMs);
    if (rxBytes != 0)
    {
        FT_STATUS status = FT_Read(_ftHandle, buf, rxBytes, (DWORD*)&bytesReceived);
        if (status != FT_OK)
        {
            rc = false;
        }
    }
    return rc;
}

bool Ft232::Read(char* buf, uint32_t bytesToRead)
{
    bool rc = true;
    DWORD bytesReceived = 0;
    FT_STATUS status = FT_Read(_ftHandle, buf, bytesToRead, &bytesReceived);
    if (status != FT_OK || bytesReceived != bytesToRead)
    {
        rc = false;
    }
    return rc;
}


bool Ft232::SetTimeout(int timeoutMs)
{
    return FT_SetTimeouts(_ftHandle, (ULONG)timeoutMs, (ULONG)timeoutMs) == FT_OK;
}

bool Ft232::Send(const char* buf, int size)
{
    if (size == 0)
    {
        size = (int)strlen(buf);
    }
    
    DWORD bytesWritten = 0;
    if (FT_Write(_ftHandle, (LPVOID)buf, (DWORD)size, &bytesWritten) != FT_OK)
    {
        return false;
    }
    else if (bytesWritten != (DWORD)size)
    {
        return false;
    }

    return true;
}