#include "HttpRequest.h"
#include "../common/memmem.h"
#include "string.h"
#include <stdlib.h>

HttpRequest::HttpRequest()
{
    Reset();
}

void HttpRequest::Reset()
{
    _buf = NULL;
    _buflen = 0;
    _contentLength = 0;
    _pos = 0;
    _method = HTTP_METHOD::UNKNOWN;
    memset(_uri, 0, sizeof _uri);
}

bool HttpRequest::Parse(const char* buf, int buflen)
{
    Reset();
    _buf = buf;
    _buflen = buflen;
    if (!ParseMethod())
    {
        return false;
    }
    if (!ParseUri())
    {
        return false;
    }

    ParseContentLength();
    if (_method != HTTP_METHOD::GET && _contentLength == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool HttpRequest::ParseMethod()
{
    if (!_strnicmp(_buf, "GET", 3))
    {
        _method = HTTP_METHOD::GET;
        _pos = 3;
    }
    else if (!_strnicmp(_buf, "POST", 4))
    {
        _method = HTTP_METHOD::POST;
        _pos = 4;
    }
    else if (!_strnicmp(_buf, "PUT", 3))
    {
        _method = HTTP_METHOD::PUT;
        _pos = 3;
    }
    else
    {
        return false;
    }

    return true;
}

bool HttpRequest::ParseUri()
{
    int maxlen = _buflen - _pos;
    if (maxlen < 0)
    {
        return false;
    }

    for (int i = 0; i < maxlen; ++i)
    {
        if (_buf[_pos] != ' ')
        {
            break;
        }
        ++_pos;
    }

    maxlen = _buflen - _pos;
    if (maxlen < 0)
    {
        return false;
    }
    else if (maxlen > sizeof(_uri) - 1)
    {
        maxlen = sizeof(_uri) - 1;
    }

    if (_buf[_pos] != '/')
    {
        return false;
    }

    int i;
    for (i = 0; i < maxlen; ++i)
    {
        char c = _buf[_pos++];
        if (c == ' ')
        {
            break;
        }
        _uri[i] = c;
    }
    _uri[i] = '\0';

    const char* proto = "HTTP/1.1";
    maxlen = _buflen - _pos;
    if (maxlen >= (int)strlen(proto))
    {
        return (strncmp(&_buf[_pos], proto, strlen(proto)) == 0);
    }
    else
    {
        return false;
    }
}

int HttpRequest::Find(const char* str)
{
    int pos = MemMem(&_buf[_pos], str, _buflen, (int)strlen(str));
    if (pos != -1)
    {
        pos += (int)strlen(str);
    }
    return pos;
}

void HttpRequest::ParseContentLength()
{
    int pos = Find("Content-Length:");
    if (pos == -1)
    {
        return;
    }

    _pos += pos;

    while (_buf[_pos] == ' ')
        ++_pos;

    static char n[5];
    int i = 0;
    for (; i < 5; ++i)
    {
        char c = _buf[_pos];
        if (c == '\0' || c < '0' || c > '9')
        {
            break;
        }
        n[i] = _buf[_pos];
        ++_pos;
    }
    n[i] = '\0';

    _contentLength = atoi(n);


}

