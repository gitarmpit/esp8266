#pragma once 

class PersistedSettings 
{
public:
    PersistedSettings();
    void SetApSettings(const char* ssid, const char* passwd);
    char* GetSsid() const { return _ssid;  }
    char* GetPassword() const { return _password;  }
    ~PersistedSettings();
private:
    char* _ssid;
    char* _password;
};