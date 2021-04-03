#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../common/Serial.h"
#include "../common/PersistedSettings.h"
#include "../common/Esp8266.h"
#include "../common/memmem.h"
#include "MockSerial.h"

using namespace testing;

class Esp8266_Tests : public ::testing::TestWithParam<const char*>
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


class HybridMockEsp8266 : public Esp8266
{
public:

    HybridMockEsp8266(Serial* serial, bool autoReconnect, bool echoOff, int timerVal) : 
        Esp8266(serial, autoReconnect, echoOff)
    {
        _timer.SetReturnValue(timerVal);
        ON_CALL(*this, _Log).WillByDefault(Return());
    }
    
    virtual ~HybridMockEsp8266() {}

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
    bool SendChunk(const char* buf, const int size, int timeoutMs, int linkId, bool waitAck)
    {
        return Esp8266::SendChunk(buf, size, timeoutMs, linkId, waitAck);
    }
    bool ParseIPD(int& linkId, int& bytesToRead)
    {
        return Esp8266::ParseIPD(linkId, bytesToRead);
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
    HybridMockEsp8266 esp(_serial.get(), false, false, 1000000);
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
    HybridMockEsp8266 esp(_serial.get(), false, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    bool rc = esp.WaitBoot();
    EXPECT_TRUE(rc);
}

TEST_F(Esp8266_Tests, Reset_Success)
{
    std::string command = AT_RST;
    EXPECT_CALL(*_serial.get(), Send(StrEq(command.c_str()), 0)).WillOnce(Return(true));
    HybridMockEsp8266 esp(_serial.get(), false, false, 1000000);
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

    HybridMockEsp8266 esp(_serial.get(), false, false, 1000000);
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

    HybridMockEsp8266 esp(_serial.get(), false, false, 1000000);
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

    HybridMockEsp8266 esp(_serial.get(), false, false, 1000000);
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

    HybridMockEsp8266 esp(_serial.get(), true, false, 1000000);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    bool rc = esp.ConnectToAP(NULL, NULL, true, 100);
    EXPECT_TRUE(rc);
}

TEST_F(Esp8266_Tests, ConnectToAP_NoCreds_NoAutoConnect_Failure)
{
    HybridMockEsp8266 esp(_serial.get(), false, false, 1000000);
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
    HybridMockEsp8266 esp(_serial.get(), false, false, 1000000);
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
    HybridMockEsp8266 esp(_serial.get(), false, false, 1000000);
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
    HybridMockEsp8266 esp(_serial.get(), false, false, 1000000);
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
    HybridMockEsp8266 esp(_serial.get(), false, false, 1000000);
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
    HybridMockEsp8266 esp(_serial.get(), false, false, 1000000);
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
    HybridMockEsp8266 esp(_serial.get(), false, false, 1000000);
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
    HybridMockEsp8266 esp(_serial.get(), false, false, 1000000);
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
    HybridMockEsp8266 esp(_serial.get(), false, false, 1000000);
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
    HybridMockEsp8266 esp(_serial.get(), false, false, 1000000);
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
    HybridMockEsp8266 esp(_serial.get(), false, false, 1000000);
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
    HybridMockEsp8266 esp(_serial.get(), false, false, 1000000);
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
    HybridMockEsp8266 esp(_serial.get(), false, false, 1000000);
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
    HybridMockEsp8266 esp(_serial.get(), false, false, 1000000);
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
    HybridMockEsp8266 esp(_serial.get(), false, false, 1000000);
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
    HybridMockEsp8266 esp(_serial.get(), false, false, 1000000);
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
    HybridMockEsp8266 esp(_serial.get(), false, false, 1000000);
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
    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
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
    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
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

    HybridMockEsp8266 esp(_serial.get(), false, false, 100);
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

    HybridMockEsp8266 esp(_serial.get(), false, false, 100);
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

    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
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

    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
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

    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
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

    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
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

    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
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

    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
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

    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
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

    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
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

TEST_F(Esp8266_Tests, SendChunkNoLinkWaitAck_Success)
{
    const char* buf = "123";

    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq("AT+CIPSEND=3\r\n"), 0)).WillOnce(Return(true));
        SetReadBuffer(AT_OK"> ");
        EXPECT_CALL(*_serial.get(), Send(buf, 3)).WillOnce(Return(true));
        SetReadBuffer(SEND_OK);
    }

    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    EXPECT_TRUE(esp.SendChunk(buf, 3, 100, -1, true));
}

TEST_F(Esp8266_Tests, SendChunkNoWaitAck_Success)
{
    const char* buf = "123";

    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq("AT+CIPSEND=2,3\r\n"), 0)).WillOnce(Return(true));
        SetReadBuffer(AT_OK"> ");
        EXPECT_CALL(*_serial.get(), Send(buf, 3)).WillOnce(Return(true));
    }

    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    EXPECT_TRUE(esp.SendChunk(buf, 3, 100, 2, false));
}

TEST_F(Esp8266_Tests, SendData_Success)
{
    const char* buf = "1234567";

    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq("AT+CIPSEND=2,3\r\n"), 0)).WillOnce(Return(true));
        SetReadBuffer(AT_OK"> ");
        EXPECT_CALL(*_serial.get(), Send(buf, 3)).WillOnce(Return(true));

        EXPECT_CALL(*_serial.get(), Send(StrEq("AT+CIPSEND=2,3\r\n"), 0)).WillOnce(Return(true));
        SetReadBuffer(AT_OK"> ");
        EXPECT_CALL(*_serial.get(), Send(&buf[3], 3)).WillOnce(Return(true));

        EXPECT_CALL(*_serial.get(), Send(StrEq("AT+CIPSEND=2,1\r\n"), 0)).WillOnce(Return(true));
        SetReadBuffer(AT_OK"> ");
        EXPECT_CALL(*_serial.get(), Send(&buf[6], 1)).WillOnce(Return(true));
    }

    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    esp.SetChunkSize(3);
    EXPECT_TRUE(esp.SendData(buf, 7, 100, 2, false));
}


TEST_F(Esp8266_Tests, SendDataZeroSize_Success)
{
    const char* buf = "";
    EXPECT_CALL(*_serial.get(), Send).Times(0);
    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    esp.SetChunkSize(3);
    EXPECT_TRUE(esp.SendData(buf, 0, 100, 2, false));
}

TEST_F(Esp8266_Tests, IsConnectionClosed_Failure)
{
    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_FALSE(esp.IsConnectionClosed(0));
}

TEST_F(Esp8266_Tests, IsConnectionClosed_Success)
{
    SetReadBuffer("-----*--1,CLOSED\r\n--123--");

    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    ASSERT_TRUE(esp.Expect("*"));
    EXPECT_TRUE(esp.IsConnectionClosed(1));
    char* buf = esp.GetBuf();
    EXPECT_STREQ(&buf[esp.GetDataBufPos()], "--123--");
}

TEST_F(Esp8266_Tests, ParseIPD_Success)
{
    SetReadBuffer("---*22,36789:GET /");

    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    ASSERT_TRUE(esp.Expect("*"));
    int linkId, nBytes;
    ASSERT_TRUE(esp.ParseIPD(linkId, nBytes));
    EXPECT_EQ(linkId, 22);
    EXPECT_EQ(nBytes, 36789);
    char* buf = esp.GetBuf();
    EXPECT_STREQ(&buf[esp.GetDataBufPos()], "GET /");
}
TEST_F(Esp8266_Tests, ParseIPD_NoLinkId_Success)
{
    SetReadBuffer("---*36789:GET /");

    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    ASSERT_TRUE(esp.Expect("*"));
    int linkId, nBytes;
    ASSERT_TRUE(esp.ParseIPD(linkId, nBytes));
    EXPECT_EQ(linkId, -1);
    EXPECT_EQ(nBytes, 36789);
    char* buf = esp.GetBuf();
    EXPECT_STREQ(&buf[esp.GetDataBufPos()], "GET /");
}


INSTANTIATE_TEST_SUITE_P(
    ParseID_Test,
    Esp8266_Tests,
    ::testing::Values(
        "---*22,36789,,:GET /",
        "---*2236789,:GET /",
        "---*,2:GET /",
        "---*2,:GET /",
        "---*<,2:GET /",
        "---*33,?:GET /",
        "---*22,36789",
        "---*22,36,789:"
    ));

TEST_P(Esp8266_Tests, ParseIPD_Fail)
{
    SetReadBuffer(GetParam());
    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    ASSERT_TRUE(esp.Expect("*"));
    int linkId, nBytes;
    ASSERT_FALSE(esp.ParseIPD(linkId, nBytes));
}

TEST_F(Esp8266_Tests, ReadData_Success)
{
    std::string remainder = "--remainder--";
    std::string read = "garbage---0,CONNECT\r\n+IPD,0,4:data" + remainder;
    SetReadBuffer(read.c_str());
    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    char buf[32] = { 0 };
    int linkId, len;
    EXPECT_TRUE(esp.ReceiveData(buf, len, linkId, 100, false));
    EXPECT_EQ(linkId, 0);
    EXPECT_EQ(len, 4);
    EXPECT_STREQ((char*)buf, "data");
    char* _buf = esp.GetBuf();
    int pos = esp.GetDataBufPos();
    int bytesRead = esp.GetDataBytesRead();
    EXPECT_EQ(bytesRead - pos, remainder.size());
    EXPECT_TRUE(strncmp(&_buf[pos], remainder.c_str(), remainder.size()) == 0);
}

TEST_F(Esp8266_Tests, ReadData_MultiReads_Success)
{
    const char* read1 = "garbage---0,CONNECT\r\n+IP";
    const char* read2 = "D,0,";
    const char* read3 = "4:da";
    const char* read4 = "ta--remainder--";
    {
        InSequence seq;
        SetReadBuffer(read1);
        SetReadBuffer(read2);
        SetReadBuffer(read3);
        SetReadBuffer(read4);
        SetReadBuffer(AT_OK);
    }

    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    char buf[32] = { 0 };
    int linkId, len;
    EXPECT_TRUE(esp.ReceiveData(buf, len, linkId, 100, true));
    EXPECT_EQ(linkId, 0);
    EXPECT_EQ(len, 4);
    EXPECT_STREQ((char*)buf, "data");
}

TEST_F(Esp8266_Tests, ReadDataNoIPD_Fail)
{
    SetReadBuffer("garbage---0,CONNECT\r\n+");
    HybridMockEsp8266 esp(_serial.get(), false, false, 200);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    char buf[32] = { 0 };
    int linkId, len;
    EXPECT_FALSE(esp.ReceiveData(buf, len, linkId, 100, true));
}

TEST_F(Esp8266_Tests, ReadDataNoColon_Fail)
{
    {
        InSequence seq;
        SetReadBuffer("garbage---0,CONNECT\r\n+IPD,0,4");
        SetReadBuffer("", false);
    }
    HybridMockEsp8266 esp(_serial.get(), false, false, 200);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    char buf[32] = { 0 };
    int linkId, len;
    EXPECT_FALSE(esp.ReceiveData(buf, len, linkId, 100, true));
}

TEST_F(Esp8266_Tests, ReadDataIpdParseError_Fail)
{
    SetReadBuffer("garbage---0,CONNECT\r\n+IPD,,4:");
    HybridMockEsp8266 esp(_serial.get(), false, false, 200);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    char buf[32] = { 0 };
    int linkId, len;
    EXPECT_FALSE(esp.ReceiveData(buf, len, linkId, 100, true));
}


TEST_F(Esp8266_Tests, GetListOfAps_Success)
{   
    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq("AT+CWLAP\r\n"), 0)).WillOnce(Return(true));
        SetReadBuffer("aplist\r\nOK\r\n");
    }
    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    char buf[7];
    EXPECT_TRUE(esp.GetListOfAps(buf, 7));
    EXPECT_STREQ(buf, "aplist");
}

TEST_F(Esp8266_Tests, GetListOfAps_BufferTooSmall_Failure)
{
    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq("AT+CWLAP\r\n"), 0)).WillOnce(Return(true));
        SetReadBuffer("aplist\r\nOK\r\n");
    }
    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    char buf[6];
    EXPECT_FALSE(esp.GetListOfAps(buf, 6));
}

TEST_F(Esp8266_Tests, GetListOfAps_Parse_Success)
{
    std::string aplist = 
R"(
+CWLAP:(0,"HP-Print-C4-Photosmart 7520",-85,"6c:c2:17:80:00:c4",1)
+CWLAP:(3,"ZyXEL8DD0A0",-78,"54:83:3a:8d:d0:a0",1)
+CWLAP:(3,"VETTE 94-2G",-81,"ac:ec:80:b9:b4:80",1)
+CWLAP:(3,"NTGR_VMB_1462061258",-96,"cc:40:d0:0e:17:c6",1)
+CWLAP:(3,"Casey House 24",-66,"34:6b:46:43:d3:ea",6)
+CWLAP:(3,"MySpectrumWiFi4C-2G",-91,"ec:a9:40:3d:c1:4d",6)
+CWLAP:(3,"MySpectrumWiFi88-2G",-93,"38:35:fb:7b:db:8e",6)
+CWLAP:(3,"HOMEAV",-91,"7c:d9:5c:94:ca:9a",6)
+CWLAP:(4,"Nash_1",-57,"d8:0d:17:bc:50:ce",9)
+CWLAP:(4,"Parikshak--Pool",-88,"d4:6a:91:3c:b9:2e",11)
+CWLAP:(3,"TPLINK24GHZ",-87,"84:16:f9:eb:65:db",11)
)";
    std::string read = aplist + AT_OK;
    {
        InSequence seq;
        EXPECT_CALL(*_serial.get(), Send(StrEq("AT+CWLAP\r\n"), 0)).WillOnce(Return(true));
        SetReadBuffer(read.c_str());
    }
    HybridMockEsp8266 esp(_serial.get(), false, false, 0);
    EXPECT_CALL(esp, _Log).WillRepeatedly(Return());
    char buf[1024];
    EXPECT_TRUE(esp.GetListOfAps(buf, 1024));
    ASSERT_STREQ(buf, aplist.c_str());
}
