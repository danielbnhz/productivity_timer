#ifndef PRODUCTIVITY_TIMER_TIMER_H
#define PRODUCTIVITY_TIMER_TIMER_H

#include <QObject>
#include <QElapsedTimer>
#include <QTimer>
#include <QtGlobal>

namespace productivity_timer
{

    class Timer : public QObject
    {
        Q_OBJECT

    public:
        enum class State {
            Ready,
            Running,
            Paused,
            Finished
        };
        Q_ENUM(State)

        explicit Timer(qint64 duration_ms = 25 * 60 * 1000,
                       QObject* parent = nullptr);

        [[nodiscard]] qint64 remaining_ms() const;
        [[nodiscard]] State state() const;

    public slots:
        void start_timer();
        void pause_timer();
        void reset_timer();

        signals:
            void remaining_time_changed(qint64 remaining_ms);
        void state_changed(productivity_timer::Timer::State state);
        void finished();

    private:
        void update_timer();
        void set_state(State state);

        QTimer m_refresh_timer;
        QElapsedTimer m_elapsed_timer;

        qint64 m_duration_ms;
        qint64 m_remaining_at_start_ms;
        State m_state = State::Ready;
    };

}

#endif // PRODUCTIVITY_TIMER_TIMER_H