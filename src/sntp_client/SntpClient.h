#pragma once 
#define SNTP_SERVER "us.pool.ntp.org"
#define SNTP_PORT 123
#define DEFAULT_UDP_CONNECT_TIMEOUT 3
#define DEFAULT_UDP_RECEIVE_TIMEOUT 3
#define NTP_TIMESTAMP_DELTA 2208988800ull
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <Windows.h>
#include "../common/Esp8266.h"
#include "../common/PersistedSettings.h"

typedef struct
{

    uint8_t li_vn_mode;      // Eight bits. li, vn, and mode.
                             // li.   Two bits.   Leap indicator.
                             // vn.   Three bits. Version number of the protocol.
                             // mode. Three bits. Client will pick mode 3 for client.

    uint8_t stratum;         // Eight bits. Stratum level of the local clock.
    uint8_t poll;            // Eight bits. Maximum interval between successive messages.
    uint8_t precision;       // Eight bits. Precision of the local clock.

    uint32_t rootDelay;      // 32 bits. Total round trip delay time.
    uint32_t rootDispersion; // 32 bits. Max error allowed from primary clock source.
    uint32_t refId;          // 32 bits. Reference clock identifier.

    uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds.
    uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second.

    uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds.
    uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second.

    uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds.
    uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second.

    uint32_t txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
    uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.

} ntp_packet;              // Total: 384 bits or 48 bytes.


class Esp8266;

class SntpClient 
{
public:
  SntpClient (Esp8266* esp,
              PersistedSettings* settings,
              int US_timeZone, //1:PST 2:MST 3:CST 4:EST 
              int pollIntervalSeconds, 
              int udpConnectTimeout = DEFAULT_UDP_CONNECT_TIMEOUT, 
              int udpReceiveTimeout = DEFAULT_UDP_RECEIVE_TIMEOUT);
  void Run(); 
  void Stop();
  bool Init();
  bool GetTime(int& hr, int& min, int& sec);
  bool GetTime(int& hh, int& hl, int& mh, int& ml, int& sh, int& sl);

private:

    bool ReceivePacket();
    bool ProcessPacket(int& hr, int& min, int& sec);

    Esp8266*   _esp;
    PersistedSettings* _flashSettings;
    bool       _running;
    bool       _initialized;
    time_t     _pollIntervalSeconds;
    time_t     _txTm0;
    ntp_packet _ntp_packet;
    int        _udpConnectTimeout;
    int        _udpReceiveTimeout;
    int        _timeZone;
};

/*
tm_sec + tm_min*60 + tm_hour*3600 + tm_yday*86400 +
    (tm_year-70)*31536000 + ((tm_year-69)/4)*86400 -
    ((tm_year-1)/100)*86400 + ((tm_year+299)/400)*86400
    */