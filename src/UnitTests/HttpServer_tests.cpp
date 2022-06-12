#include "../common/Esp8266.h"
#include "../http_server/HttpServer.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockEsp8266.h"
#include "MockSerial.h"
#include "MockTimer.h"

class HttpServerTests : public ::testing::TestWithParam<const char*>
{
public:
    virtual void SetUp()
    {
        _esp = std::make_shared<MockEsp8266>();
        _srv = std::make_shared<HttpServer>(_esp.get(), _mainPage, &_ps, &_timer, 3);
        _timer.SetReturnValue(0);
    }
    virtual void TearDown()
    {
        Mock::VerifyAndClearExpectations(&_esp);
    }

    void SetMainPage(const char* html)
    {
        _srv = std::make_shared<HttpServer>(_esp.get(), html, &_ps, &_timer, 3);
    }

    bool ProcessRequest()
    {
        _srv->_running = true;
        return _srv->ProcessRequest();
    }

    void SetReadBuffer(const char* buf, bool result = true)
    {
        EXPECT_CALL(*_esp.get(), ReceiveData(_, _, _, _, _)).
            WillOnce(DoAll(SetArrayArgument<0>(buf, buf + strlen(buf)),
                SetArgReferee<1>((uint32_t)strlen(buf)),
                Return(result)));
    }

    void SetApListBuffer(const char* buf, bool result = true)
    {
        EXPECT_CALL(*_esp.get(), GetListOfAps(_, _)).
            WillOnce(DoAll(SetArrayArgument<0>(buf, buf + strlen(buf)),
                Return(result)));
    }

    const char* GetFormat200() { return HttpServer::_format200;  }
    const char* GetFormat400() { return HttpServer::_format400; }
    const char* GetFormat404() { return HttpServer::_format404; }
    const char* GetFormat500() { return HttpServer::_format500; }

protected:
    std::shared_ptr<MockEsp8266> _esp;
    std::shared_ptr<HttpServer> _srv;
    PersistedSettings _ps;
    MockTimer _timer;
    const char* _mainPage = "<html><body>test</body></html>";
private:
};

TEST_F(HttpServerTests, Init_Success)
{
    MockEsp8266 esp;
    PersistedSettings ps;
    HttpServer srv(&esp, "", &ps, &_timer, 3);
    EXPECT_CALL(esp, ConnectToAP).Times(0);
    EXPECT_TRUE(srv.Init());
}

TEST_F(HttpServerTests, InitWaitBootError_Failure)
{
    MockEsp8266 esp;
    PersistedSettings ps;
    HttpServer srv (&esp, "", &ps, &_timer, 3);
    EXPECT_CALL(esp, WaitBoot()).WillOnce(Return(false));
    EXPECT_FALSE(srv.Run());
}

TEST_F(HttpServerTests, InitSetModetError_Failure)
{
    MockEsp8266 esp;
    PersistedSettings ps;
    HttpServer srv(&esp, "", &ps, &_timer, 3);
    EXPECT_CALL(esp, SetMode(_)).WillOnce(Return(false));
    EXPECT_FALSE(srv.Run());
}

TEST_F(HttpServerTests, InitSsidSet_ConnectToAP_Success)
{
    MockEsp8266 esp;
    PersistedSettings ps;
    const char* ssid = "ssid1";
    const char* pass = "pass1";
    ps.SetApSettings(ssid, pass);
    HttpServer srv(&esp, "", &ps, &_timer, 3);

    EXPECT_CALL(esp, ConnectToAP(StrEq(ssid), StrEq(pass), _, _)).WillOnce(Return(true));
    EXPECT_CALL(esp, SetStationIpAddress).WillOnce(Return(true));

    EXPECT_TRUE(srv.Init());
}
TEST_F(HttpServerTests, InitNoSsid_Success)
{
    MockEsp8266 esp;
    PersistedSettings ps;
    HttpServer srv(&esp, "", &ps, &_timer, 3);

    EXPECT_CALL(esp, ConnectToAP).Times(0);
    EXPECT_CALL(esp, SetStationIpAddress).Times(0);

    EXPECT_TRUE(srv.Init());
}


TEST_F(HttpServerTests, InitSsidSet_ConnectToAP_Failure)
{
    MockEsp8266 esp;
    PersistedSettings ps;
    const char* ssid = "ssid1";
    const char* pass = "pass1";
    ps.SetApSettings(ssid, pass);
    HttpServer srv(&esp, "", &ps, &_timer, 3);

    EXPECT_CALL(esp, ConnectToAP).WillOnce(Return(false));
    EXPECT_CALL(esp, SetStationIpAddress).Times(0);

    EXPECT_FALSE(srv.Init());
}
TEST_F(HttpServerTests, InitSsidSet_SetStationIP_Failure)
{
    MockEsp8266 esp;
    PersistedSettings ps;
    const char* ssid = "ssid1";
    const char* pass = "pass1";
    ps.SetApSettings(ssid, pass);
    HttpServer srv(&esp, "", &ps, &_timer, 3);

    EXPECT_CALL(esp, ConnectToAP).WillOnce(Return(true));
    EXPECT_CALL(esp, SetStationIpAddress).WillOnce(Return(false));

    EXPECT_FALSE(srv.Init());
}

TEST_F(HttpServerTests, Init_SetMux_Failure)
{
    MockEsp8266 esp;
    PersistedSettings ps;
    const char* ssid = "ssid1";
    const char* pass = "pass1";
    ps.SetApSettings(ssid, pass);
    HttpServer srv(&esp, "", &ps, &_timer, 3);

    EXPECT_CALL(esp, ConnectToAP).WillOnce(Return(true));
    EXPECT_CALL(esp, SetStationIpAddress).WillOnce(Return(true));
    EXPECT_CALL(esp, SetMux).WillOnce(Return(false));

    EXPECT_FALSE(srv.Init());
}

TEST_F(HttpServerTests, Init_StartServer_Failure)
{
    MockEsp8266 esp;
    PersistedSettings ps;
    const char* ssid = "ssid1";
    const char* pass = "pass1";
    ps.SetApSettings(ssid, pass);
    HttpServer srv(&esp, "", &ps, &_timer, 3);

    EXPECT_CALL(esp, ConnectToAP).WillOnce(Return(true));
    EXPECT_CALL(esp, SetStationIpAddress).WillOnce(Return(true));
    EXPECT_CALL(esp, SetMux).WillOnce(Return(true));
    EXPECT_CALL(esp, StartServer).WillOnce(Return(false));

    EXPECT_FALSE(srv.Run());
}


TEST_F(HttpServerTests, ServeMainPage_Success)
{
    char buf[256];
    sprintf(buf, GetFormat200(), CONTENT_TYPE_TEXT_HTML, strlen(_mainPage));
    const char* http_request = "GET / HTTP/1.1\r\n\r\n";
    {
        InSequence seq;
        SetReadBuffer(http_request);
        EXPECT_CALL(*_esp.get(), SendData(StrEq(buf), (int)strlen(buf),_,_,_)).WillOnce(Return(true));
        EXPECT_CALL(*_esp.get(), SendData(StrEq(_mainPage), (int)strlen(_mainPage), _, _, _)).WillOnce(Return(true));
    }

    EXPECT_TRUE(ProcessRequest());
}

TEST_F(HttpServerTests, ServeMainPageWithTemplate_Success)
{
    char buf[256];
    const char* mainPageTpl = "<html><body>....$AP_LIST$....$AP_STATUS$..</body></html>";
    const char* mainPageResult = "<html><body>....ssid1\r\nssid2\r\nssid3\r\n....ssid1..</body></html>";
    const char* aplist = "ssid1\r\nssid2\r\nssid3\r\n";
    sprintf(buf, GetFormat200(), CONTENT_TYPE_TEXT_HTML, strlen(mainPageResult));
    _ps.SetApSettings("ssid1", "pass1");
    SetMainPage(mainPageTpl);
    SetApListBuffer(aplist);

    const char* http_request = "GET / HTTP/1.1\r\n\r\n";
    {
        InSequence seq;
        SetReadBuffer(http_request);
        EXPECT_CALL(*_esp.get(), SendData(StrEq(buf), (int)strlen(buf), _, _, _)).WillOnce(Return(true));
        EXPECT_CALL(*_esp.get(), SendData(StrEq(mainPageResult), (int)strlen(mainPageResult), _, _, _)).WillOnce(Return(true));
    }

    EXPECT_TRUE(ProcessRequest());
}

TEST_F(HttpServerTests, ServeMainPageIncompleteTemplate_Success)
{
    char buf[256];
    const char* mainPageTpl = "<html><body>....$AP_";
    const char* aplist = "ssid1\r\nssid2\r\nssid3\r\n";
    sprintf(buf, GetFormat200(), CONTENT_TYPE_TEXT_HTML, strlen(mainPageTpl));
    _ps.SetApSettings("ssid1", "pass1");
    SetMainPage(mainPageTpl);
    SetApListBuffer(aplist);

    const char* http_request = "GET / HTTP/1.1\r\n\r\n";
    {
        InSequence seq;
        SetReadBuffer(http_request);
        EXPECT_CALL(*_esp.get(), SendData(StrEq(buf), (int)strlen(buf), _, _, _)).WillOnce(Return(true));
        EXPECT_CALL(*_esp.get(), SendData(StrEq(mainPageTpl), (int)strlen(mainPageTpl), _, _, _)).WillOnce(Return(true));
    }

    EXPECT_TRUE(ProcessRequest());
}


TEST_F(HttpServerTests, ServeMainPageInTwoReads_Success)
{
    char buf[256];
    sprintf(buf, GetFormat200(), CONTENT_TYPE_TEXT_HTML, strlen(_mainPage));
    const char* http_request = "GET / HTTP/1.1\r\n";
    {
        InSequence seq;
        SetReadBuffer(http_request);
        SetReadBuffer("\r\n");
        EXPECT_CALL(*_esp.get(), SendData(StrEq(buf), (int)strlen(buf), _, _, _)).WillOnce(Return(true));
        EXPECT_CALL(*_esp.get(), SendData(StrEq(_mainPage), (int)strlen(_mainPage), _, _, _)).WillOnce(Return(true));
    }

    _timer.SetReturnValue(0);
    EXPECT_TRUE(ProcessRequest());
}

TEST_F(HttpServerTests, TimeoutGettingHeaders_Failure)
{
    char buf[256];
    sprintf(buf, GetFormat200(), CONTENT_TYPE_TEXT_HTML, strlen(_mainPage));
    const char* http_request = "GET / HTTP/1.1\r\n";
    SetReadBuffer(http_request);

    _timer.SetReturnValue(10000);
    EXPECT_FALSE(ProcessRequest());
}

TEST_F(HttpServerTests, ErrorParsingHeaders_Failure)
{
    const char* http_request = "PATCH / HTTP/1.1\r\n\r\n";
    SetReadBuffer(http_request);

    _timer.SetReturnValue(10000);
    EXPECT_FALSE(ProcessRequest());
}

TEST_F(HttpServerTests, Post_ReadBodyError_400)
{
    char buf[256];
    sprintf(buf, GetFormat400(), 0);
    const char* http_request = "POST / HTTP/1.1\r\n;Content-Length: 10\r\n\r\n";
    
    {
        InSequence seq;
        SetReadBuffer(http_request);
        SetReadBuffer("", false);
        EXPECT_CALL(*_esp.get(), SendData(StrEq(buf), (int)strlen(buf), _, _, _)).WillOnce(Return(true));
    }

    _timer.SetReturnValue(0);
    EXPECT_TRUE(ProcessRequest());
}

TEST_F(HttpServerTests, Get_404)
{
    char buf[256];
    sprintf(buf, GetFormat404(), 0);
    const char* http_request = "GET /missing HTTP/1.1\r\n\r\n";

    {
        InSequence seq;
        SetReadBuffer(http_request);
        EXPECT_CALL(*_esp.get(), SendData(StrEq(buf), (int)strlen(buf), _, _, _)).WillOnce(Return(true));
    }

    _timer.SetReturnValue(0);
    EXPECT_TRUE(ProcessRequest());
}

TEST_F(HttpServerTests, QueryAPConfigNotSet_Success)
{
    char buf[256];
    const char* response = "";
    sprintf(buf, GetFormat200(), CONTENT_TYPE_TEXT_HTML, strlen(response));
    std::string http_request = "GET" + std::string(HTTP_EP_QUERY_AP_CONFIG) + " HTTP/1.1\r\n\r\n";
    {
        InSequence seq;
        SetReadBuffer(http_request.c_str());
        EXPECT_CALL(*_esp.get(), SendData(StrEq(buf), (int)strlen(buf), _, _, _)).WillOnce(Return(true));
        EXPECT_CALL(*_esp.get(), SendData(StrEq(response), (int)strlen(response), _, _, _)).WillOnce(Return(true));
    }

    EXPECT_TRUE(ProcessRequest());
}

TEST_F(HttpServerTests, QueryAPConfigSsidSet_Success)
{
    char buf[256];
    const char* response = "ssid1";
    _ps.SetApSettings("ssid1", "pass1");
    sprintf(buf, GetFormat200(), CONTENT_TYPE_TEXT_HTML, strlen(response));
    std::string http_request = "GET " + std::string(HTTP_EP_QUERY_AP_CONFIG) + " HTTP/1.1\r\n\r\n";
    {
        InSequence seq;
        SetReadBuffer(http_request.c_str());
        EXPECT_CALL(*_esp.get(), SendData(StrEq(buf), (int)strlen(buf), _, _, _)).WillOnce(Return(true));
        EXPECT_CALL(*_esp.get(), SendData(StrEq(response), (int)strlen(response), _, _, _)).WillOnce(Return(true));
    }

    EXPECT_TRUE(ProcessRequest());
}

TEST_F(HttpServerTests, SetAPConfig_Success)
{
    char buf[256];
    const char* body = "ssid1;pass1";
    sprintf(buf, GetFormat200(), CONTENT_TYPE_TEXT_HTML, 0);
    std::string http_request = "POST " + std::string(HTTP_EP_SET_AP_CONFIG) + " HTTP/1.1\r\nContent-Length: 11\r\n\r\n";
    {
        InSequence seq;
        SetReadBuffer(http_request.c_str());
        SetReadBuffer(body);
        EXPECT_CALL(*_esp.get(), SendData(StrEq(buf), (int)strlen(buf), _, _, _)).WillOnce(Return(true));
    }

    EXPECT_TRUE(ProcessRequest());
    EXPECT_STREQ(_ps.GetSsid(), "ssid1");
    EXPECT_STREQ(_ps.GetPassword(), "pass1");
}

TEST_F(HttpServerTests, GetApList_200)
{
    char buf[256];
    const char* response = "ssid1\r\nssid2\r\n";
    sprintf(buf, GetFormat200(), CONTENT_TYPE_TEXT_HTML, strlen(response));
    std::string http_request = "GET " + std::string(HTTP_EP_GET_AP_LIST) + " HTTP/1.1\r\n\r\n";
    {
        InSequence seq;
        SetReadBuffer(http_request.c_str());
        EXPECT_CALL(*_esp.get(), SendData(StrEq(buf), (int)strlen(buf), _, _, _)).WillOnce(Return(true));
        EXPECT_CALL(*_esp.get(), SendData(StrEq(response), (int)strlen(response), _, _, _)).WillOnce(Return(true));
    }

    SetApListBuffer(response);

    EXPECT_TRUE(ProcessRequest());
}

TEST_F(HttpServerTests, GetApList_500)
{
    char buf[256];
    sprintf(buf, GetFormat500(), 0);
    std::string http_request = "GET " + std::string(HTTP_EP_GET_AP_LIST) + " HTTP/1.1\r\n\r\n";
    {
        InSequence seq;
        SetReadBuffer(http_request.c_str());
        EXPECT_CALL(*_esp.get(), SendData(StrEq(buf), (int)strlen(buf), _, _, _)).WillOnce(Return(true));
    }

    EXPECT_CALL(*_esp.get(), GetListOfAps(_, _)).WillOnce(Return(false));
    EXPECT_TRUE(ProcessRequest());
}
