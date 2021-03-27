#include "PersistedSettings.h"
#include <string.h>
#include <stdlib.h>

PersistedSettings::PersistedSettings()
{
    _ssid = NULL;
    _password = NULL;
}

void PersistedSettings::SetApSettings(const char* ssid, const char* passwd)
{
    if (ssid != NULL && strlen(ssid) > 0)
    {
        _ssid = _strdup(ssid);
    }
    else
    {
        _ssid = NULL;
    }

    if (passwd != NULL && strlen(passwd) > 0)
    {
        _password = _strdup(passwd);
    }
    else
    {
        _password = NULL;
    }
}

PersistedSettings::~PersistedSettings()
{
    free(_ssid);
    free(_password);
}


