#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QList>
#include <QObject>
#include <QSqlDatabase>
#include <QIcon>
#include <optional>
#include "src/audio/sound_player.h"
#include "src/model/timer.h"
#include "src/view/timer_view.h"
#include "src/persistence/session_repository.h"
#include <QSettings>
#include "src/view/theme.h"
#include <QStyleHints>
#include <QInputDialog>
#include <QLineEdit>
#include <QTime>
#include<QTimeZone>


#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>

#endif

namespace
{
    void apply_dark_title_bar(QWidget* window)
    {
#ifdef Q_OS_WIN
        const HWND hwnd = reinterpret_cast<HWND>(window->winId());

        const BOOL dark = TRUE;
        const HRESULT r1 =
            DwmSetWindowAttribute(hwnd, 20, &dark, sizeof(dark));

        const COLORREF caption = RGB(0x0F, 0x1B, 0x2D);
        const HRESULT r2 =
            DwmSetWindowAttribute(hwnd, 35, &caption, sizeof(caption));

        const COLORREF text = RGB(0xE6, 0xED, 0xF7);
        const HRESULT r3 =
            DwmSetWindowAttribute(hwnd, 36, &text, sizeof(text));

        qDebug() << "DWM results (0 = ok): dark" << r1
                 << "caption" << r2 << "text" << r3;
#else
        Q_UNUSED(window);
#endif
    }
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.styleHints()->setColorScheme(Qt::ColorScheme::Dark);


    app.setWindowIcon(QIcon(":/resources/icons/timer_icon.png"));
    app.setStyle("Fusion");
    app.setStyleSheet(productivity_timer::theme_stylesheet());

    QCoreApplication::setOrganizationName("DanielHernandez");
    QCoreApplication::setApplicationName("ProductivityTimer");

    qDebug() << "Available SQL drivers:"
             << QSqlDatabase::drivers();

    qDebug() << "SQLite available:"
             << QSqlDatabase::isDriverAvailable("QSQLITE");

    using productivity_timer::SessionRepository;
    using productivity_timer::TaskArchetype;
    using productivity_timer::Timer;

    SessionRepository repository;
    const bool database_ready = repository.initialize();

    QString startup_error;

    if (!database_ready) {
        startup_error = repository.last_error();
        qWarning() << "Database initialization failed:"
                   << startup_error;
    } else {
        qDebug() << "Database ready:"
                 << repository.database_path();

        const qint64 count = repository.session_count();

        if (count < 0) {
            qWarning() << "Session count failed:"
                       << repository.last_error();
        } else {
            qDebug() << "Saved sessions:" << count;
        }
    }

    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       "DanielHernandez", "ProductivityTimer");

    const int start_minutes = Timer::snap_minutes(
        settings.value("timer/duration_minutes", 45).toInt());


    Timer_View view;

    productivity_timer::SoundSettings sound_settings;
    sound_settings.done_enabled  = settings.value("sound/done_enabled", true).toBool();
    sound_settings.done_volume   = settings.value("sound/done_volume", 70).toInt();
    sound_settings.click_enabled = settings.value("sound/click_enabled", true).toBool();
    sound_settings.click_volume  = settings.value("sound/click_volume", 60).toInt();

    productivity_timer::SoundPlayer sounds;
    sounds.apply(sound_settings);
    view.set_sound_settings(sound_settings);
    Timer timer(static_cast<qint64>(start_minutes) * 60 * 1000);
    bool break_mode = false;
    int work_duration_minutes = timer.duration_minutes();
    struct CurrentTask
    {
        qint64 id;
        QString title;
    };

    struct SessionDraft
    {
        qint64 task_id;
        QDateTime started_at;
        QDateTime ended_at;
        int priority;
        qint64 planned_ms;

    };

    std::optional<CurrentTask> current_task;
    std::optional<SessionDraft> session;
    bool pending_save = false;

    const auto refresh_controls = [&]() {
        const Timer::State state = timer.state();

        view.set_controls(
            state == Timer::State::Running,
            state == Timer::State::Paused,
            state == Timer::State::Finished);

        view.set_task_context(
            current_task ? current_task->title : QString{},
            current_task.has_value(),
            state == Timer::State::Running
                || state == Timer::State::Paused,
            pending_save);
    };

    QObject::connect(
    &view,
    &Timer_View::break_mode_requested,
    &view,
    [&]() {
        const Timer::State state = timer.state();

        if (state == Timer::State::Running
            || state == Timer::State::Paused
            || pending_save) {
            view.show_task_save_result(
                false,
                "Reset or finish the current session before changing modes.");
            return;
        }

        session.reset();

        break_mode = true;

        timer.reset_timer();
        timer.set_duration_minutes(5);

        view.set_break_mode(true);
        view.set_duration_minutes(
            5,
            false,
            true);

        refresh_controls();
    });

    QObject::connect(
        &view,
        &Timer_View::work_mode_requested,
        &view,
        [&]() {
            const Timer::State state = timer.state();

            if (state == Timer::State::Running
                || state == Timer::State::Paused
                || pending_save) {
                view.show_task_save_result(
                    false,
                    "Reset or finish the current session before changing modes.");
                return;
            }

            session.reset();
            break_mode = false;

            timer.reset_timer();
            timer.set_duration_minutes(work_duration_minutes);

            view.set_break_mode(false);


            const int minutes = timer.duration_minutes();

            view.set_duration_minutes(
                minutes,
                minutes - Timer::kStepMinutes >= Timer::kMinMinutes,
                minutes + Timer::kStepMinutes <= Timer::kMaxMinutes);

            refresh_controls();
        });

    const auto save_completion = [&]() {
        if (!session || !pending_save) {
            return;
        }

        const bool saved =
            repository.complete_task_session(
                session->task_id,
                session->started_at,
                session->ended_at,
                session->planned_ms,
                session->planned_ms,
                session->priority);

        if (!saved) {
            view.show_task_save_result(
                false,
                "Completion not saved: "
                    + repository.last_error());

            refresh_controls();
            return;
        }

        const qint64 saved_task_id = session->task_id;

        pending_save = false;
        session.reset();
        current_task.reset();

        view.show_task_save_result(
            true,
            QString(
                "Task %1 completed and session saved. "
                "Save another task to begin.")
                .arg(saved_task_id));

        qDebug() << "Completed task:" << saved_task_id;

        refresh_controls();
    };

    QObject::connect(
        &view,
        &Timer_View::task_save_requested,
        &view,
        [&](qint64 archetype_id, const QString& title) {
            const Timer::State state = timer.state();

            if (pending_save
                || state == Timer::State::Running
                || state == Timer::State::Paused) {
                view.show_task_save_result(
                    false,
                    "Finish or reset the current session first.");
                return;
            }

            const qint64 task_id =
                repository.create_task(archetype_id, title);

            if (task_id == -1) {
                view.show_task_save_result(
                    false,
                    repository.last_error());
                return;
            }

            current_task = CurrentTask{
                task_id,
                title.trimmed()
            };

            session.reset();
            timer.reset_timer();

            view.show_task_save_result(
                true,
                QString("Task saved and selected. ID: %1")
                    .arg(task_id));

            refresh_controls();
        });

    QObject::connect(
        &view,
        &Timer_View::start_requested,
        &view,
        [&]() {
            if (pending_save) {
                save_completion();
                return;
            }

            if (timer.state() == Timer::State::Running) {
                return;
            }

            if (timer.state() == Timer::State::Paused) {
                timer.start_timer();
                return;
            }

            if (!current_task) {
                view.show_task_save_result(
                    false,
                    "Save a task before starting.");
                return;
            }

            int priority = 0;

            if (!repository.task_priority(
                    current_task->id, priority)) {
                view.show_task_save_result(
                    false,
                    repository.last_error());
                return;
            }

            session = SessionDraft{
                current_task->id,
                QDateTime::currentDateTimeUtc(),
                QDateTime{},
                priority,
                static_cast<qint64>(timer.duration_minutes()) * 60 * 1000
            };

            timer.start_timer();
        });

    QObject::connect(
        &view,
        &Timer_View::pause_requested,
        &timer,
        &Timer::pause_timer);

    QObject::connect(
        &view,
        &Timer_View::reset_requested,
        &view,
        [&]() {
            if (pending_save) {
                return;
            }

            session.reset();
            timer.reset_timer();

            view.show_task_save_result(
                false,
                "Unfinished session discarded. "
                "The selected task remains active.");

            refresh_controls();
        });

    QObject::connect(
        &timer,
        &Timer::remaining_time_changed,
        &view,
        &Timer_View::set_remaining_time);

    QObject::connect(
        &timer,
        &Timer::state_changed,
        &view,
        [&](Timer::State) {
            refresh_controls();
        });

    QObject::connect(
        &timer,
        &Timer::finished,
        &view,
        [&]() {
            sounds.play(productivity_timer::SoundKind::Done);
            if (break_mode) {
                view.show_task_save_result(
                    true,
                    "Break complete. Take a moment before returning to Work.");

                refresh_controls();
                return;
            }
            if (!session) {
                view.show_task_save_result(
                    false,
                    "Timer finished without session data.");
                return;
            }

            session->ended_at =
                QDateTime::currentDateTimeUtc();

            pending_save = true;
            save_completion();
        });

    if (!database_ready) {
        view.show_task_save_result(false, startup_error);
    } else {
        QList<TaskArchetype> archetypes;

        if (!repository.load_active_archetypes(archetypes)) {
            view.show_task_save_result(
                false, repository.last_error());
        } else {
            view.clear_task_archetypes();


            view.clear_category_rows();

            for (const auto& archetype : archetypes) {
                view.add_task_archetype(
                    archetype.id,
                    archetype.name);

                view.add_category_row(
                    archetype.id,
                    archetype.name,
                    false);
            }

            qDebug() << "Loaded active archetypes:"
                     << archetypes.size();

            if (archetypes.isEmpty()) {
                view.show_task_save_result(
                    false,
                    "No categories yet. Add categories in Settings.");
            }
        }
    }

    productivity_timer::TimeRange usage_range =
        productivity_timer::TimeRange::All;

    const auto refresh_usage = [&]() {
        productivity_timer::UsageSummary summary;

        if (!repository.load_usage_summary(summary, usage_range)) {
            view.set_usage_error(
                "Could not load data: " + repository.last_error());
            return;
        }

        view.set_usage_totals(
            summary.total_focused_ms,
            summary.total_weighted_ms);

        view.clear_usage_rows();

        for (const auto& row : summary.by_archetype) {
            view.add_usage_row(
                row.name,
                row.focused_ms,
                row.weighted_ms);
        }
    };

    QObject::connect(
        &view,
        &Timer_View::data_view_requested,
        &view,
        refresh_usage);

    QObject::connect(
        &view,
        &Timer_View::time_range_changed,
        &view,
        [&](productivity_timer::TimeRange range) {
            usage_range = range;
            refresh_usage();
        });

    QObject::connect(
        &view,
        &Timer_View::manual_log_requested,
        &view,
        [&](qint64 archetype_id,
            const QString& title,
            const QDateTime& started_at,
            int minutes) {
            const qint64 duration_ms =
                static_cast<qint64>(minutes) * 60 * 1000;

            if (!repository.log_manual_session(
                    archetype_id, title, started_at, duration_ms)) {
                view.show_log_result(false, repository.last_error());
                return;
            }

            view.show_log_result(
                true,
                QString("Logged %1 min: %2")
                    .arg(minutes)
                    .arg(title.trimmed()));

            sounds.play(productivity_timer::SoundKind::Click);

        });

    QObject::connect(
        &view,
        &Timer_View::calendar_date_requested,
        &view,
        [&repository, &view](const QDate& day)
        {
            const QDateTime range_start(
                day, QTime(0, 0), QTimeZone::systemTimeZone());

            const QDateTime range_end(
                day.addDays(1),
                QTime(0, 0),
                QTimeZone::systemTimeZone());

            QList<productivity_timer::SessionEntry> sessions;

            if (!repository.load_sessions_for_range(
                    range_start, range_end, sessions)) {

                return;
            }

            view.show_calendar_sessions(day, sessions);
        });

    view.set_remaining_time(timer.remaining_ms());
    refresh_controls();
    const auto push_duration = [&]() {
        const int minutes = timer.duration_minutes();
        view.set_duration_minutes(
            minutes,
            minutes - Timer::kStepMinutes >= Timer::kMinMinutes,
            minutes + Timer::kStepMinutes <= Timer::kMaxMinutes);
    };

    QObject::connect(
        &view, &Timer_View::duration_step_requested, &view,
        [&](int direction) {
            const int target =
                timer.duration_minutes() + direction * Timer::kStepMinutes;

            if (!timer.set_duration_minutes(target)) {
                return;
            }

            settings.setValue("timer/duration_minutes",
                              timer.duration_minutes());
            push_duration();
        });

    QObject::connect(
        &view, &Timer_View::sound_settings_changed, &view,
        [&](const productivity_timer::SoundSettings& s) {
            sounds.apply(s);
            settings.setValue("sound/done_enabled", s.done_enabled);
            settings.setValue("sound/done_volume", s.done_volume);
            settings.setValue("sound/click_enabled", s.click_enabled);
            settings.setValue("sound/click_volume", s.click_volume);
        });

    QObject::connect(
        &view, &Timer_View::sound_test_requested, &view,
        [&](productivity_timer::SoundKind kind) {
            sounds.play_test(kind);
        });

    QObject::connect(&view, &Timer_View::category_add_requested,
                     &view, [&repository, &view](const QString& name) {
    if (name.isEmpty()) {
        view.show_task_save_result(false,
                                   "Enter a category name.");
        return;
    }


    const qint64 id = repository.create_archetype(
        name, productivity_timer::Priority::Normal);

    if (id < 0) {
        view.show_task_save_result(
            false, repository.last_error());
        return;
    }

    QList<TaskArchetype> archetypes;

    if (!repository.load_active_archetypes(archetypes)) {
        view.show_task_save_result(
            false, repository.last_error());
        return;
    }

    view.clear_task_archetypes();
    view.clear_category_rows();

    for (const auto& archetype : archetypes) {
        view.add_task_archetype(
            archetype.id, archetype.name);

        view.add_category_row(
            archetype.id, archetype.name,
            archetype.archived);
    }

    view.show_task_save_result(
        true, "Category added successfully.");

});

    QObject::connect(
    &view, &Timer_View::category_rename_requested,
    &view, [&repository, &view](qint64 id, const QString& old_name) {
        if (id <= 0) {
            view.show_task_save_result(
                false, "Select a category to rename.");
            return;
        }

        bool accepted = false;
        const QString new_name = QInputDialog::getText(
            &view,
            "Rename Category",
            "New category name:",
            QLineEdit::Normal,
            old_name,
            &accepted
        ).trimmed();

        if (!accepted) {
            return;
        }

        if (new_name.isEmpty()) {
            view.show_task_save_result(
                false, "Enter a category name.");
            return;
        }

        // if (!repository.rename_archetype(id, new_name)) {
        //     view.show_task_save_result(
        //         false, repository.last_error());
        //     return;
        // }

        const bool renamed =
            repository.rename_archetype(id, new_name);

        qDebug() << "Rename requested:"
                 << "ID =" << id
                 << "Old name =" << old_name
                 << "New name =" << new_name
                 << "Success =" << renamed
                 << "Error =" << repository.last_error();

        if (!renamed) {
            view.show_task_save_result(
                false, repository.last_error());
            return;
        }

        QList<TaskArchetype> archetypes;

        if (!repository.load_active_archetypes(archetypes)) {
            view.show_task_save_result(
                false, repository.last_error());
            return;
        }

        view.clear_task_archetypes();
        view.clear_category_rows();

        for (const auto& archetype : archetypes) {
            view.add_task_archetype(
                archetype.id, archetype.name);

            view.add_category_row(
                archetype.id, archetype.name,
                archetype.archived);
        }

        view.show_task_save_result(
            true, "Category renamed successfully.");
    });

    push_duration();
    view.show();



    return app.exec();
}