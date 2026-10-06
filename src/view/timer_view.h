#ifndef PRODUCTIVITY_TIMER_TIMER_VIEW_H
#define PRODUCTIVITY_TIMER_TIMER_VIEW_H

#include <QMainWindow>
#include <QString>
#include <QtGlobal>

class QLabel;
class QPushButton;

class Timer_View : public QMainWindow
{
    Q_OBJECT

public:
    explicit Timer_View(QWidget* parent = nullptr);

    void set_remaining_time(qint64 remaining_ms);

    void set_controls(bool running,
                      bool resumable,
                      bool finished);

    signals:
        void start_requested();
    void pause_requested();
    void reset_requested();

private:
    QLabel* m_timer_label = nullptr;
    QLabel* m_status_label = nullptr;
    QPushButton* m_start_button = nullptr;
    QPushButton* m_pause_button = nullptr;
    QPushButton* m_reset_button = nullptr;
};

#endif // PRODUCTIVITY_TIMER_TIMER_VIEW_H