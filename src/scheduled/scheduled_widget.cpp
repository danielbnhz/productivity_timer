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
    m_task_list->clear();

    for (const auto& task : m_tasks)
    {
        auto* item = new QListWidgetItem(m_task_list);
        auto* row = new QWidget(m_task_list);
        auto* layout = new QHBoxLayout(row);

        layout->setContentsMargins(6, 3, 6, 3);

        QString display_text = task.title;

        if (task.recurring)
            display_text += " [Recurring]";

        auto* label = new QLabel(display_text, row);
        auto* ready_button = new QPushButton("Ready", row);
        auto* log_button = new QPushButton("Log", row);
        auto* delete_button = new QPushButton("Delete", row);

        layout->addWidget(label, 1);
        layout->addWidget(ready_button);
        layout->addWidget(log_button);
        layout->addWidget(delete_button);

        item->setSizeHint(row->sizeHint());
        m_task_list->setItemWidget(item, row);

        connect(ready_button, &QPushButton::clicked,
                this, [this, task]()
        {
            emit launch_requested(task.id);
        });

        connect(log_button, &QPushButton::clicked,
                this, [this, task]()
        {
            emit log_requested(task);
        });

        connect(delete_button, &QPushButton::clicked,
                this, [this, task]()
        {
            emit delete_requested(task.id);
        });
    }
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