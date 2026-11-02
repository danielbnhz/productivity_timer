//
// Created by super on 11/1/2026.
//

#include "scheduled_task_dialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include "../persistence/session_repository.h"

namespace productivity_timer
{
    struct TaskArchetype;
}

ScheduledTaskDialog::ScheduledTaskDialog(
    const QList<productivity_timer::TaskArchetype>& archetypes,
    QWidget* parent
)
    : QDialog(parent)
{
    setWindowTitle("Scheduled Activity");

    auto* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Activity title", this));

    m_title_edit = new QLineEdit(this);
    m_title_edit->setPlaceholderText("Example: Study programming");
    layout->addWidget(m_title_edit);

    layout->addWidget(new QLabel("Supertask", this));

    m_archetype_combo = new QComboBox(this);

    for (const productivity_timer::TaskArchetype& archetype : archetypes)
    {
        m_archetype_combo->addItem(
            archetype.name,
            archetype.id
        );
    }

    layout->addWidget(m_archetype_combo);

    m_recurring_check = new QCheckBox("Recurring activity", this);
    layout->addWidget(m_recurring_check);

    auto* button_layout = new QHBoxLayout;

    auto* save_button = new QPushButton("Save", this);
    auto* cancel_button = new QPushButton("Cancel", this);

    button_layout->addWidget(save_button);
    button_layout->addWidget(cancel_button);

    layout->addLayout(button_layout);

    connect(save_button, &QPushButton::clicked,
            this, &QDialog::accept);

    connect(cancel_button, &QPushButton::clicked,
            this, &QDialog::reject);
}




ScheduledTask ScheduledTaskDialog::task_data() const
{
    ScheduledTask task;

    task.title = m_title_edit->text().trimmed();

    task.archetype_id =
        m_archetype_combo->currentData().toLongLong();

    task.recurring = m_recurring_check->isChecked();

    return task;
}
