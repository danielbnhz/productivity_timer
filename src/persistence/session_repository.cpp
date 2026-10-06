#include "session_repository.h"

#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QStringList>
#include <QUuid>
#include <QVariant>

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

}