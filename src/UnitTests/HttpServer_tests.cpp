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
    }
    virtual void TearDown()
    {
        Mock::VerifyAndClearExpectations(&_esp);
    }

    void SetReadBuffer(const char* buf, bool result = true)
    {
        EXPECT_CALL(*_esp.get(), ReceiveData(_, _, _, _, _)).
            WillOnce(DoAll(SetArrayArgument<0>(buf, buf + strlen(buf)),
                SetArgReferee<1>((uint32_t)strlen(buf)),
                Return(result)));
    }

protected:
    std::shared_ptr<MockEsp8266> _esp;
    MockTimer _timer;
private:
};

TEST_F(HttpServerTests, Init_Success)
{
    MockEsp8266 esp;
    PersistedSettings ps;
    HttpServer srv(&esp, &ps, &_timer, 3);
    EXPECT_CALL(esp, ConnectToAP).Times(0);
    EXPECT_TRUE(srv.Init());
}

TEST_F(HttpServerTests, InitWaitBootError_Failure)
{
    MockEsp8266 esp;
    PersistedSettings ps;
    HttpServer srv (&esp, &ps, &_timer, 3);
    EXPECT_CALL(esp, WaitBoot()).WillOnce(Return(false));
    EXPECT_FALSE(srv.Run());
}

TEST_F(HttpServerTests, InitSetModetError_Failure)
{
    MockEsp8266 esp;
    PersistedSettings ps;
    HttpServer srv(&esp, &ps, &_timer, 3);
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
    HttpServer srv(&esp, &ps, &_timer, 3);

    EXPECT_CALL(esp, ConnectToAP(StrEq(ssid), StrEq(pass), _, _)).WillOnce(Return(true));
    EXPECT_CALL(esp, SetStationIpAddress).WillOnce(Return(true));

    EXPECT_TRUE(srv.Init());
}
TEST_F(HttpServerTests, InitNoSsid_Success)
{
    MockEsp8266 esp;
    PersistedSettings ps;
    HttpServer srv(&esp, &ps, &_timer, 3);

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
    HttpServer srv(&esp, &ps, &_timer, 3);

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
    HttpServer srv(&esp, &ps, &_timer, 3);

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
    HttpServer srv(&esp, &ps, &_timer, 3);

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
    HttpServer srv(&esp, &ps, &_timer, 3);

    EXPECT_CALL(esp, ConnectToAP).WillOnce(Return(true));
    EXPECT_CALL(esp, SetStationIpAddress).WillOnce(Return(true));
    EXPECT_CALL(esp, SetMux).WillOnce(Return(true));
    EXPECT_CALL(esp, StartServer).WillOnce(Return(false));

    EXPECT_FALSE(srv.Run());
}

TEST_F(HttpServerTests, NextTest_Failure)
{
    MockEsp8266 esp;
    PersistedSettings ps;
    const char* ssid = "ssid1";
    const char* pass = "pass1";
    ps.SetApSettings(ssid, pass);
    HttpServer srv(&esp, &ps, &_timer, 3);

    EXPECT_CALL(esp, ConnectToAP).WillOnce(Return(true));
    EXPECT_CALL(esp, SetStationIpAddress).WillOnce(Return(true));
    EXPECT_CALL(esp, SetMux).WillOnce(Return(true));
    EXPECT_CALL(esp, StartServer).WillOnce(Return(false));

    EXPECT_FALSE(srv.Init());
}
