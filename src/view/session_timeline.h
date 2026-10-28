#ifndef PRODUCTIVITY_TIMER_SESSION_TIMELINE_H
#define PRODUCTIVITY_TIMER_SESSION_TIMELINE_H

#include <QWidget>
#include <QDate>
#include <QList>

#include "../persistence/session_repository.h"

namespace productivity_timer
{
    struct SessionEntry;

    class SessionTimeline : public QWidget
    {
        Q_OBJECT

    public:
        explicit SessionTimeline(QWidget* parent = nullptr);

        void set_day(const QDate& day);
        void set_sessions(const QList<SessionEntry>& sessions);

    protected:
        void paintEvent(QPaintEvent* event) override;

    private:
        QDate m_day = QDate::currentDate();
        QList<SessionEntry> m_sessions;
    };

}

#endif