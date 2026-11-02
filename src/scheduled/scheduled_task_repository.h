//
// Created by super on 11/1/2026.
//

#ifndef PRODUCTIVITY_TIMER_SCHEDULED_TASK_REPOSITORY_H
#define PRODUCTIVITY_TIMER_SCHEDULED_TASK_REPOSITORY_H
#pragma once

#include "scheduled_task.h"

#include <QList>
#include <QString>

#include <QtSql/QSqlDatabase>

class ScheduledTaskRepository
{
public:
    explicit ScheduledTaskRepository(const QString& database_path);
    ~ScheduledTaskRepository();

    bool initialize();

    qint64 add_task(qint64 archetype_id,
                    const QString& title,
                    bool recurring);

    bool load_tasks(QList<ScheduledTask>& tasks);

    bool load_task_by_id(qint64 id, ScheduledTask& task);

    bool update_task(const ScheduledTask& task);

    bool delete_task(qint64 id);

    QString last_error() const;

private:
    QSqlDatabase m_database;
    QString m_connection_name;
    QString m_last_error;
};

#endif //PRODUCTIVITY_TIMER_SCHEDULED_TASK_REPOSITORY_H