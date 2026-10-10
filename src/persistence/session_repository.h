#ifndef PRODUCTIVITY_TIMER_SESSION_REPOSITORY_H
#define PRODUCTIVITY_TIMER_SESSION_REPOSITORY_H

#include <QString>
#include <QtGlobal>
#include <QList>
namespace productivity_timer
{

    enum class Priority : int {
        Leisure   = 0,
        Low       = 1,
        Normal    = 2,
        High      = 3,
        Emergency = 4
    };

    struct TaskArchetype
    {
        qint64 id;
        QString name;
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

        bool load_active_archetypes(QList<TaskArchetype>& result);

        qint64 create_archetype(const QString& name,
                       Priority default_priority);

        qint64 create_task(qint64 archetype_id,
                   const QString& title);


    private:
        QString m_connection_name;
        QString m_database_path;
        QString m_last_error;


        bool database_is_ready();
    };

}

#endif // PRODUCTIVITY_TIMER_SESSION_REPOSITORY_H