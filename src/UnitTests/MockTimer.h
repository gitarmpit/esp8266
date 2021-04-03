#pragma once

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
