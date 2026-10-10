#include "timer_view.h"

#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QVariant>

Timer_View::Timer_View(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Productivity Timer");
    resize(420, 380);

    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);

    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    m_timer_label = new QLabel(central);
    m_timer_label->setAlignment(Qt::AlignCenter);

    QFont timer_font = m_timer_label->font();
    timer_font.setPointSize(36);
    timer_font.setBold(true);
    m_timer_label->setFont(timer_font);

    m_status_label = new QLabel("Ready", central);
    m_status_label->setAlignment(Qt::AlignCenter);

    m_start_button = new QPushButton("Start", central);
    m_pause_button = new QPushButton("Pause", central);
    m_reset_button = new QPushButton("Reset", central);

    auto* button_layout = new QHBoxLayout;
    button_layout->addWidget(m_start_button);
    button_layout->addWidget(m_pause_button);
    button_layout->addWidget(m_reset_button);

    layout->addWidget(m_timer_label);
    layout->addWidget(m_status_label);
    layout->addLayout(button_layout);
    layout->addWidget(new QLabel("Task title", central));

    m_task_title = new QLineEdit(central);
    m_task_title->setPlaceholderText(
        "Example: Finish task database saving");
    layout->addWidget(m_task_title);

    layout->addWidget(new QLabel("Super task", central));

    m_archetype_combo = new QComboBox(central);
    layout->addWidget(m_archetype_combo);

    m_save_task_button =
        new QPushButton("Save task", central);
    layout->addWidget(m_save_task_button);

    m_task_feedback_label = new QLabel(central);
    m_task_feedback_label->setWordWrap(true);
    layout->addWidget(m_task_feedback_label);

    clear_task_archetypes();

    connect(m_task_title, &QLineEdit::textChanged,
            this, [this](const QString&) {
                update_task_save_enabled();
            });

    connect(m_archetype_combo,
            &QComboBox::currentIndexChanged,
            this, [this](int) {
                update_task_save_enabled();
            });

    connect(m_save_task_button, &QPushButton::clicked,
            this, [this]() {
                const QString title =
                    m_task_title->text().trimmed();

                const QVariant selected_id =
                    m_archetype_combo->currentData();

                if (title.isEmpty()
                    || !selected_id.isValid()
                    || selected_id.isNull()) {
                    show_task_save_result(
                        false,
                        "Enter a title and select a super task.");
                    return;
                }

                bool id_ok = false;
                const qint64 archetype_id =
                    selected_id.toLongLong(&id_ok);

                if (!id_ok) {
                    show_task_save_result(
                        false,
                        "The selected super task has an invalid ID.");
                    return;
                }

                emit task_save_requested(archetype_id, title);
            });

    update_task_save_enabled();

    setCentralWidget(central);
    set_controls(false, false, false);

    connect(m_start_button, &QPushButton::clicked,
            this, &Timer_View::start_requested);

    connect(m_pause_button, &QPushButton::clicked,
            this, &Timer_View::pause_requested);

    connect(m_reset_button, &QPushButton::clicked,
            this, &Timer_View::reset_requested);
}

void Timer_View::set_remaining_time(qint64 remaining_ms)
{
    // Round up: 24:59.9 should display as 25:00.
    const qint64 milliseconds = qMax<qint64>(0, remaining_ms);
    const qint64 seconds =
        milliseconds / 1000 + (milliseconds % 1000 != 0);

    const qint64 minutes_part = seconds / 60;
    const qint64 seconds_part = seconds % 60;

    const QString text =
        QString("%1:%2")
            .arg(minutes_part, 2, 10, QChar('0'))
            .arg(seconds_part, 2, 10, QChar('0'));

    m_timer_label->setText(text);
}

void Timer_View::set_controls(bool running,
                             bool resumable,
                             bool finished)
{
    m_start_button->setEnabled(!running);
    m_pause_button->setEnabled(running);
    m_start_button->setText(resumable ? "Resume" : "Start");

    if (finished) {
        m_status_label->setText("Session complete!");
    } else if (running) {
        m_status_label->setText("Focusing");
    } else if (resumable) {
        m_status_label->setText("Paused");
    } else {
        m_status_label->setText("Ready");
    }
}

void Timer_View::clear_task_archetypes()
{
    m_archetype_combo->clear();
    m_archetype_combo->addItem("Select a super task...");
    m_archetype_combo->setCurrentIndex(0);

    update_task_save_enabled();
}

void Timer_View::add_task_archetype(
    qint64 id,
    const QString& name)
{
    m_archetype_combo->addItem(
        name,
        QVariant::fromValue(id));

    update_task_save_enabled();
}

void Timer_View::update_task_save_enabled()
{
    const QVariant selected_id =
        m_archetype_combo->currentData();

    const bool valid_selection =
        selected_id.isValid() && !selected_id.isNull();

    const bool has_title =
        !m_task_title->text().trimmed().isEmpty();

    m_save_task_button->setEnabled(
        valid_selection && has_title);
}

void Timer_View::show_task_save_result(
    bool success,
    const QString& message)
{
    m_task_feedback_label->setText(message);

    if (success) {
        m_task_title->clear();
        m_archetype_combo->setCurrentIndex(0);
    }

    update_task_save_enabled();
}