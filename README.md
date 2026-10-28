# Productivity Timer

A personal productivity tracking application built with **C++20, Qt 6 Widgets, and SQLite**. The project combines a focus timer with task organization, manual session logging, productivity summaries, and a visual calendar for reviewing work sessions.

The goal is to build a practical, extensible tool for tracking focused work over time, with an architecture that can eventually support an Android port.

## Current Features

### ⏱️ Timer and Session Tracking
- Focus timer for recording productive work sessions.
- Task and task-archetype organization.
- Session persistence in a local SQLite database.
- Recording of planned duration, focused duration, and effective priority snapshots.
- Manual session logging for work completed outside the active timer.

### 📅 Calendar and Session Timeline
- Calendar-based navigation for reviewing sessions by date.
- A daily timeline that visualizes sessions against the hours of the day.
- Session blocks positioned according to their start and end times.
- Sessions crossing midnight can be clipped to the selected day's boundaries for display without changing their stored timestamps.
- A foundation for identifying overlapping sessions and improving time-allocation decisions.

The calendar is intended to make historical activity easier to inspect visually, rather than relying exclusively on numerical summaries.

### 📊 Productivity Data
- Data summaries organized by day, week, month, year, and all-time periods.
- Productivity calculations based on focused duration and the priority captured for each session.
- A foundation for future trend analysis and historical comparisons.

### 🔊 Custom Sound Effects
The application includes configurable audio feedback for timer completion and session logging.

- **Completion sound:** `fantastical_app_arp.wav`
- **Logging sound:** `log_sound_effect.wav`
- Independent enable/disable settings for each sound.
- Independent volume settings for each sound.
- Volume scaling that squares the normalized percentage, providing finer control at lower volume levels.
- Playback through Qt's `QSoundEffect`, with sound assets loaded from the Qt resource system.

The logging sound uses a custom-designed, retro digital/typewriter-style effect, giving session logging a distinct auditory cue.

### ⚙️ Persistence and Configuration
- SQLite for persistent task and session data.
- Qt settings support for application preferences.
- Repository-based database operations, keeping persistence logic separate from the user interface.
- Session timestamps stored in UTC to support consistent time comparisons.

## Technology Stack

- **Language:** C++20
- **GUI:** Qt 6 Widgets
- **Database:** SQLite through Qt SQL
- **Build system:** CMake
- **Development environment:** CLion with MinGW on Windows

## Project Direction

The application is being developed incrementally, with an emphasis on practical features and reusable logic.

Planned areas of development include:

- **Trends tab:** Visualize productivity patterns and changes over time.
- **Overlap detection:** Identify conflicting session intervals and warn the user without unnecessarily preventing manual logging.
- **Expanded calendar views:** Extend daily inspection toward weekly scheduling and review.
- **Android port:** Reuse suitable domain logic in a Kotlin-based implementation.

The longer-term objective is a personal productivity system that makes it easier to record work, understand time allocation, and make informed decisions about future sessions.

## Design Principles

- **Local-first persistence:** Keep productivity records in a local database.
- **Separation of concerns:** Keep UI, timer behavior, persistence, and reusable time-interval logic distinct.
- **Useful feedback:** Make important actions perceptible through visual and audio cues.
- **Incremental development:** Add features without destabilizing existing timer and logging behavior.
- **Cross-platform potential:** Prefer reusable business logic where practical.

## Status

The project has progressed beyond a basic countdown timer into a multi-view productivity tracking application, with persistent session data, configurable sound effects, productivity summaries, and calendar-based session visualization. Development is ongoing, with trend analysis and expanded scheduling functionality among the next goals.
