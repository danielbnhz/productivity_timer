#ifndef PRODUCTIVITY_TIMER_SOUND_SETTINGS_H
#define PRODUCTIVITY_TIMER_SOUND_SETTINGS_H

namespace productivity_timer
{
    enum class SoundKind { Done, Click };

    struct SoundSettings
    {
        bool done_enabled = true;
        int done_volume = 70;
        bool click_enabled = true;
        int click_volume = 60;
    };
}

#endif