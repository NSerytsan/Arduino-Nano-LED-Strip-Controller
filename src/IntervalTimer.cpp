#include "IntervalTimer.h"

IntervalTimer::IntervalTimer(unsigned long interval) 
    : interval_(interval) {}

bool IntervalTimer::ready()
{
    unsigned long now = millis();
    if (now - last_ >= interval_)
    {
        last_ = now;
        return true;
    }

    return false;
}

void IntervalTimer::setInterval(unsigned long interval)
{
    interval_ = interval;
}

unsigned long IntervalTimer::get_last()
{
    return last_;
}