#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../common/Serial.h"
#include "../common/PersistedSettings.h"
#include "../common/Esp8266.h"
#include "../common/memmem.h"

using namespace testing;

class MockSerial : public Serial
{
public:
    MockSerial()
    {
        ON_CALL(*this, Read(_, _)).WillByDefault(Return(false));
        ON_CALL(*this, Read(_, _, _)).WillByDefault(Return(false));
        ON_CALL(*this, Send).WillByDefault(Return(true));
        ON_CALL(*this, SetTimeout).WillByDefault(Return(true));
        ON_CALL(*this, WaitRead).WillByDefault(Return(0));

    }
    MOCK_METHOD(bool, Initialize, (int baudRate), (override));
    MOCK_METHOD(uint32_t, WaitRead, (int timeoutMs), (override));
    MOCK_METHOD(bool, Read, (char* buf, uint32_t& bytesReceived, int timeoutMs), (override));
    MOCK_METHOD(bool, Read, (char* buf, uint32_t bytesToRead), (override));
    MOCK_METHOD(bool, Send, (const char* buf, int size), (override));
    MOCK_METHOD(bool, SetTimeout, (int timeoutMs), (override));
};

class Esp8266_Tests : public ::testing::Test
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

class MockTimer : public Timer
{
public:
    MockTimer() { _val = 1000000; }
    void SetReturnValue(int val)
    {
        _val = val;
    }
    virtual int ElapsedMs()
    {
        return _val;
    }
private:
    int _val;
};


class MockEsp8266 : public Esp8266
{
public:

    MockEsp8266(Serial* serial, bool autoReconnect, bool echoOff, int timerVal) : 
        Esp8266(serial, autoReconnect, echoOff)
    {
        _timer.SetReturnValue(timerVal);
        ON_CALL(*this, _Log).WillByDefault(Return());
    }

    MOCK_METHOD(void, _Log, (const char*), (override));

    virtual Timer& CreateTimer()
    {
        return _timer;
    }

    int GetDataBytesRead()
    {
        return _dataBytesRead;
    }
    int GetDataBufPos()
    {
        return _databufPos;
    }
    char* GetBuf()
    {
        return _databuf;
    }

private:
    MockTimer _timer;
};

TEST_F(Esp8266_Tests, SetStationIpAddress_Success)
{
    MockSerial serial;
    PersistedSettings ps;
    ps.SetApSettings("ssid", "pass");
    bool echoOff = true;
    bool autoConnect = false;
    Esp8266 esp(&serial, autoConnect, echoOff);
    std::string ipAddress = "127.0.0.1";
    std::string expected_str = AT_CIPSTA_CUR + std::string("=\"") + ipAddress + "\"\r\n";
    EXPECT_CALL(serial, Send(StrEq(expected_str.c_str()), 0)).WillOnce(Return(true));

    esp.SetStationIpAddress(ipAddress.c_str());
}

TEST_F(Esp8266_Tests, EchoOff_Success)
{
    SetReadBuffer(AT_OK);
    std::string command = AT_ATE0;
    EXPECT_CALL(*_serial.get(), Send(StrEq(command.c_str()), 0)).WillOnce(Return(true));
    MockEsp8266 esp(_serial.get(), false, false, 1000000);
    bool rc = esp.EchoOff();
    EXPECT_TRUE(rc);
}

TEST_F(Esp8266_Tests, WaitBoot_Success)
{
    {
        InSequence seq;
        SetReadBuffer("ready\r\n");
        SetReadBuffer(AT_OK);
    }

    std::string command = std::string(AT_CWAUTOCONN) + "=0\r\n";
    EXPECT_CALL(*_serial.get(), Send(StrEq(command.c_str()), 0)).WillOnce(Return(true));
    MockEsp8266 esp(_serial.get(), false, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    bool rc = esp.WaitBoot();
    EXPECT_TRUE(rc);
}

TEST_F(Esp8266_Tests, Reset_Success)
{
    std::string command = AT_RST;
    EXPECT_CALL(*_serial.get(), Send(StrEq(command.c_str()), 0)).WillOnce(Return(true));
    MockEsp8266 esp(_serial.get(), false, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    esp.Reset();
}

TEST_F(Esp8266_Tests, FactoryRestore_Success)
{
    std::string command = AT_RESTORE;
    std::string command2 = std::string(AT_CWAUTOCONN) + "=0\r\n";
    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq(command.c_str()), 0)).WillOnce(Return(true));
        SetReadBuffer(AT_OK);
        SetReadBuffer("ready\r\n");
        EXPECT_CALL(*_serial.get(), Send(StrEq(command2.c_str()), 0)).WillOnce(Return(true));
        SetReadBuffer(AT_OK);
    }

    MockEsp8266 esp(_serial.get(), false, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    bool rc = esp.FactoryRestore();
    EXPECT_TRUE(rc);

}

TEST_F(Esp8266_Tests, DisconnectFromAP_Success)
{
    std::string command = AT_CWQAP;
    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq(command.c_str()), 0)).WillOnce(Return(true));
        SetReadBuffer(AT_OK);
    }

    MockEsp8266 esp(_serial.get(), false, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    bool rc = esp.DisconnectFromAP();
    EXPECT_TRUE(rc);
}

TEST_F(Esp8266_Tests, ConnectToAP_HasCreds_SaveToFlash_Success)
{
    std::string ssid = "ssid";
    std::string password = "password";

    std::string command = std::string(AT_CWJAP_DEF) + "=\"" + ssid + "\",\"" + password + "\"\r\n";

    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq(command.c_str()), 0)).WillOnce(Return(true));
        SetReadBuffer(WIFI_CONNECTED);
        SetReadBuffer(WIFI_GOT_IP);
        SetReadBuffer(AT_OK);
    }

    MockEsp8266 esp(_serial.get(), false, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    bool rc = esp.ConnectToAP(ssid.c_str(), password.c_str(), true, 100);
    EXPECT_TRUE(rc);
}

TEST_F(Esp8266_Tests, ConnectToAP_NoCreds_AutoConnect_Success)
{
    {
        InSequence seq;
        SetReadBuffer(WIFI_CONNECTED);
        SetReadBuffer(WIFI_GOT_IP);
    }

    MockEsp8266 esp(_serial.get(), true, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    bool rc = esp.ConnectToAP(NULL, NULL, true, 100);
    EXPECT_TRUE(rc);
}

TEST_F(Esp8266_Tests, ConnectToAP_NoCreds_NoAutoConnect_Failure)
{
    MockEsp8266 esp(_serial.get(), false, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    bool rc = esp.ConnectToAP(NULL, NULL, true, 100);
    EXPECT_FALSE(rc);
}

TEST_F(Esp8266_Tests, SetMode_Success)
{
    std::string command = std::string(AT_CWMODE_CUR) + "=2\r\n";
    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq(command.c_str()), 0)).WillOnce(Return(true));
        SetReadBuffer(AT_OK);
    }
    MockEsp8266 esp(_serial.get(), false, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    bool rc = esp.SetMode(Esp8266::MODE::MODE_AP);
    EXPECT_TRUE(rc);
}

TEST_F(Esp8266_Tests, SetAutoconnect_Success)
{
    std::string command = std::string(AT_CWAUTOCONN) + "=1\r\n";
    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq(command.c_str()), 0)).WillOnce(Return(true));
        SetReadBuffer(AT_OK);
    }
    MockEsp8266 esp(_serial.get(), false, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    bool rc = esp.SetAutoConnect(true);
    EXPECT_TRUE(rc);
}

TEST_F(Esp8266_Tests, IsConnectedToAP_Success)
{
    std::string command = AT_CWJAP_CUR_GET;
    std::string read_buffer = std::string(AT_OK) + "some garbage  " + std::string(CWJAP_CUR_PATTERN);
    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq(command.c_str()), 0)).WillOnce(Return(true));
        SetReadBuffer(read_buffer.c_str());
    }
    MockEsp8266 esp(_serial.get(), false, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    bool rc = esp.IsConnectedToAP();
    EXPECT_TRUE(rc);
}

TEST_F(Esp8266_Tests, IsConnectedToAP_Failure)
{
    std::string command = AT_CWJAP_CUR_GET;
    std::string read_buffer = std::string(AT_OK) + "some garbage  ";
    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq(command.c_str()), 0)).WillOnce(Return(true));
        SetReadBuffer(read_buffer.c_str());
    }
    MockEsp8266 esp(_serial.get(), false, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    bool rc = esp.IsConnectedToAP();
    EXPECT_FALSE(rc);
}

TEST_F(Esp8266_Tests, SetMux_Success)
{
    std::string command = std::string(AT_CIPMUX) + "=1\r\n";
    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq(command.c_str()), 0)).WillOnce(Return(true));
        SetReadBuffer(AT_OK);
    }
    MockEsp8266 esp(_serial.get(), false, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    bool rc = esp.SetMux(true);
    EXPECT_TRUE(rc);
}

TEST_F(Esp8266_Tests, StopServer_Success)
{
    std::string command = std::string(AT_CIPSERVER) + "=0\r\n";
    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq(command.c_str()), 0)).WillOnce(Return(true));
        SetReadBuffer(AT_OK);
    }
    MockEsp8266 esp(_serial.get(), false, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    bool rc = esp.StopServer();
    EXPECT_TRUE(rc);
}

TEST_F(Esp8266_Tests, StartServer_Success)
{
    int port = 999;
    std::string command = std::string(AT_CIPSERVER) + "=1,999\r\n";
    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq(command.c_str()), 0)).WillOnce(Return(true));
        SetReadBuffer(AT_OK);
    }
    MockEsp8266 esp(_serial.get(), false, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    bool rc = esp.StartServer(port, 1, 0);
    EXPECT_TRUE(rc);
}

TEST_F(Esp8266_Tests, ConnectUdp_Success)
{
    std::string hostName = "hostName";

    std::string command = "AT+CIPSTART=\"UDP\",\"hostName\",9999\r\n";
    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq(command.c_str()), 0)).WillOnce(Return(true));
        SetReadBuffer(AT_OK);
    }
    MockEsp8266 esp(_serial.get(), false, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    bool rc = esp.ConnectUDP(hostName.c_str(), 9999, 100, -1);
    EXPECT_TRUE(rc);
}

TEST_F(Esp8266_Tests, ConnectUdp2_Success)
{
    std::string hostName = "hostName";

    std::string command = "AT+CIPSTART=3,\"UDP\",\"hostName\",9999\r\n";
    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq(command.c_str()), 0)).WillOnce(Return(true));
        SetReadBuffer(AT_OK);
    }
    MockEsp8266 esp(_serial.get(), false, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    bool rc = esp.ConnectUDP(hostName.c_str(), 9999, 100, 3);
    EXPECT_TRUE(rc);
}

TEST_F(Esp8266_Tests, ConnectTcp_Success)
{
    std::string hostName = "hostName";

    std::string command = "AT+CIPSTART=\"TCP\",\"hostName\",9999\r\n";
    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq(command.c_str()), 0)).WillOnce(Return(true));
        SetReadBuffer(AT_OK);
    }
    MockEsp8266 esp(_serial.get(), false, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    bool rc = esp.ConnectTCP(hostName.c_str(), 9999, 100, -1);
    EXPECT_TRUE(rc);
}

TEST_F(Esp8266_Tests, ConnectTcp2_Success)
{
    std::string hostName = "hostName";

    std::string command = "AT+CIPSTART=3,\"TCP\",\"hostName\",9999\r\n";
    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq(command.c_str()), 0)).WillOnce(Return(true));
        SetReadBuffer(AT_OK);
    }
    MockEsp8266 esp(_serial.get(), false, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    bool rc = esp.ConnectTCP(hostName.c_str(), 9999, 100, 3);
    EXPECT_TRUE(rc);
}

TEST_F(Esp8266_Tests, CloseConnection_Success)
{
    std::string command = AT_CIPCLOSE + std::string(AT_CRLF);
    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq(command.c_str()), 0)).WillOnce(Return(true));
        SetReadBuffer(AT_OK);
    }
    MockEsp8266 esp(_serial.get(), false, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    bool rc = esp.CloseConnection(-1);
    EXPECT_TRUE(rc);
}

TEST_F(Esp8266_Tests, CloseConnection2_Success)
{
    std::string command = AT_CIPCLOSE + std::string("=3") + std::string(AT_CRLF);
    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq(command.c_str()), 0)).WillOnce(Return(true));
        SetReadBuffer(AT_OK);
    }
    MockEsp8266 esp(_serial.get(), false, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    bool rc = esp.CloseConnection(3);
    EXPECT_TRUE(rc);
}

TEST_F(Esp8266_Tests, PassthruOn_Success)
{
    std::string command1 = "AT+CIPMODE=1\r\n";
    std::string command2 = "AT+CIPSEND\r\n";
    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq(command1.c_str()), 0)).WillOnce(Return(true));
        SetReadBuffer(AT_OK);
        EXPECT_CALL(*_serial.get(), Send(StrEq(command2.c_str()), 0)).WillOnce(Return(true));
        SetReadBuffer(AT_OK"> ");
    }
    MockEsp8266 esp(_serial.get(), false, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    bool rc = esp.PassthroughMode(true);
    EXPECT_TRUE(rc);
}

TEST_F(Esp8266_Tests, PassthruOff_Success)
{
    std::string command = "AT+CIPMODE=0\r\n";
    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq("+++"), 0)).WillOnce(Return(true));
        EXPECT_CALL(*_serial.get(), Send(StrEq(command.c_str()), 0)).WillOnce(Return(true));
        SetReadBuffer(AT_OK);
    }
    MockEsp8266 esp(_serial.get(), false, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    bool rc = esp.PassthroughMode(false);
    EXPECT_TRUE(rc);
    esp._Log("");
}

TEST_F(Esp8266_Tests, Expect_3patterns_2reads_Success)
{
    InSequence seq;
    SetReadBuffer("----<1>---<2>");
    SetReadBuffer("---.");
    MockEsp8266 esp(_serial.get(), false, false, 1000000);
    EXPECT_TRUE(esp.Expect("<1>", 1));
    EXPECT_TRUE(esp.Expect("<2>", 1));
    EXPECT_TRUE(esp.Expect(".", 1));
}

TEST_F(Esp8266_Tests, Expect_3patterns_4reads_Success)
{
    InSequence seq;
    SetReadBuffer("---1");
    SetReadBuffer("---2");
    SetReadBuffer("---3");
    SetReadBuffer("---4");
    MockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_TRUE(esp.Expect("1", 1));
    EXPECT_TRUE(esp.Expect("2", 1));
    EXPECT_TRUE(esp.Expect("4", 1));
}

TEST_F(Esp8266_Tests, Expect_TwoReads_Success)
{
    {
        InSequence seq;
        SetReadBuffer("---1");
        SetReadBuffer("---2");
    }
    MockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    EXPECT_CALL(esp, _Log(StrEq("expecting: 1\n"))).Times(1);
    EXPECT_CALL(esp, _Log(StrEq("expecting: 2\n"))).Times(1);
    EXPECT_TRUE(esp.Expect("1", 1));
    EXPECT_TRUE(esp.Expect("2", 1));
}

TEST_F(Esp8266_Tests, Expect_Second_Fail)
{
    {
        InSequence seq;
        SetReadBuffer("---1");
        SetReadBuffer("", false);
    }

    MockEsp8266 esp(_serial.get(), false, false, 100);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    EXPECT_CALL(esp, _Log(StrEq("expecting: 2\n"))).Times(1);
    EXPECT_CALL(esp, _Log(StrEq("expecting: 1\n"))).Times(1);

    EXPECT_TRUE(esp.Expect("1", 1));
    EXPECT_FALSE(esp.Expect("2", 1));
}

TEST_F(Esp8266_Tests, ExpectTimeout_Failure)
{
    {
        InSequence seq;
        SetReadBuffer("---1");
        SetReadBuffer(" ");
    }

    MockEsp8266 esp(_serial.get(), false, false, 100);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    EXPECT_CALL(esp, _Log(HasSubstr("timeout"))).Times(1);
    EXPECT_CALL(esp, _Log(StrEq("expecting: 2\n"))).Times(1);
    EXPECT_CALL(esp, _Log(StrEq("expecting: 1\n"))).Times(1);

    EXPECT_TRUE(esp.Expect("1", 1));
    EXPECT_FALSE(esp.Expect("2", 1));
}

TEST_F(Esp8266_Tests, ExpectInTwoReads_Success)
{
    const char* read1 = "-------1234";
    const char* read2 = "567-------------";
    {
        InSequence seq;
        SetReadBuffer(read1);
        SetReadBuffer(read2);
    }

    MockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());

    EXPECT_TRUE(esp.Expect("1234567", 1));
    EXPECT_EQ(esp.GetDataBytesRead(), (int)(strlen(read1) + strlen(read2)));
    EXPECT_EQ(esp.GetDataBufPos(), (int)strlen(read1) + 3);
    EXPECT_TRUE(strcmp(&esp.GetBuf()[esp.GetDataBufPos()], "-------------") == 0);
}

TEST_F(Esp8266_Tests, ExpectFoundInRemainder_Success)
{
    const char* read1 = "-------1234";
    {
        InSequence seq;
        SetReadBuffer(read1);
    }

    MockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    EXPECT_CALL(esp, _Log(HasSubstr("remainder"))).Times(1);

    EXPECT_TRUE(esp.Expect("123", 1));
    EXPECT_TRUE(esp.Expect("4", 1));
    EXPECT_EQ(esp.GetDataBufPos(), (int)strlen(read1));
}

TEST_F(Esp8266_Tests, ExpectFoundInRemainder2_Success)
{
    const char* read1 = "-------12345";
    const char* read2 = "67....";
    {
        InSequence seq;
        SetReadBuffer(read1);
        SetReadBuffer(read2);
    }

    MockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    EXPECT_CALL(esp, _Log(HasSubstr("remainder"))).Times(1);

    EXPECT_TRUE(esp.Expect("123", 1));
    EXPECT_TRUE(esp.Expect("4", 1));
    EXPECT_EQ(esp.GetDataBufPos(), (int)strlen(read1)-1);
    EXPECT_TRUE(esp.Expect("567", 1));
    EXPECT_EQ(esp.GetDataBytesRead(), (int)(strlen(read1)+strlen(read2)));

}

TEST_F(Esp8266_Tests, ExpectMultipleRemainder_Success)
{
    const char* read1 = "1234";
    const char* read2 = "5678";
    const char* read3 = "0124";
    const char* read4 = "-*-";
    {
        InSequence seq;
        SetReadBuffer(read1);
        SetReadBuffer(read2);
        SetReadBuffer(read3);
        SetReadBuffer(read4);
    }

    MockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    EXPECT_CALL(esp, _Log(HasSubstr("remainder"))).Times(1);

    EXPECT_TRUE(esp.Expect("2", 1));
    EXPECT_TRUE(esp.Expect("3", 1));
    EXPECT_TRUE(esp.Expect("6", 1));
    EXPECT_TRUE(esp.Expect("124", 1));
    EXPECT_EQ(esp.GetDataBytesRead(), (int)(strlen(read1) + strlen(read2) + strlen(read3)));
    EXPECT_TRUE(esp.Expect("*", 1));
    EXPECT_EQ(esp.GetDataBytesRead(), 3);
}

TEST_F(Esp8266_Tests, ExpectRemainderSamePatternAgain_Success)
{
    const char* read1 = "1234";
    const char* read2 = "5678";
    const char* read3 = "0123";
    const char* read4 = "-*-";
    {
        InSequence seq;
        SetReadBuffer(read1);
        SetReadBuffer(read2);
        SetReadBuffer(read3);
        SetReadBuffer(read4);
    }

    MockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    EXPECT_CALL(esp, _Log(HasSubstr("remainder"))).Times(1);

    EXPECT_TRUE(esp.Expect("2", 1));
    EXPECT_TRUE(esp.Expect("3", 1));
    EXPECT_TRUE(esp.Expect("6", 1));
    EXPECT_TRUE(esp.Expect("123", 1));
    EXPECT_EQ(esp.GetDataBufPos(), 12);
    EXPECT_EQ(esp.GetDataBytesRead(), (int)(strlen(read1) + strlen(read2) + strlen(read3)));

    EXPECT_TRUE(esp.Expect("*", 1));
    EXPECT_EQ(esp.GetDataBytesRead(), 3);
    EXPECT_EQ(esp.GetDataBufPos(), 2);
}

TEST_F(Esp8266_Tests, ExpectRemainderMultiPattern2_Success)
{
    const char* read1 = "1234";
    const char* read2 = "5678";
    const char* read3 = "901-";
    {
        InSequence seq;
        SetReadBuffer(read1);
        SetReadBuffer(read2);
        SetReadBuffer(read3);
    }

    MockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    EXPECT_CALL(esp, _Log(HasSubstr("remainder"))).Times(1);

    EXPECT_TRUE(esp.Expect("2", 1));
    EXPECT_TRUE(esp.Expect("3", 1));
    EXPECT_TRUE(esp.Expect("1", 1));
    EXPECT_EQ(esp.GetDataBytesRead(), (int)(strlen(read1) + strlen(read2) + strlen(read3)));
    EXPECT_EQ(esp.GetDataBufPos(), 11);
}

TEST_F(Esp8266_Tests, ExpectSpanMutlipleReads_Success)
{
    const char* read1 = "123";
    const char* read2 = "456";
    const char* read3 = "78----";
    {
        InSequence seq;
        SetReadBuffer(read1);
        SetReadBuffer(read2);
        SetReadBuffer(read3);
    }

    MockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());

    EXPECT_TRUE(esp.Expect("2345678", 1));
    EXPECT_EQ(esp.GetDataBytesRead(), (int)(strlen(read1) + strlen(read2) + strlen(read3)));
    char* buf = esp.GetBuf();
    EXPECT_TRUE(strncpy(&buf[esp.GetDataBufPos()], "----", 4));
}


TEST_F(Esp8266_Tests, ExpectMany_Success)
{
    const char* read1 = "-------1234****56x";
    const char* expect1 = "1234";
    const char* expect2 = "56";
    const char* read2 = "----5";
    const char* read3 = "67....";
    const char* expect3 = "567";
    {
        InSequence seq;
        SetReadBuffer(read1);
        SetReadBuffer(read2);
        SetReadBuffer(read3);
    }

    MockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());

    EXPECT_TRUE(esp.Expect(expect1, 1));
    int pos = MemMem(read1, expect1, (int)strlen(read1), (int)strlen(expect1));
    EXPECT_EQ(esp.GetDataBufPos(), pos + strlen(expect1));
    EXPECT_EQ(esp.GetDataBytesRead(), strlen(read1));
    char* buf = esp.GetBuf();
    int remainder = esp.GetDataBytesRead() - esp.GetDataBufPos();
    EXPECT_EQ(remainder, 7);
    EXPECT_TRUE(strncmp(&buf[esp.GetDataBufPos()], "****56x", 7) == 0);

    EXPECT_TRUE(esp.Expect(expect2, 1));
    buf = esp.GetBuf();

    EXPECT_EQ(esp.GetDataBytesRead(), strlen(read1));
    remainder = esp.GetDataBytesRead() - esp.GetDataBufPos();
    EXPECT_EQ(remainder, 1);
    EXPECT_TRUE(strncmp(&buf[esp.GetDataBufPos()], "x", 1) == 0);


    EXPECT_TRUE(esp.Expect("567", 1));
    EXPECT_EQ(esp.GetDataBytesRead(), strlen(read1) + strlen(read2) + strlen(read3));
    remainder = esp.GetDataBytesRead() - esp.GetDataBufPos();
    EXPECT_EQ(remainder, 4);
    buf = esp.GetBuf();
    EXPECT_TRUE(strncmp(&buf[esp.GetDataBufPos()], "....", 4) == 0);

    
}

