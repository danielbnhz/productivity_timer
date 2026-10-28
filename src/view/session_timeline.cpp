
#include <QDateTime>

#include "session_timeline.h"

#include <QPainter>
#include <QPaintEvent>
#include <QDateTime>
#include <QTime>
#include <QFontMetrics>
#include <algorithm>
#include <QTimeZone>

namespace productivity_timer
{

SessionTimeline::SessionTimeline(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(500, 650);
}

void SessionTimeline::set_day(const QDate& day)
{
    if (!day.isValid()) {
        return;
    }

    m_day = day;
    update();
}

void SessionTimeline::set_sessions(
    const QList<SessionEntry>& sessions)
{
    m_sessions = sessions;
    update();
}

void SessionTimeline::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int left_margin = 65;
    const int right_margin = 15;
    const int top_margin = 20;
    const int bottom_margin = 20;

    const int timeline_height =
        height() - top_margin - bottom_margin;

    if (timeline_height <= 0) {
        return;
    }

    const int timeline_width =
        width() - left_margin - right_margin;

    const qreal hour_height = timeline_height / 24.0;

    painter.drawText(
        left_margin,
        15,
        m_day.toString("ddd, MMM d yyyy"));

    // Draw the 24-hour grid.
    for (int hour = 0; hour <= 24; ++hour) {
        const int y = top_margin
            + static_cast<int>(hour * hour_height);

        painter.setPen(QPen(QColor(100, 100, 100, 100), 1));
        painter.drawLine(left_margin, y,
                         left_margin + timeline_width, y);

        if (hour < 24) {
            painter.setPen(palette().text().color());
            painter.drawText(
                5, y + 5,
                QString("%1:00").arg(hour, 2, 10, QChar('0')));
        }
    }

    const QDateTime day_start(
        m_day, QTime(0, 0), QTimeZone::systemTimeZone());

    const QDateTime day_end(
        m_day.addDays(1), QTime(0, 0),
        QTimeZone::systemTimeZone());

    if (!day_start.isValid() || !day_end.isValid()) {
        return;
    }

    for (const SessionEntry& session : m_sessions) {
        if (!session.started_at.isValid()
            || !session.ended_at.isValid()) {
            continue;
        }

        const QDateTime clipped_start =
            std::max(session.started_at.toLocalTime(), day_start);

        const QDateTime clipped_end =
            std::min(session.ended_at.toLocalTime(), day_end);

        if (clipped_start >= clipped_end) {
            continue;
        }

        const qint64 start_ms =
            day_start.msecsTo(clipped_start);

        const qint64 end_ms =
            day_start.msecsTo(clipped_end);

        const qreal start_fraction =
            static_cast<qreal>(start_ms)
            / day_start.msecsTo(day_end);

        const qreal end_fraction =
            static_cast<qreal>(end_ms)
            / day_start.msecsTo(day_end);

        const int y = top_margin
            + static_cast<int>(start_fraction * timeline_height);

        const int bottom_y = top_margin
            + static_cast<int>(end_fraction * timeline_height);

        const int block_height = std::max(3, bottom_y - y);

        const QRect block(
            left_margin + 5, y,
            std::max(1, timeline_width - 10), block_height);

        painter.fillRect(block, QColor(230, 140, 50, 180));
        painter.setPen(QColor(255, 255, 255));
        painter.drawRect(block);

        if (block_height >= 16) {
            painter.drawText(
                block.adjusted(5, 0, -5, 0),
                Qt::AlignVCenter | Qt::AlignLeft,
                session.title);
        }
    }
}

} // namespace productivity_timer
