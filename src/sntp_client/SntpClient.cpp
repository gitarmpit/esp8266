#include "SntpClient.h"
#include <stdint.h>

#include <string.h>
#include <time.h>


static __inline uint32_t __bswap32(uint32_t _x)
{
    return ((uint32_t)((_x >> 24) | ((_x >> 8) & 0xff00) |
        ((_x << 8) & 0xff0000) | ((_x << 24) & 0xff000000)));
}

SntpClient::SntpClient(Esp8266* esp,
    PersistedSettings* settings,
    int US_timeZone,
    int pollIntervalSeconds,
    int udpConnectTimeout,
    int udpReceiveTimeout)
{
    _esp = esp;
    _flashSettings = settings;
    _running = false;
    _initialized = false;
    _pollIntervalSeconds = pollIntervalSeconds;
    _txTm0 = 0;
    
    if (US_timeZone < 1 || US_timeZone > 4)
    {
        US_timeZone = 1;
    }

    _timeZone = US_timeZone - 9;

    _udpConnectTimeout = udpConnectTimeout;
    _udpReceiveTimeout = udpReceiveTimeout;
    memset(&_ntp_packet, 0, sizeof(ntp_packet));
}

bool SntpClient::Init()
{
    _esp->Reset();
    if (!_esp->WaitBoot())
    {
        printf("WaitBoot error\n");
        return false;
    }

    if (!_esp->SetMode(Esp8266::MODE::MODE_STATION))
    {
        printf("SetMode error\n");
        return false;
    }

    // _esp->SendCommand("AT+CWAUTOCONN?\r\n");
    // _esp->Expect("OK\r\n");

    printf("Connecting to AP...\n");
    char* ssid = _flashSettings->GetSsid();
    char* pass = _flashSettings->GetPassword();
    if (!_esp->ConnectToAP(ssid, pass, false, 5000) || !_esp->IsConnectedToAP())
    {
        printf("ConnectToAP error\n");
        return false;
    }

    printf("Connected to AP\n");
 
    _initialized = _esp->SetMux(true);
    return _initialized;
}

bool SntpClient::ReceivePacket()
{
    memset(&_ntp_packet, 0, sizeof(ntp_packet));

    _ntp_packet.li_vn_mode = 0x1b;
    int linkId = 0;
    if (!_esp->SendData((char*)&_ntp_packet, sizeof _ntp_packet, 3000, linkId))
    {
        fprintf(stderr, "SendData error\n");
        return false;
    }

    int rxBytes;
    bool rc = _esp->ReceiveData((char*)&_ntp_packet, rxBytes, linkId, _udpReceiveTimeout*1000, true);

    if (!rc || rxBytes != sizeof _ntp_packet)
    {
        fprintf(stderr, "Received data error\n");
        rc = false;
    }

    return rc;
}

bool SntpClient::ProcessPacket(int& hr, int& min, int& sec)
{
    _ntp_packet.txTm_s = __bswap32(_ntp_packet.txTm_s); // Time-stamp seconds.
    // _ntp_packet.txTm_f = __bswap32(_ntp_packet.txTm_f); // Time-stamp fraction of a second.

    time_t txTm = (time_t)(_ntp_packet.txTm_s - NTP_TIMESTAMP_DELTA);
    fprintf(stderr, "txTm: %llx\n", txTm);

    if (_ntp_packet.stratum == 0)
    {
        fprintf(stderr, "stratum is zero\n");
        return false;
    }

    // if (_txTm0 != 0 && (txTm < _txTm0 || ((txTm - _txTm0) > 3600*24)))
    if (_txTm0 != 0 && (txTm < _txTm0))
    {
        fprintf(stderr, "txTm out of range: tx: %llx tx0: %llx\n", txTm, _txTm0);
        return false;
    }

    char* ct = ctime((const time_t*)&txTm);
    if (!ct)
    {
        fprintf(stderr, "ctime error\n");
        return false;
    }

    fprintf(stderr, "Time: %s", ct);

    struct tm* tm = gmtime((const time_t*)&txTm);

    if (tm == NULL)
    {
        fprintf(stderr, "gmtime error\n");
        return false;
    }
    hr = tm->tm_hour;
    min = tm->tm_min;
    sec = tm->tm_sec;

    _txTm0 = txTm;
    return true;
}

bool SntpClient::GetTime(int& hr, int&min, int& sec)
{
    if (!_initialized)
    {
        return false;
    }

    int linkId = 0;

    if (!_esp->ConnectUDP(SNTP_SERVER, SNTP_PORT, _udpConnectTimeout * 1000, linkId))
    {
        fprintf(stderr, "Error connecting to time server\n");
        return false;
    }

    bool rc = ReceivePacket() && ProcessPacket(hr, min, sec);

    if (rc)
    {
        hr +=  _timeZone;
        if (hr < 0)
        {
            hr += 24;
        }
        else if (hr > 23)
        {
            hr -= 24;
        }
    }

    if (!_esp->CloseConnection(linkId))
    {
        rc = false;
        fprintf(stderr, "Close connection error\n");
    }

    return rc;
}

bool SntpClient::GetTime(int& hh, int& hl, int& mh, int& ml, int& sh, int& sl)
{
    int hr, min, sec;
    bool rc = false;
    
    if (GetTime(hr, min, sec))
    {
        hl = hr % 10;
        hh = hr / 10;
        ml = min % 10;
        mh = min / 10;
        sl = sec % 10;
        sh = sec / 10;
        rc = true;
    }

    return rc;
}


void SntpClient::Run()
{
    _running = true; 
    if (!Init())
    {
        _running = false;
        return;
    }

    int hr, min, sec;
    int n_failures = 0;
    while (_running)
    {
        if (GetTime(hr, min, sec))
        {
            n_failures = 0;
        }
        else if (Init())
        {
            if (++n_failures < 5)
            {
                continue;
            }
        }
        else
        {
            break;
        }
        Sleep((DWORD)_pollIntervalSeconds * 1000);
    }
    
    _running = false;
}

void SntpClient::Stop()
{
    _running = false;
}


