#include <QApplication>
#include <QObject>

#include "src/model/timer.h"
#include "src/view/timer_view.h"
#include <QDebug>
#include <QSqlDatabase>
#include "src/persistence/session_repository.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    qDebug() << "Available SQL drivers:"
         << QSqlDatabase::drivers();

    qDebug() << "SQLite available:"
             << QSqlDatabase::isDriverAvailable("QSQLITE");


    QCoreApplication::setOrganizationName("DanielHernandez");
    QCoreApplication::setApplicationName("ProductivityTimer");

    productivity_timer::SessionRepository repository;

    if (!repository.initialize()) {
        qWarning() << "Database initialization failed:"
                   << repository.last_error();
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

    using productivity_timer::Timer;

    // Timer timer(25 * 60 * 1000);
    Timer timer(5000);

    Timer_View view;

    // View requests -> model actions.
    QObject::connect(
        &view, &Timer_View::start_requested,
        &timer, &Timer::start_timer
    );

    QObject::connect(
        &view, &Timer_View::pause_requested,
        &timer, &Timer::pause_timer
    );

    QObject::connect(
        &view, &Timer_View::reset_requested,
        &timer, &Timer::reset_timer
    );

    // Model updates -> displayed countdown.
    QObject::connect(
        &timer, &Timer::remaining_time_changed,
        &view, &Timer_View::set_remaining_time
    );

    // Model state -> button availability and status text.
    const auto update_controls = [&view](Timer::State state) {
        view.set_controls(
            state == Timer::State::Running,
            state == Timer::State::Paused,
            state == Timer::State::Finished
        );
    };

    QObject::connect(
        &timer, &Timer::state_changed,
        &view, update_controls
    );

    // Render the initial state before showing the window.
    view.set_remaining_time(timer.remaining_ms());
    update_controls(timer.state());

    view.show();

    return app.exec();
}