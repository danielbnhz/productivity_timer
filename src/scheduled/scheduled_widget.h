//
// Created by super on 11/1/2026.
//

#ifndef PRODUCTIVITY_TIMER_SCHEDULED_WIDGET_H
#define PRODUCTIVITY_TIMER_SCHEDULED_WIDGET_H
#pragma once

#include "scheduled_task.h"

#include <QWidget>
#include <QList>
#include "../persistence/session_repository.h"



class QListWidget;
class QPushButton;
class QCheckBox;
class QLineEdit;
class QComboBox;

class ScheduledWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScheduledWidget(QWidget* parent = nullptr);

    void set_tasks(const QList<ScheduledTask>& tasks);
    void set_archetypes(const QList<productivity_timer::TaskArchetype>& archetypes);


    signals:
    void launch_requested(qint64 scheduled_id);
    void log_requested(const ScheduledTask& task);

    void add_requested(qint64 archetype_id,
                       const QString& title,
                       bool recurring);

    void edit_requested(const ScheduledTask& task);
    void delete_requested(qint64 scheduled_id);

private:
    void build_ui();
    void refresh_list();

    QList<ScheduledTask> m_tasks;

    QListWidget* m_task_list = nullptr;

    QPushButton* m_add_button = nullptr;
    QList<productivity_timer::TaskArchetype> m_archetypes;
};

#endif //PRODUCTIVITY_TIMER_SCHEDULED_WIDGET_H