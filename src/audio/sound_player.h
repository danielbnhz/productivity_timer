#ifndef PRODUCTIVITY_TIMER_SOUND_PLAYER_H
#define PRODUCTIVITY_TIMER_SOUND_PLAYER_H

#include <QObject>
#include "sound_settings.h"

class QSoundEffect;

namespace productivity_timer
{
    class SoundPlayer : public QObject
    {
        Q_OBJECT

    public:
        explicit SoundPlayer(QObject* parent = nullptr);

        void apply(const SoundSettings& settings);
        void play(SoundKind kind);
        void play_test(SoundKind kind);

    private:
        SoundSettings m_settings;
        QSoundEffect* m_done = nullptr;
        QSoundEffect* m_click = nullptr;
    };
}

#endif