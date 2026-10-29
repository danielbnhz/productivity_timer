#ifndef PRODUCTIVITY_TIMER_TIMER_VIEW_H
#define PRODUCTIVITY_TIMER_TIMER_VIEW_H

#include <QCheckBox>
#include <QMainWindow>
#include <QString>
#include <QtGlobal>
#include <QDateTime>
#include <QSlider>
#include <QDate>
#include <QList>
#include "../persistence/session_repository.h"
#include "session_timeline.h"
#include <QCalendarWidget>
#include "../audio/sound_settings.h"
#include "../model/time_range.h"
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>

class QLabel;
class QPushButton;
class QLineEdit;
class QComboBox;
class QDateTimeEdit;
class QSpinBox;
class QTabWidget;
class QTableWidget;

class Timer_View : public QMainWindow
{
    Q_OBJECT

public:
    explicit Timer_View(QWidget* parent = nullptr);

    void set_remaining_time(qint64 remaining_ms);

    void set_controls(bool running,
                      bool resumable,
                      bool finished);
    void set_break_mode(bool break_mode);
    void clear_task_archetypes();
    void add_task_archetype(qint64 id, const QString& name);

    void show_task_save_result(bool success,
                               const QString& message);

    void set_usage_totals(qint64 total_focused_ms,
                          qint64 total_weighted_ms);
    void clear_usage_rows();
    void add_usage_row(const QString& name,
                       qint64 focused_ms,
                       qint64 weighted_ms);
    void set_usage_error(const QString& message);
    void show_log_result(bool success, const QString& message);

    void set_task_context(
    const QString& title,
    bool has_task,
    bool session_in_progress,
    bool pending_save);
    void set_duration_minutes(int minutes, bool can_decrease, bool can_increase);
    void set_sound_settings(const productivity_timer::SoundSettings& settings);
    void show_calendar_sessions(
    const QDate& day,
    const QList<productivity_timer::SessionEntry>& sessions);
    void build_category_settings(QVBoxLayout* layout, QWidget* parent);
    void add_category_row(
            qint64 id,
            const QString& name,
            bool archived);
    void clear_category_rows();


signals:
    void start_requested();
    void pause_requested();
    void reset_requested();
    void task_save_requested(qint64 archetype_id,
                         const QString& title);
    void data_view_requested();
    void time_range_changed(productivity_timer::TimeRange range);
    void manual_log_requested(qint64 archetype_id,
                              const QString& title,
                              const QDateTime& started_at,
                              int minutes);
    void calendar_date_requested(const QDate& day);

    void duration_step_requested(int direction);
    void sound_settings_changed(const productivity_timer::SoundSettings& settings);
    void sound_test_requested(productivity_timer::SoundKind kind);
    void category_add_requested(const QString& name);
    void work_mode_requested();
    void break_mode_requested();
    void category_rename_requested(qint64 id, const QString& old_name);


private:
    QLabel* m_timer_label = nullptr;
    QLabel* m_status_label = nullptr;
    QPushButton* m_start_button = nullptr;
    QPushButton* m_pause_button = nullptr;
    QPushButton* m_reset_button = nullptr;
    void update_task_save_enabled();
    QLabel* m_current_task_label = nullptr;
    bool m_task_editable = true;
    QLineEdit* m_task_title = nullptr;
    QComboBox* m_archetype_combo = nullptr;
    QPushButton* m_save_task_button = nullptr;
    QLabel* m_task_feedback_label = nullptr;
    QWidget* build_log_page();
    void update_log_enabled();
    QLineEdit* m_log_title = nullptr;
    QComboBox* m_log_archetype_combo = nullptr;
    QDateTimeEdit* m_log_start_edit = nullptr;
    QSpinBox* m_log_duration_spin = nullptr;
    QPushButton* m_log_button = nullptr;
    QLabel* m_log_feedback_label = nullptr;
    QWidget* build_data_page();
    QLabel* m_total_hours_label = nullptr;
    QLabel* m_productive_hours_label = nullptr;
    QLabel* m_data_status_label = nullptr;
    QTableWidget* m_usage_table = nullptr;
    QTableWidget* m_category_table = nullptr;
    QLineEdit* m_category_name_input = nullptr;
    QComboBox* m_category_priority_input = nullptr;
    QString m_saved_task_title;
    QVariant m_saved_archetype_id;
    QPushButton* m_add_category_button = nullptr;
    QPushButton* m_rename_category_button = nullptr;
    QPushButton* m_update_category_priority_button = nullptr;
    QPushButton* m_archive_category_button = nullptr;
    QWidget* build_settings_page();
    productivity_timer::SoundSettings current_sound_settings() const;
    void update_duration_buttons();
    QCalendarWidget* m_calendar_widget = nullptr;
    productivity_timer::SessionTimeline* m_session_timeline = nullptr;
    QPushButton* m_duration_minus = nullptr;
    QPushButton* m_duration_plus = nullptr;
    QLabel* m_duration_label = nullptr;
    bool m_duration_adjustable = true;
    bool m_can_decrease = true;
    bool m_can_increase = true;
    QPushButton* m_work_mode_button = nullptr;
    QPushButton* m_break_mode_button = nullptr;
    bool m_is_break_mode = false;
    QCheckBox* m_done_enabled = nullptr;
    QSlider* m_done_volume = nullptr;
    QCheckBox* m_click_enabled = nullptr;
    QSlider* m_click_volume = nullptr;

};

#endif // PRODUCTIVITY_TIMER_TIMER_VIEW_H