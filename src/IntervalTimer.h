#pragma once
#include <Arduino.h>

class IntervalTimer
{
private:
    unsigned long interval_;
    unsigned long last_ = 0;
public:
    explicit IntervalTimer(unsigned long interval);
    bool ready();
    void setInterval(unsigned long interval);
    unsigned long get_last();
};

