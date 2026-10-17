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
#include <QHeaderView>
#include <QTabWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QColor>
#include <QShortcut>
#include <QKeySequence>

namespace
{
    QString format_hours(qint64 milliseconds)
    {
        return QString::number(
                   static_cast<double>(milliseconds) / 3'600'000.0,
                   'f', 1)
               + " h";
    }
}


Timer_View::Timer_View(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Productivity Timer");
    resize(480, 540);
    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);

    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(16);

    m_timer_label = new QLabel(central);
    m_timer_label->setObjectName("timerLabel");
    m_timer_label->setAlignment(Qt::AlignCenter);



    m_status_label = new QLabel("Ready", central);
    m_status_label->setObjectName("statusLabel");

    m_status_label->setAlignment(Qt::AlignCenter);

    m_start_button = new QPushButton("Start", central);
    m_start_button->setObjectName("startButton");
    m_pause_button = new QPushButton("Pause", central);
    m_reset_button = new QPushButton("Reset", central);

    auto* button_layout = new QHBoxLayout;
    button_layout->addWidget(m_start_button);
    button_layout->addWidget(m_pause_button);
    button_layout->addWidget(m_reset_button);

    layout->addWidget(m_timer_label);

    layout->addWidget(m_status_label);

    m_current_task_label =
        new QLabel("Current task: none", central);
    m_current_task_label->setObjectName("currentTaskLabel");
    m_current_task_label->setWordWrap(true);
    m_current_task_label->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_current_task_label);

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

    auto* tabs = new QTabWidget(this);
    tabs->addTab(central, "Timer");
    tabs->addTab(build_data_page(), "Data");
    setCentralWidget(tabs);

    auto* space_shortcut =
    new QShortcut(QKeySequence(Qt::Key_Space), this);

    connect(space_shortcut, &QShortcut::activated, this,
            [this, tabs]() {
                if (tabs->currentIndex() != 0) {
                    return;  // only on the Timer tab
                }
                if (m_pause_button->isEnabled()) {
                    emit pause_requested();
                } else if (m_start_button->isEnabled()) {
                    emit start_requested();
                }
            });

    connect(tabs, &QTabWidget::currentChanged,
            this, [this](int index) {
                if (index == 1) {
                    emit data_view_requested();
                }
            });
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
        m_task_editable && valid_selection && has_title);
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

    void Timer_View::set_task_context(
    const QString& title,
    bool has_task,
    bool session_in_progress,
    bool pending_save)
{
    m_current_task_label->setText(
        has_task
            ? QString("Current task: %1").arg(title)
            : QString("Current task: none"));

    m_task_editable =
        !session_in_progress && !pending_save;

    m_task_title->setEnabled(m_task_editable);
    m_archetype_combo->setEnabled(m_task_editable);
    update_task_save_enabled();

    if (!has_task) {
        m_start_button->setEnabled(false);
    }

    m_reset_button->setEnabled(!pending_save);

    if (pending_save) {
        m_status_label->setText(
            "Timer finished — database save failed.");

        m_start_button->setText("Retry save");
        m_start_button->setEnabled(true);
        m_pause_button->setEnabled(false);
    }
}


QWidget* Timer_View::build_data_page()
{
    auto* page = new QWidget;
    auto* layout = new QVBoxLayout(page);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(12);

    const auto add_stat = [&](const QString& caption,
                              QLabel*& value_label,
                              const QString& object_name) {
        auto* caption_label = new QLabel(caption, page);
        caption_label->setAlignment(Qt::AlignCenter);

        value_label = new QLabel(format_hours(0), page);
        value_label->setObjectName(object_name);
        value_label->setAlignment(Qt::AlignCenter);

        QFont font = value_label->font();
        font.setPointSize(font.pointSize() + 10);
        font.setBold(true);
        value_label->setFont(font);

        layout->addWidget(caption_label);
        layout->addWidget(value_label);
    };

    add_stat("Total hours (all super tasks)",
             m_total_hours_label, "totalHoursLabel");
    add_stat("Productive hours (hours x priority)",
             m_productive_hours_label, "productiveHoursLabel");

    layout->addWidget(new QLabel("By super task", page));

    m_usage_table = new QTableWidget(0, 3, page);
    m_usage_table->setHorizontalHeaderLabels(
        {"Super task", "Hours", "Productive"});
    m_usage_table->verticalHeader()->setVisible(false);
    m_usage_table->setEditTriggers(
        QAbstractItemView::NoEditTriggers);
    m_usage_table->setSelectionMode(
        QAbstractItemView::NoSelection);
    m_usage_table->horizontalHeader()->setSectionResizeMode(
        0, QHeaderView::Stretch);
    layout->addWidget(m_usage_table);

    m_data_status_label = new QLabel(page);
    m_data_status_label->setWordWrap(true);
    layout->addWidget(m_data_status_label);

    return page;
}

void Timer_View::set_usage_totals(qint64 total_focused_ms,
                                  qint64 total_weighted_ms)
{
    m_total_hours_label->setText(format_hours(total_focused_ms));
    m_productive_hours_label->setText(
        format_hours(total_weighted_ms));
    m_data_status_label->clear();
}

void Timer_View::clear_usage_rows()
{
    m_usage_table->setRowCount(0);
}

void Timer_View::add_usage_row(const QString& name,
                               qint64 focused_ms,
                               qint64 weighted_ms)
{
    const int row = m_usage_table->rowCount();
    m_usage_table->insertRow(row);

    m_usage_table->setItem(row, 0, new QTableWidgetItem(name));

    const auto make_number_item = [](qint64 ms) {
        auto* item = new QTableWidgetItem(format_hours(ms));
        item->setTextAlignment(
            Qt::AlignRight | Qt::AlignVCenter);
        return item;
    };

    m_usage_table->setItem(row, 1, make_number_item(focused_ms));
    m_usage_table->setItem(row, 2, make_number_item(weighted_ms));
    m_usage_table->item(row, 2)->setForeground(QColor("#FF8A1F"));
}

void Timer_View::set_usage_error(const QString& message)
{
    m_data_status_label->setText(message);
}