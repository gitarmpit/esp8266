#include "ApSetup.h"
#include "HttpServer.h"
#include <string.h>
#include <stdio.h>


ApSetup::ApSetup(Esp8266_Base* esp, const char* body, HttpRequest* req)
{
    _esp = esp;
    _body = body;
    _req = req;
}

bool ApSetup::ParsePostRequest(PersistedSettings* settings)
{
    if (_req->GetMethod() != HTTP_METHOD::POST || _req->GetContentLength() == 0)
    {
        return false;
    }

    char ssid[32];
    char pass[32];
    memset(ssid, 0, sizeof ssid);
    memset(pass, 0, sizeof pass);

    // Parse "ssid;password" string received from Ajax call
    char* sptr = strchr((char*)_body, ';');
    if (sptr)
    {
        int spos = (int)(sptr - _body);
        if (spos <= 31)
        {
            strncpy(ssid, _body, spos);
            ssid[spos] = '\0';
        }
        int plen =  _req->GetContentLength() - spos  - 1;
        if (plen > 0 && plen <= 31)
        {
            strncpy(pass, &_body[spos + 1], plen);
            pass[plen] = '\0';
        }
    }

    bool rc;

    // SSID and password parsed, set station mode and connect to router
    if (strlen(ssid) && strlen(pass))
    {
        printf("Stopping server\n");
        
        if (!_esp->StopServer())
        {
            printf("Error stopping server\n");
            return false;
        }

        /*
        if (!_esp->SetMode(Esp8266::MODE::MODE_STATION_AP))
        {
            fprintf(stderr, "Error setting station mode\n");
            return false;
        }
        */

        // Verify that we can connect to AP then switch back to AP mode
        // and start server again
        if (strcmp(pass, "error") && _esp->ConnectToAP(ssid, pass, false, 10000) && _esp->IsConnectedToAP())
        {
            settings->SetApSettings(ssid, pass);
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
            bool expect_rc = _esp->Expect("FAIL", 10000);
            fprintf(stderr, "FAIL: %d\n", expect_rc);
        }

        /*
        // Switch back to AP 
        if (!_esp->SetMode(Esp8266::MODE::MODE_AP))
        {
            fprintf(stderr, "Error setting AP mode\n");
            return false;
        }

        fprintf(stderr, "AP mode set\n");
        */
        // Sleep(5000);

        fprintf(stderr, "Starting server\n");

        if (!_esp->StartServer(HTTP_PORT, 2, SOCKET_TIMEOUT_SEC))
        {
            fprintf(stderr, "StartServer error\n");
            return false;
        }

        fprintf(stderr, "Server ready\n");
        rc = true;
    }
    else
    {
        rc = false;
    }

    return rc;
}
