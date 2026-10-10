#ifndef PRODUCTIVITY_TIMER_TIMER_VIEW_H
#define PRODUCTIVITY_TIMER_TIMER_VIEW_H

#include <QMainWindow>
#include <QString>
#include <QtGlobal>

class QLabel;
class QPushButton;
class QLineEdit;
class QComboBox;

class Timer_View : public QMainWindow
{
    Q_OBJECT

public:
    explicit Timer_View(QWidget* parent = nullptr);

    void set_remaining_time(qint64 remaining_ms);

    void set_controls(bool running,
                      bool resumable,
                      bool finished);
    void clear_task_archetypes();
    void add_task_archetype(qint64 id, const QString& name);

    void show_task_save_result(bool success,
                               const QString& message);

    signals:
        void start_requested();
    void pause_requested();
    void reset_requested();
    void task_save_requested(qint64 archetype_id,
                         const QString& title);

private:
    QLabel* m_timer_label = nullptr;
    QLabel* m_status_label = nullptr;
    QPushButton* m_start_button = nullptr;
    QPushButton* m_pause_button = nullptr;
    QPushButton* m_reset_button = nullptr;
    void update_task_save_enabled();

    QLineEdit* m_task_title = nullptr;
    QComboBox* m_archetype_combo = nullptr;
    QPushButton* m_save_task_button = nullptr;
    QLabel* m_task_feedback_label = nullptr;
};

#endif // PRODUCTIVITY_TIMER_TIMER_VIEW_H