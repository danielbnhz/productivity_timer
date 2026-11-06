#include "trend_controller.h"

#include <QDateTime>
#include <QTimeZone>
#include <algorithm>

namespace productivity_timer
{
    TrendController::TrendController(SessionRepository& repository)
        : m_repository(repository)
    {
    }

    bool TrendController::load_daily_trends(
    const QDate& start_date,
    const QDate& end_date,
    QList<TrendPoint>& result
)
    {
        result.clear();

        if (!start_date.isValid()
            || !end_date.isValid()
            || end_date < start_date)
        {
            return false;
        }

        const QTimeZone zone = QTimeZone::systemTimeZone();

        const QDateTime range_start(
            start_date,
            QTime(0, 0),
            zone
        );

        const QDateTime range_end(
            end_date.addDays(1),
            QTime(0, 0),
            zone
        );

        QList<SessionEntry> sessions;

        if (!m_repository.load_sessions_for_range(
                range_start,
                range_end,
                sessions))
        {
            return false;
        }

        result = aggregate_daily(
            sessions,
            start_date,
            end_date
        );

        return true;
    }


    QList<TrendPoint> TrendController::aggregate_daily(
        const QList<SessionEntry>& sessions,
        const QDate& start_date,
        const QDate& end_date
    ) const
    {
        QList<TrendPoint> result;

        if (!start_date.isValid()
            || !end_date.isValid()
            || end_date < start_date)
        {
            return result;
        }

        const QTimeZone zone = QTimeZone::systemTimeZone();


        for (QDate date = start_date;
             date <= end_date;
             date = date.addDays(1))
        {
            TrendPoint point;
            point.date = date;

            result.append(point);
        }

        for (const SessionEntry& session : sessions)
        {
            if (!session.started_at.isValid()
                || !session.ended_at.isValid()
                || session.ended_at <= session.started_at
                || session.focused_duration_ms <= 0)
            {
                continue;
            }

            const QDateTime session_start =
                session.started_at.toTimeZone(zone);

            const QDateTime session_end =
                session.ended_at.toTimeZone(zone);

            const qint64 elapsed_ms =
                session.started_at.msecsTo(session.ended_at);

            if (elapsed_ms <= 0)
            {
                continue;
            }

            const QDate first_day = std::max(
                start_date,
                session_start.date()
            );

            const QDate last_day = std::min(
                end_date,
                session_end.date()
            );

            for (QDate date = first_day;
                 date <= last_day;
                 date = date.addDays(1))
            {
                const QDateTime day_start(
                    date, QTime(0, 0), zone
                );

                const QDateTime day_end(
                    date.addDays(1), QTime(0, 0), zone
                );

                const QDateTime overlap_start =
                    std::max(session.started_at, day_start);

                const QDateTime overlap_end =
                    std::min(session.ended_at, day_end);

                const qint64 overlap_ms =
                    overlap_start.msecsTo(overlap_end);

                if (overlap_ms <= 0)
                {
                    continue;
                }

                const qint64 focused_share =
                    session.focused_duration_ms
                    * overlap_ms / elapsed_ms;

                const int index =
                    date.daysTo(start_date) * -1;

                if (index < 0 || index >= result.size())
                {
                    continue;
                }

                TrendPoint& point = result[index];

                point.focused_ms += focused_share;

                point.weighted_ms +=
                    focused_share * session.priority_snapshot;

                point.session_count++;
            }
        }

        return result;
    }
}