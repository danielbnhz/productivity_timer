#ifndef PRODUCTIVITY_TIMER_TIME_RANGE_H
#define PRODUCTIVITY_TIMER_TIME_RANGE_H

namespace productivity_timer
{

    enum class TimeRange : int {
        Day = 0,
        Week,
        Month,
        Year,
        All
    };

}

#endif // PRODUCTIVITY_TIMER_TIME_RANGE_H