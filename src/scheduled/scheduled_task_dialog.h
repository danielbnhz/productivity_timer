//
// Created by super on 11/1/2026.
//

#ifndef PRODUCTIVITY_TIMER_SCHEDULED_TASK_DIALOG_H
#define PRODUCTIVITY_TIMER_SCHEDULED_TASK_DIALOG_H
#pragma once

#include "scheduled_task.h"

#include <QDialog>
#include <QList>

namespace productivity_timer
{
    struct TaskArchetype;
}

class QLineEdit;
class QComboBox;
class QCheckBox;
class QPushButton;

class ScheduledTaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScheduledTaskDialog(
        const QList<productivity_timer::TaskArchetype>& archetypes,
        QWidget* parent = nullptr
    );

    ScheduledTask task_data() const;

private:
    QLineEdit* m_title_edit = nullptr;
    QComboBox* m_archetype_combo = nullptr;
    QCheckBox* m_recurring_check = nullptr;
};


#endif //PRODUCTIVITY_TIMER_SCHEDULED_TASK_DIALOG_H