#pragma once
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace testing;

class MockEsp8266 : public Esp8266_Base
{
public:
    MockEsp8266() 
    {
        ON_CALL(*this, WaitBoot).WillByDefault(Return(true));
        ON_CALL(*this, SetMode).WillByDefault(Return(true));
        ON_CALL(*this, ConnectToAP).WillByDefault(Return(true));
        ON_CALL(*this, IsConnectedToAP).WillByDefault(Return(true));
        ON_CALL(*this, SetMux).WillByDefault(Return(true));
        ON_CALL(*this, StartServer).WillByDefault(Return(true));
    }
    virtual ~MockEsp8266() {}
    MOCK_METHOD(bool, SendCommand, (const char* buf, int len), (override));

    MOCK_METHOD(bool, WaitBoot, (), (override));
    MOCK_METHOD(bool, EchoOff, (), (override));
    MOCK_METHOD(void, Reset, (), (override));
    MOCK_METHOD(bool, FactoryRestore, (), (override));
    MOCK_METHOD(bool, SetAutoConnect, (bool), (override));
    MOCK_METHOD(bool, SetStationIpAddress, (const char*), (override));
    MOCK_METHOD(bool, PassthroughMode, (bool), (override));
    MOCK_METHOD(bool, ConnectTCP, (const char* hostname, int port, int timeoutMs, int linkId), (override));
    MOCK_METHOD(bool, ConnectUDP, (const char* hostname, int port, int timeoutMs, int linkId), (override));
    MOCK_METHOD(bool, SendData, (const char* buf, const int size, int timeoutMs, int linkId, bool waitAck), (override));
    MOCK_METHOD(bool, ReceiveData, (char* buf, int& size, int& linkId, int timeoutMs, bool waitForOK), (override));
    MOCK_METHOD(bool, CloseConnection, (int linkId), (override));
    MOCK_METHOD(bool, IsConnectionClosed, (int linkId), (override));
    MOCK_METHOD(void, SetChunkSize, (int chunkSize), (override));
    MOCK_METHOD(bool, SetMux, (bool isMux), (override));
    MOCK_METHOD(bool, GetListOfAps, (char* buf, int buflen), (override));
    
    MOCK_METHOD(bool, StopServer, (), (override));
    MOCK_METHOD(bool, SetMode, (Esp8266::MODE), (override));
    MOCK_METHOD(bool, ConnectToAP, (const char* ssid, const char* password, bool saveToFlash, int timeoutMs), (override));
    MOCK_METHOD(bool, IsConnectedToAP, (), (override));
    MOCK_METHOD(bool, DisconnectFromAP, (), (override));
    MOCK_METHOD(bool, Expect, (const char* expect, int timeoutMs, bool resetPointer), (override));
    MOCK_METHOD(bool, StartServer, (int port, int maxConnections, int timeoutSec), (override));
};

