#include <QApplication>
#include <QObject>

#include "timer.h"
#include "../view/timer_view.h"

#include <algorithm>

namespace productivity_timer
{

Timer::Timer(qint64 duration_ms, QObject* parent)
    : QObject(parent),
      m_duration_ms(std::max<qint64>(1, duration_ms)),
      m_remaining_at_start_ms(m_duration_ms)
{
    m_refresh_timer.setInterval(100);

    connect(&m_refresh_timer, &QTimer::timeout,
            this, &Timer::update_timer);
}

qint64 Timer::remaining_ms() const
{
    if (m_state != State::Running) {
        return m_remaining_at_start_ms;
    }

    return std::max<qint64>(
        0,
        m_remaining_at_start_ms - m_elapsed_timer.elapsed()
    );
}

Timer::State Timer::state() const
{
    return m_state;
}

void Timer::start_timer()
{
    if (m_state == State::Running) {
        return;
    }

    if (m_state == State::Finished) {
        m_remaining_at_start_ms = m_duration_ms;
    }

    m_elapsed_timer.start();
    m_refresh_timer.start();

    set_state(State::Running);
    emit remaining_time_changed(remaining_ms());
}

void Timer::pause_timer()
{
    if (m_state != State::Running) {
        return;
    }

    const qint64 remaining = remaining_ms();

    if (remaining == 0) {
        update_timer();
        return;
    }

    m_refresh_timer.stop();
    m_remaining_at_start_ms = remaining;
    m_elapsed_timer.invalidate();

    set_state(State::Paused);
    emit remaining_time_changed(m_remaining_at_start_ms);
}

void Timer::reset_timer()
{
    m_refresh_timer.stop();
    m_elapsed_timer.invalidate();
    m_remaining_at_start_ms = m_duration_ms;

    set_state(State::Ready);
    emit remaining_time_changed(m_remaining_at_start_ms);
}

void Timer::update_timer()
{
    if (m_state != State::Running) {
        return;
    }

    const qint64 remaining = remaining_ms();

    if (remaining > 0) {
        emit remaining_time_changed(remaining);
        return;
    }

    m_refresh_timer.stop();
    m_remaining_at_start_ms = 0;
    m_elapsed_timer.invalidate();

    set_state(State::Finished);
    emit remaining_time_changed(0);
    emit finished();
}

void Timer::set_state(State state)
{
    if (m_state == state) {
        return;
    }

    m_state = state;
    emit state_changed(m_state);
}

}