//
// Created by super on 11/1/2026.
//

#include "scheduled_widget.h"

#include "scheduled_widget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include "scheduled_task_dialog.h"

ScheduledWidget::ScheduledWidget(QWidget* parent)
    : QWidget(parent)
{

    build_ui();
    connect(m_add_button, &QPushButton::clicked,
            this, [this]()
    {
        ScheduledTaskDialog dialog(m_archetypes, this);

        if (dialog.exec() == QDialog::Accepted)
        {
            const ScheduledTask task = dialog.task_data();

            emit add_requested(
                task.archetype_id,
                task.title,
                task.recurring
            );
        }
    });
}

void ScheduledWidget::build_ui()
{
    auto* main_layout = new QVBoxLayout(this);

    auto* heading = new QLabel("Scheduled Activities", this);

    m_task_list = new QListWidget(this);

    m_add_button = new QPushButton("Add Activity", this);

    main_layout->addWidget(heading);
    main_layout->addWidget(m_task_list);
    main_layout->addWidget(m_add_button);
}

void ScheduledWidget::set_tasks(
    const QList<ScheduledTask>& tasks)
{
    m_tasks = tasks;
    refresh_list();
}

void ScheduledWidget::refresh_list()
{
    m_task_list->clear();

    for (const ScheduledTask& task : m_tasks)
    {
        QString label = task.title;

        if (task.recurring)
            label += "  [Recurring]";

        m_task_list->addItem(label);
    }
}

void ScheduledWidget::set_archetypes(
    const QList<productivity_timer::TaskArchetype>& archetypes)
{
    m_archetypes = archetypes;
}