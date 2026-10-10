#include "session_repository.h"

#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QStringList>
#include <QUuid>
#include <QVariant>
#include <QDateTime>

namespace productivity_timer
{

SessionRepository::SessionRepository()
    : m_connection_name(
          "sessions-" +
          QUuid::createUuid().toString(QUuid::WithoutBraces))
{
}

SessionRepository::~SessionRepository()
{
    if (QSqlDatabase::contains(m_connection_name)) {
        {
            auto db =
                QSqlDatabase::database(m_connection_name, false);
            db.close();
        }

        QSqlDatabase::removeDatabase(m_connection_name);
    }
}

bool SessionRepository::initialize()
{
    m_last_error.clear();

    const QString directory =
        QStandardPaths::writableLocation(
            QStandardPaths::AppLocalDataLocation);

    if (directory.isEmpty()) {
        m_last_error =
            "Could not determine the application data directory.";
        return false;
    }

    if (!QDir().mkpath(directory)) {
        m_last_error =
            "Could not create application data directory: "
            + directory;
        return false;
    }

     m_database_path =
        QDir(directory).filePath("productivity-v1.sqlite");

    auto db = QSqlDatabase::contains(m_connection_name)
        ? QSqlDatabase::database(m_connection_name, false)
        : QSqlDatabase::addDatabase(
              "QSQLITE", m_connection_name);

    db.setDatabaseName(m_database_path);

    if (!db.open()) {
        m_last_error = db.lastError().text();
        return false;
    }

     {
        QSqlQuery query(db);

        if (!query.exec("PRAGMA foreign_keys = ON")) {
            m_last_error = query.lastError().text();
            return false;
        }

        if (!query.exec("PRAGMA foreign_keys")) {
            m_last_error = query.lastError().text();
            return false;
        }

        if (!query.next() || query.value(0).toInt() != 1) {
            m_last_error =
                "SQLite foreign-key enforcement is unavailable.";
            return false;
        }
    }

    const QStringList statements {
        QStringLiteral(R"SQL(
            CREATE TABLE IF NOT EXISTS task_archetypes (
                id INTEGER PRIMARY KEY,
                name TEXT NOT NULL UNIQUE,
                default_priority INTEGER NOT NULL DEFAULT 1
                    CHECK (default_priority BETWEEN 0 AND 4),
                archived INTEGER NOT NULL DEFAULT 0
                    CHECK (archived IN (0, 1))
            )
        )SQL"),

        QStringLiteral(R"SQL(
            CREATE TABLE IF NOT EXISTS tasks (
                id INTEGER PRIMARY KEY,
                archetype_id INTEGER NOT NULL,
                title TEXT NOT NULL,
                priority_override INTEGER
                    CHECK (
                        priority_override IS NULL
                        OR priority_override BETWEEN 0 AND 4
                    ),
                scheduled_at_utc TEXT,
                status TEXT NOT NULL DEFAULT 'active'
                    CHECK (
                        status IN (
                            'active',
                            'completed',
                            'archived'
                        )
                    ),
                created_at_utc TEXT NOT NULL,
                completed_at_utc TEXT,


                FOREIGN KEY (archetype_id)
                    REFERENCES task_archetypes(id)
                    ON DELETE RESTRICT
            )
        )SQL"),

        QStringLiteral(R"SQL(
            CREATE TABLE IF NOT EXISTS sessions (
                id INTEGER PRIMARY KEY,
                task_id INTEGER NOT NULL,
                started_at_utc TEXT NOT NULL,
                ended_at_utc TEXT NOT NULL,
                planned_duration_ms INTEGER NOT NULL
                    CHECK (planned_duration_ms > 0),
                focused_duration_ms INTEGER NOT NULL
                    CHECK (focused_duration_ms >= 0),
                effective_priority_snapshot INTEGER NOT NULL
                    CHECK (
                        effective_priority_snapshot
                        BETWEEN 0 AND 4
                    ),

                FOREIGN KEY (task_id)
                    REFERENCES tasks(id)
                    ON DELETE RESTRICT
            )
        )SQL")
    };

    if (!db.transaction()) {
        m_last_error = db.lastError().text();
        return false;
    }

    for (const QString& sql : statements) {
        QSqlQuery query(db);

        if (!query.exec(sql)) {
            m_last_error = query.lastError().text();
            query.finish();
            db.rollback();
            return false;
        }
    }

    if (!db.commit()) {
        m_last_error = db.lastError().text();
        db.rollback();
        return false;
    }

    return true;
}
    bool SessionRepository::ensure_starter_archetypes()
{
    m_last_error.clear();

    if (!database_is_ready()) {
        return false;
    }

    const auto db =
        QSqlDatabase::database(m_connection_name, false);

    QSqlQuery query(db);

    if (!query.exec(R"SQL(
        INSERT INTO task_archetypes (
            name,
            default_priority
        )
        VALUES
            ('Programming', 2),
            ('Music', 1),
            ('Exercise', 2)
        ON CONFLICT(name) DO NOTHING
    )SQL")) {
        m_last_error = query.lastError().text();
        return false;
    }

    return true;
}
qint64 SessionRepository::session_count()
{
    m_last_error.clear();

    if (!QSqlDatabase::contains(m_connection_name)) {
        m_last_error = "Database has not been initialized.";
        return -1;
    }

    auto db =
        QSqlDatabase::database(m_connection_name, false);

    if (!db.isOpen()) {
        m_last_error = "Database connection is not open.";
        return -1;
    }

    QSqlQuery query(db);

    if (!query.exec("SELECT COUNT(*) FROM sessions")) {
        m_last_error = query.lastError().text();
        return -1;
    }

    if (!query.next()) {
        m_last_error =
            "Session count query returned no result.";
        return -1;
    }

    return query.value(0).toLongLong();
}

QString SessionRepository::database_path() const
{
    return m_database_path;
}

QString SessionRepository::last_error() const
{
    return m_last_error;
}
    bool SessionRepository::database_is_ready()

{
    if (!QSqlDatabase::contains(m_connection_name)) {
        m_last_error = "Database has not been initialized.";
        return false;
    }

    const auto db =
        QSqlDatabase::database(m_connection_name, false);

    if (!db.isOpen()) {
        m_last_error = "Database connection is not open.";
        return false;
    }

    return true;
}
    bool SessionRepository::load_active_archetypes(
        QList<TaskArchetype>& result)
{
    m_last_error.clear();
    result.clear();

    if (!database_is_ready()) {
        return false;
    }

    const auto db =
        QSqlDatabase::database(m_connection_name, false);

    QSqlQuery query(db);

    if (!query.exec(
            "SELECT id, name "
            "FROM task_archetypes "
            "WHERE archived = 0 "
            "ORDER BY name COLLATE NOCASE, id")) {
        m_last_error = query.lastError().text();
        return false;
            }

    while (query.next()) {
        result.append(TaskArchetype{
            query.value(0).toLongLong(),
            query.value(1).toString()
        });
    }

    if (query.lastError().isValid()) {
        m_last_error = query.lastError().text();
        result.clear();
        return false;
    }

    return true;
}
    qint64 SessionRepository::create_archetype(
        const QString& name,
        Priority default_priority)
{
    m_last_error.clear();

    const QString clean_name = name.trimmed();
    const int priority = static_cast<int>(default_priority);

    if (clean_name.isEmpty()) {
        m_last_error = "Archetype name cannot be empty.";
        return -1;
    }

    if (priority < 0 || priority > 4) {
        m_last_error = "Priority must be between 0 and 4.";
        return -1;
    }

    if (!database_is_ready()) {
        return -1;
    }

    const auto db =
        QSqlDatabase::database(m_connection_name, false);

    QSqlQuery query(db);

    if (!query.prepare(
            "INSERT INTO task_archetypes "
            "(name, default_priority) "
            "VALUES (:name, :priority)")) {
        m_last_error = query.lastError().text();
        return -1;
            }

    query.bindValue(":name", clean_name);
    query.bindValue(":priority", priority);

    if (!query.exec()) {
        m_last_error = query.lastError().text();
        return -1;
    }

    return query.lastInsertId().toLongLong();
}

    qint64 SessionRepository::create_task(
    qint64 archetype_id,
    const QString& title)
{
    m_last_error.clear();

    const QString clean_title = title.trimmed();

    if (clean_title.isEmpty()) {
        m_last_error = "Task title cannot be empty.";
        return -1;
    }

    if (!database_is_ready()) {
        return -1;
    }

    const auto db =
        QSqlDatabase::database(m_connection_name, false);

    QSqlQuery query(db);

    if (!query.prepare(R"SQL(
        INSERT INTO tasks (
            archetype_id,
            title,
            created_at_utc
        )
        SELECT id, :title, :created_at
        FROM task_archetypes
        WHERE id = :archetype_id
          AND archived = 0
    )SQL")) {
        m_last_error = query.lastError().text();
        return -1;
    }

    query.bindValue(":archetype_id", archetype_id);
    query.bindValue(":title", clean_title);
    query.bindValue(
        ":created_at",
        QDateTime::currentDateTimeUtc().toString(
            Qt::ISODateWithMs));

    if (!query.exec()) {
        m_last_error = query.lastError().text();
        return -1;
    }

    if (query.numRowsAffected() != 1) {
        m_last_error =
            "Select an existing, nonarchived super task.";
        return -1;
    }

    return query.lastInsertId().toLongLong();
}
    bool SessionRepository::task_priority(
    qint64 task_id,
    int& priority)
{
    m_last_error.clear();

    if (!database_is_ready()) {
        return false;
    }

    const auto db =
        QSqlDatabase::database(m_connection_name, false);

    QSqlQuery query(db);

    if (!query.prepare(R"SQL(
        SELECT COALESCE(
            t.priority_override,
            a.default_priority
        )
        FROM tasks AS t
        JOIN task_archetypes AS a
            ON a.id = t.archetype_id
        WHERE t.id = :task_id
          AND t.status = 'active'
          AND a.archived = 0
    )SQL")) {
        m_last_error = query.lastError().text();
        return false;
    }

    query.bindValue(":task_id", task_id);

    if (!query.exec()) {
        m_last_error = query.lastError().text();
        return false;
    }

    if (!query.next()) {
        m_last_error =
            "The task is unavailable or is no longer active.";
        return false;
    }

    priority = query.value(0).toInt();

    if (priority < 0 || priority > 4) {
        m_last_error = "The task has an invalid priority.";
        return false;
    }

    return true;
}
    bool SessionRepository::complete_task_session(
    qint64 task_id,
    const QDateTime& started_at,
    const QDateTime& ended_at,
    qint64 planned_duration_ms,
    qint64 focused_duration_ms,
    int priority_snapshot)
{
    m_last_error.clear();

    if (!started_at.isValid()
        || !ended_at.isValid()
        || planned_duration_ms <= 0
        || focused_duration_ms < 0
        || focused_duration_ms > planned_duration_ms
        || priority_snapshot < 0
        || priority_snapshot > 4) {
        m_last_error = "Invalid session data.";
        return false;
    }

    if (!database_is_ready()) {
        return false;
    }

    auto db =
        QSqlDatabase::database(m_connection_name, false);

    if (!db.transaction()) {
        m_last_error = db.lastError().text();
        return false;
    }

    const QString start_text =
        started_at.toUTC().toString(Qt::ISODateWithMs);

    const QString end_text =
        ended_at.toUTC().toString(Qt::ISODateWithMs);

    const auto fail = [&db, this](const QString& error) {
        m_last_error = error;

        if (!db.rollback()) {
            m_last_error +=
                "\nRollback failed: " + db.lastError().text();
        }

        return false;
    };

    {
        QSqlQuery query(db);

        if (!query.prepare(R"SQL(
            UPDATE tasks
            SET status = 'completed',
                completed_at_utc = :ended_at
            WHERE id = :task_id
              AND status = 'active'
        )SQL")) {
            return fail(query.lastError().text());
        }

        query.bindValue(":task_id", task_id);
        query.bindValue(":ended_at", end_text);

        if (!query.exec()) {
            return fail(query.lastError().text());
        }

        if (query.numRowsAffected() != 1) {
            return fail(
                "Task is missing or is already completed.");
        }
    }

    {
        QSqlQuery query(db);

        if (!query.prepare(R"SQL(
            INSERT INTO sessions (
                task_id,
                started_at_utc,
                ended_at_utc,
                planned_duration_ms,
                focused_duration_ms,
                effective_priority_snapshot
            )
            VALUES (
                :task_id,
                :started_at,
                :ended_at,
                :planned_ms,
                :focused_ms,
                :priority
            )
        )SQL")) {
            return fail(query.lastError().text());
        }

        query.bindValue(":task_id", task_id);
        query.bindValue(":started_at", start_text);
        query.bindValue(":ended_at", end_text);
        query.bindValue(":planned_ms", planned_duration_ms);
        query.bindValue(":focused_ms", focused_duration_ms);
        query.bindValue(":priority", priority_snapshot);

        if (!query.exec()) {
            return fail(query.lastError().text());
        }
    }

    if (!db.commit()) {
        return fail(db.lastError().text());
    }

    return true;
}
}