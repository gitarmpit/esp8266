#ifndef _HTTP_REQUEST_H
#define _HTTP_REQUEST_H

#include <stdint.h>
enum class HTTP_METHOD
{
    UNKNOWN,
    GET, 
    POST, 
    PUT
};

class HttpRequest
{
public:

    HttpRequest ();
    bool        Parse(const char* buf, int buflen);
    HTTP_METHOD GetMethod() const { return _method; }
    int         GetContentLength() const { return _contentLength; }
    const char* GetURI() const { return _uri; }

private:

    const char* _buf;
    int   _buflen; 
    char  _uri[64];
    HTTP_METHOD _method;
    int   _pos;
    int   _contentLength;

    void Reset();
    int  Find(const char* str);
    void ParseContentLength();
    bool ParseMethod();
    bool ParseUri();


};


#endif
