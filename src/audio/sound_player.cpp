#include "sound_player.h"

#include <QDebug>
#include <QSoundEffect>
#include <QUrl>
#include <QtGlobal>

namespace productivity_timer
{
    namespace
    {

        qreal to_volume(int percent)
        {
            const qreal v = qBound(0, percent, 100) / 100.0;
            return v * v;
        }
    }

    SoundPlayer::SoundPlayer(QObject* parent)
        : QObject(parent)
    {
        m_done = new QSoundEffect(this);
        m_done->setSource(QUrl(
            QStringLiteral("qrc:/resources/sounds/fantastical_app_arp.wav")));

        m_click = new QSoundEffect(this);
        m_click->setSource(QUrl(
            QStringLiteral("qrc:/resources/sounds/log_sound_effect.wav")));

        apply(m_settings);
    }

    void SoundPlayer::apply(const SoundSettings& settings)
    {
        m_settings = settings;
        m_done->setVolume(to_volume(settings.done_volume));
        m_click->setVolume(to_volume(settings.click_volume));
    }

    void SoundPlayer::play(SoundKind kind)
    {
        const bool enabled = (kind == SoundKind::Done)
            ? m_settings.done_enabled
            : m_settings.click_enabled;

        if (enabled) {
            play_test(kind);
        }
    }

    void SoundPlayer::play_test(SoundKind kind)
    {
        QSoundEffect* effect =
            (kind == SoundKind::Done) ? m_done : m_click;

        if (effect->status() == QSoundEffect::Error) {
            qWarning() << "Sound failed to load:" << effect->source();
            return;
        }

        effect->play();
    }
}