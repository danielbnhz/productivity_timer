#ifndef PRODUCTIVITY_TIMER_TREND_CONTROLLER_H
#define PRODUCTIVITY_TIMER_TREND_CONTROLLER_H

#include <QList>
#include <QDate>

#include "../model/trend_point.h"
#include "../persistence/session_repository.h"

namespace productivity_timer
{
    class TrendController
    {
    public:
        explicit TrendController(SessionRepository& repository);

        QList<TrendPoint> aggregate_daily(
            const QList<SessionEntry>& sessions,
            const QDate& start_date,
            const QDate& end_date
        ) const;

        bool load_daily_trends(
            const QDate& start_date,
            const QDate& end_date,
            QList<TrendPoint>& result
        );

    private:
        SessionRepository& m_repository;
    };
}

#endif // PRODUCTIVITY_TIMER_TREND_CONTROLLER_HY_TIMER_TREND_CONTROLLER_H