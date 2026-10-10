#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QList>
#include <QObject>
#include <QSqlDatabase>

#include <optional>

#include "src/model/timer.h"
#include "src/view/timer_view.h"
#include "src/persistence/session_repository.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

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

    constexpr qint64 duration_ms = 5000;

    Timer timer(duration_ms);
    Timer_View view;

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

    const auto save_completion = [&]() {
        if (!session || !pending_save) {
            return;
        }

        const bool saved =
            repository.complete_task_session(
                session->task_id,
                session->started_at,
                session->ended_at,
                duration_ms,
                duration_ms,
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
            false,
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
                priority
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
    } else if (!repository.ensure_starter_archetypes()) {
        view.show_task_save_result(
            false, repository.last_error());
    } else {
        QList<TaskArchetype> archetypes;

        if (!repository.load_active_archetypes(archetypes)) {
            view.show_task_save_result(
                false, repository.last_error());
        } else {
            view.clear_task_archetypes();

            for (const auto& archetype : archetypes) {
                view.add_task_archetype(
                    archetype.id,
                    archetype.name);
            }

            qDebug() << "Loaded active archetypes:"
                     << archetypes.size();

            if (archetypes.isEmpty()) {
                view.show_task_save_result(
                    false,
                    "No active super tasks are available.");
            }
        }
    }

    view.set_remaining_time(timer.remaining_ms());
    refresh_controls();
    view.show();

    return app.exec();
}