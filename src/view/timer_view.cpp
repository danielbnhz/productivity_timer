#include "timer_view.h"

#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

Timer_View::Timer_View(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("Productivity Timer");
    resize(360, 220);

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