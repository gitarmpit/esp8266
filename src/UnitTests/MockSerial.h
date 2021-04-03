#pragma once
#include "../common/Serial.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace testing;

class MockSerial : public Serial
{
public:
    MockSerial()
    {
        ON_CALL(*this, Read(_, _, _)).WillByDefault(Return(false));
        ON_CALL(*this, Send).WillByDefault(Return(true));
        ON_CALL(*this, SetTimeout).WillByDefault(Return(true));
        ON_CALL(*this, WaitRead).WillByDefault(Return(0));

    }
    MOCK_METHOD(bool, Initialize, (int baudRate), (override));
    MOCK_METHOD(uint32_t, WaitRead, (int timeoutMs), (override));
    MOCK_METHOD(bool, Read, (char* buf, int& bytesReceived, int timeoutMs), (override));
    MOCK_METHOD(bool, Send, (const char* buf, int size), (override));
    MOCK_METHOD(bool, SetTimeout, (int timeoutMs), (override));
};

