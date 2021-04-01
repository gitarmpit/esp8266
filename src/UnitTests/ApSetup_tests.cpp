#include "../common/Esp8266.h"
#include "../http_server/ApSetup.h"
#include "../http_server/HttpRequest.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "MockEsp8266.h"

using namespace testing;


TEST(ApSetupTests, ParsePostRequest_Success)
{
    MockEsp8266 mockEsp8266;
    
    EXPECT_CALL(mockEsp8266, StopServer).WillOnce(Return(true));
    EXPECT_CALL(mockEsp8266, SetMode).WillRepeatedly(Return(true));
    EXPECT_CALL(mockEsp8266, ConnectToAP).WillOnce(Return(true));
    EXPECT_CALL(mockEsp8266, IsConnectedToAP).WillOnce(Return(true));
    EXPECT_CALL(mockEsp8266, DisconnectFromAP).WillOnce(Return(true));
    EXPECT_CALL(mockEsp8266, StartServer).WillOnce(Return(true));

    const char* body = "ssid1;pass1";
    HttpRequest req;
    const char* header = "POST /url HTTP/1.1\r\nContent-Length: 11\r\n";
    ASSERT_TRUE(req.Parse(header, (int)strlen(header)));
    ApSetup apSetup(&mockEsp8266, body, &req);
    PersistedSettings ps;
    EXPECT_TRUE(apSetup.ParsePostRequest(&ps));
    EXPECT_STREQ("ssid1", ps.GetSsid());
    EXPECT_STREQ("pass1", ps.GetPassword());
}

TEST(ApSetupTests, ParsePostRequest_ConnectError_Success)
{
    MockEsp8266 mockEsp8266;

    EXPECT_CALL(mockEsp8266, StopServer).WillOnce(Return(true));
    EXPECT_CALL(mockEsp8266, SetMode).WillRepeatedly(Return(true));
    EXPECT_CALL(mockEsp8266, ConnectToAP).WillOnce(Return(true));
    EXPECT_CALL(mockEsp8266, IsConnectedToAP).WillOnce(Return(false));
    EXPECT_CALL(mockEsp8266, DisconnectFromAP).Times(0);
    EXPECT_CALL(mockEsp8266, StartServer).WillOnce(Return(true));

    const char* body = "ssid1;pass1";
    HttpRequest req;
    const char* header = "POST /url HTTP/1.1\r\nContent-Length: 11\r\n";
    ASSERT_TRUE(req.Parse(header, (int)strlen(header)));
    ApSetup apSetup(&mockEsp8266, body, &req);
    PersistedSettings ps;
    EXPECT_TRUE(apSetup.ParsePostRequest(&ps));
    EXPECT_EQ(NULL, ps.GetSsid());
    EXPECT_EQ(NULL, ps.GetPassword());
}

TEST(ApSetupTests, ParsePostRequest_ParseError_Success)
{
    MockEsp8266 mockEsp8266;

    EXPECT_CALL(mockEsp8266, StopServer).Times(0);
    EXPECT_CALL(mockEsp8266, SetMode).Times(0);
    EXPECT_CALL(mockEsp8266, ConnectToAP).Times(0);
    EXPECT_CALL(mockEsp8266, IsConnectedToAP).Times(0);
    EXPECT_CALL(mockEsp8266, DisconnectFromAP).Times(0);
    EXPECT_CALL(mockEsp8266, StartServer).Times(0);

    const char* body = "ssid1";
    HttpRequest req;
    const char* header = "POST /url HTTP/1.1\r\nContent-Length: 6\r\n";
    ASSERT_TRUE(req.Parse(header, (int)strlen(header)));
    ApSetup apSetup(&mockEsp8266, body, &req);
    PersistedSettings ps;
    EXPECT_FALSE(apSetup.ParsePostRequest(&ps));
}
