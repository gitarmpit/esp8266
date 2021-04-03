#include "../http_server/HttpRequest.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <vector>
#include "MockSerial.h"

using namespace testing;

class HttpServer_Tests : public ::testing::TestWithParam<const char*>
{
public:
    virtual void SetUp()
    {
        _serial = std::make_shared<MockSerial>();
    }
    virtual void TearDown()
    {
        Mock::VerifyAndClearExpectations(&_serial);
    }

    void SetReadBuffer(const char* buf, bool result = true)
    {
        EXPECT_CALL(*_serial.get(), Read(_, _, _)).
            WillOnce(DoAll(SetArrayArgument<0>(buf, buf + strlen(buf)),
                SetArgReferee<1>((uint32_t)strlen(buf)),
                Return(result)));
    }

protected:
    std::shared_ptr<MockSerial> _serial;
private:
};
TEST(HttpRequestTest, NoMethod_fail)
{
    HttpRequest request;
    const char* buf = "HTTP/1.1";
    EXPECT_FALSE(request.Parse(buf, (int)strlen(buf)));
}

TEST(HttpRequestTest, NoUrl_fail)
{
    HttpRequest request;
    const char* buf = "GET HTTP/1.1";
    EXPECT_FALSE(request.Parse(buf, (int)strlen(buf)));
}

TEST(HttpRequestTest, Minimal_Success)
{
    HttpRequest request;
    const char* buf = "GET / HTTP/1.1 \r\n";
    EXPECT_TRUE(request.Parse(buf, (int)strlen(buf)));
    EXPECT_STREQ("/", request.GetURI());
    EXPECT_EQ(HTTP_METHOD::GET, request.GetMethod());
}

TEST(HttpRequestTest, NoProto_Failure)
{
    std::vector<std::string> reqs;
    reqs.push_back("GET /");
    reqs.push_back("GET / H");
    reqs.push_back("GET / HTTP/1.");
    reqs.push_back("GET /HTTP/1.");

    for (auto req : reqs)
    {
        HttpRequest request;
        EXPECT_FALSE(request.Parse(req.c_str(), (int)req.size()));
    }
}

TEST(HttpRequestTest, POST_Success)
{
    HttpRequest request;
    const char* buf = "POST /url HTTP/1.1\r\nContent-Length: 3\r\n";
    EXPECT_TRUE(request.Parse(buf, (int)strlen(buf)));
    EXPECT_STREQ("/url", request.GetURI());
    EXPECT_EQ(HTTP_METHOD::POST, request.GetMethod());
    EXPECT_EQ(3, request.GetContentLength());
}

TEST(HttpRequestTest, PUT_Success)
{
    HttpRequest request;
    const char* buf = "PUT   / HTTP/1.1\r\nContent-Length:   3\r\n";
    EXPECT_TRUE(request.Parse(buf, (int)strlen(buf)));
    EXPECT_STREQ("/", request.GetURI());
    EXPECT_EQ(HTTP_METHOD::PUT, request.GetMethod());
    EXPECT_EQ(3, request.GetContentLength());
}


TEST(HttpRequestTest, POST_NoContent_Failure)
{
    HttpRequest request;
    const char* buf = "POST /url HTTP/1.1\r\n\r\n";
    EXPECT_FALSE(request.Parse(buf, (int)strlen(buf)));
}
