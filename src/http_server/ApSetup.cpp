#include "ApSetup.h"
#include "HttpServer.h"
#include <string.h>
#include <stdio.h>


ApSetup::ApSetup(Esp8266* esp, const char* body, HttpRequest* req)
{
    _esp = esp;
    _body = body;
    _req = req;
    memset(_ssid, 0, sizeof _ssid);
    memset(_pass, 0, sizeof _pass);
}

bool ApSetup::ParsePostRequest(PersistedSettings* settings)
{
    if (_req->GetMethod() != HTTP_METHOD::POST || _req->GetContentLength() == 0)
    {
        return false;
    }

    // Parse "ssid;password" string received from Ajax call
    char* sptr = strchr((char*)_body, ';');
    if (sptr)
    {
        int spos = (int)(sptr - _body);
        if (spos <= 31)
        {
            strncpy(_ssid, _body, spos);
            _ssid[spos] = '\0';
        }
        int plen =  _req->GetContentLength() - spos  - 1;
        if (plen > 0 && plen <= 31)
        {
            strncpy(_pass, &_body[spos + 1], plen);
            _pass[plen] = '\0';
        }
    }

    // SSID and password parsed, set station mode and connect to router
    if (strlen(_ssid) && strlen(_pass))
    {
        printf("Stopping server\n");
        
        if (!_esp->StopServer())
        {
            printf("Error stopping server\n");
            return false;
        }

        if (!_esp->SetMode(Esp8266::MODE::MODE_STATION_AP))
        {
            fprintf(stderr, "Error setting station mode\n");
            return false;
        }

        // Verify that we can connec to AP then switch back to AP mode
        // and start server again
        if (_esp->ConnectToAP(_ssid, _pass, false, 10000) && _esp->IsConnectedToAP())
        {
            settings->SetApSettings(_ssid, _pass);
            fprintf(stderr, "Disconnecting from AP\n");
            if (!_esp->DisconnectFromAP())
            {
                fprintf(stderr, "DisconnectFromAP error\n");
            }
        }
        else
        {
            settings->SetApSettings(NULL, NULL);
            fprintf(stderr, "ConnectToAP error\n");
            bool rc = _esp->Expect("FAIL", 10000);
            fprintf(stderr, "FAIL: %d\n", rc);
        }

        // Switch back to AP 
        if (!_esp->SetMode(Esp8266::MODE::MODE_AP))
        {
            fprintf(stderr, "Error setting AP mode\n");
            return false;
        }

        fprintf(stderr, "AP mode set\n");
        // Sleep(5000);

        fprintf(stderr, "Starting server\n");

        if (!_esp->StartServer(HTTP_PORT, 2, SOCKET_TIMEOUT_SEC))
        {
            fprintf(stderr, "StartServer error\n");
            return false;
        }

        fprintf(stderr, "Server ready\n");

    }

    return true;
}
