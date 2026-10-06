#ifndef PRODUCTIVITY_TIMER_SESSION_REPOSITORY_H
#define PRODUCTIVITY_TIMER_SESSION_REPOSITORY_H

#include <QString>
#include <QtGlobal>

namespace productivity_timer
{

    enum class Priority : int {
        Leisure   = 0,
        Low       = 1,
        Normal    = 2,
        High      = 3,
        Emergency = 4
    };

    class SessionRepository
    {
    public:
        SessionRepository();
        ~SessionRepository();

        SessionRepository(const SessionRepository&) = delete;
        SessionRepository& operator=(const SessionRepository&) = delete;

        bool initialize();

        // Returns -1 on failure.
        qint64 session_count();

        [[nodiscard]] QString database_path() const;
        [[nodiscard]] QString last_error() const;

    private:
        QString m_connection_name;
        QString m_database_path;
        QString m_last_error;
    };

}

#endif // PRODUCTIVITY_TIMER_SESSION_REPOSITORY_H