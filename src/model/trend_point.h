#ifndef PRODUCTIVITY_TIMER_TREND_POINT_H
#define PRODUCTIVITY_TIMER_TREND_POINT_H

#include <QDate>
#include <QtGlobal>

namespace productivity_timer
{
    struct TrendPoint
    {
        QDate date;

        qint64 focused_ms = 0;
        qint64 weighted_ms = 0;

        int session_count = 0;
    };
}

#endif // PRODUCTIVITY_TIMER_TREND_POINT_H