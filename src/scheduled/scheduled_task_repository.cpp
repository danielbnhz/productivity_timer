//
// Created by super on 11/1/2026.
//

#include "scheduled_task_repository.h"

#include "scheduled_task_repository.h"

#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>
#include <QVariant>

ScheduledTaskRepository::ScheduledTaskRepository(
    const QString& database_path)
{
    m_connection_name =
        "scheduled_tasks_" +
        QUuid::createUuid().toString(QUuid::WithoutBraces);

    m_database = QSqlDatabase::addDatabase(
        "QSQLITE",
        m_connection_name
    );

    m_database.setDatabaseName(database_path);

    if (!m_database.open())
    {
        m_last_error = m_database.lastError().text();
    }
}

ScheduledTaskRepository::~ScheduledTaskRepository()
{
    if (m_database.isOpen())
    {
        m_database.close();
    }

    m_database = QSqlDatabase();

    QSqlDatabase::removeDatabase(m_connection_name);
}

bool ScheduledTaskRepository::initialize()
{
    if (!m_database.isOpen())
    {
        m_last_error = "Scheduled database connection is not open.";
        return false;
    }

    QSqlQuery pragma(m_database);

    if (!pragma.exec("PRAGMA foreign_keys = ON"))
    {
        m_last_error = pragma.lastError().text();
        return false;
    }

    QSqlQuery query(m_database);

    const QString sql = R"(
        CREATE TABLE IF NOT EXISTS scheduled_tasks (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            archetype_id INTEGER NOT NULL,
            title TEXT NOT NULL,
            recurring INTEGER NOT NULL DEFAULT 0
                CHECK (recurring IN (0, 1)),
            FOREIGN KEY (archetype_id)
                REFERENCES task_archetypes(id)
                ON DELETE RESTRICT
        )
    )";

    if (!query.exec(sql))
    {
        m_last_error = query.lastError().text();
        return false;
    }

    m_last_error.clear();
    return true;
}


qint64 ScheduledTaskRepository::add_task(
    qint64 archetype_id,
    const QString& title,
    bool recurring)
{
    const QString clean_title = title.trimmed();

    if (archetype_id < 1 || clean_title.isEmpty())
    {
        m_last_error = "Invalid scheduled task details.";
        return -1;
    }

    QSqlQuery query(m_database);

    query.prepare(R"(
        INSERT INTO scheduled_tasks
            (archetype_id, title, recurring)
        VALUES
            (:archetype_id, :title, :recurring)
    )");

    query.bindValue(":archetype_id", archetype_id);
    query.bindValue(":title", clean_title);
    query.bindValue(":recurring", recurring ? 1 : 0);

    if (!query.exec())
    {
        m_last_error = query.lastError().text();
        return -1;
    }

    m_last_error.clear();
    return query.lastInsertId().toLongLong();
}

bool ScheduledTaskRepository::load_tasks(
    QList<ScheduledTask>& tasks)
{
    QSqlQuery query(m_database);

    if (!query.exec(R"(
        SELECT id, archetype_id, title, recurring
        FROM scheduled_tasks
        ORDER BY id ASC
    )"))
    {
        m_last_error = query.lastError().text();
        return false;
    }

    QList<ScheduledTask> loaded_tasks;

    while (query.next())
    {
        ScheduledTask task;

        task.id = query.value(0).toLongLong();
        task.archetype_id = query.value(1).toLongLong();
        task.title = query.value(2).toString();
        task.recurring = query.value(3).toBool();

        loaded_tasks.append(task);
    }

    tasks = loaded_tasks;

    m_last_error.clear();
    return true;
}


bool ScheduledTaskRepository::load_task_by_id(
    qint64 id,
    ScheduledTask& task)
{
    QSqlQuery query(m_database);

    query.prepare(R"(
        SELECT id, archetype_id, title, recurring
        FROM scheduled_tasks
        WHERE id = :id
    )");

    query.bindValue(":id", id);

    if (!query.exec())
    {
        m_last_error = query.lastError().text();
        return false;
    }

    if (!query.next())
    {
        m_last_error = "Scheduled task not found.";
        return false;
    }

    task.id = query.value(0).toLongLong();
    task.archetype_id = query.value(1).toLongLong();
    task.title = query.value(2).toString();
    task.recurring = query.value(3).toBool();

    m_last_error.clear();
    return true;
}


bool ScheduledTaskRepository::update_task(
    const ScheduledTask& task)
{
    const QString clean_title = task.title.trimmed();

    if (task.id < 1 ||
        task.archetype_id < 1 ||
        clean_title.isEmpty())
    {
        m_last_error = "Invalid scheduled task details.";
        return false;
    }

    QSqlQuery query(m_database);

    query.prepare(R"(
        UPDATE scheduled_tasks
        SET archetype_id = :archetype_id,
            title = :title,
            recurring = :recurring
        WHERE id = :id
    )");

    query.bindValue(":id", task.id);
    query.bindValue(":archetype_id", task.archetype_id);
    query.bindValue(":title", clean_title);
    query.bindValue(":recurring", task.recurring ? 1 : 0);

    if (!query.exec())
    {
        m_last_error = query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0)
    {
        m_last_error = "Scheduled task not found.";
        return false;
    }

    m_last_error.clear();
    return true;
}

bool ScheduledTaskRepository::delete_task(qint64 id)
{
    QSqlQuery query(m_database);

    query.prepare(R"(
        DELETE FROM scheduled_tasks
        WHERE id = :id
    )");

    query.bindValue(":id", id);

    if (!query.exec())
    {
        m_last_error = query.lastError().text();
        return false;
    }

    if (query.numRowsAffected() == 0)
    {
        m_last_error = "Scheduled task not found.";
        return false;
    }

    m_last_error.clear();
    return true;
}


QString ScheduledTaskRepository::last_error() const
{
    return m_last_error;
}