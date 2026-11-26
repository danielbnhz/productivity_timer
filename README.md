# Productivity Timer

A personal productivity tracking application built with **C++20, Qt 6 Widgets, and SQLite**. The application combines focus timing, task organization, scheduled work, manual session logging, productivity analysis, and historical activity review.

Designed as a local-first productivity system, the project emphasizes persistent session tracking, reusable application logic, and a modular architecture that can support future expansion and an eventual Android implementation.

## Features

### ⏱️ Timer and Session Tracking

- Focus timer for recording productive work sessions.
- Task and task-archetype organization.
- Configurable task priorities ranging from 0 to 4.
- Persistent session recording through SQLite.
- Tracking of planned duration and actual focused duration.
- Priority snapshots captured when sessions are recorded.
- Break periods excluded from productive session calculations.
- Task-completion feedback through visual window and taskbar notifications.
- Keyboard-oriented timer interactions for efficient session management.

Productivity scoring uses focused duration multiplied by the priority captured for the session:

`Productivity Score = Focused Duration × Priority Snapshot`

Capturing priority at session time preserves historical scoring even when task priorities are subsequently changed.

### 📋 Task Organization and Scheduling

The application separates tasks into archetypes and individual task entries.

- Organize tasks by category and archetype.
- Assign individual priority levels.
- Maintain scheduled tasks independently of active timer sessions.
- Transition ready scheduled tasks into the timer workflow.
- Persist task and scheduling information locally.

This organization provides a foundation for planning future work while maintaining a historical record of completed sessions.

### 📅 Calendar and Session History

Calendar-based navigation provides a way to inspect previously recorded activity.

- Navigate historical sessions by date.
- View daily activity through a visual timeline.
- Display session blocks according to their recorded start and end times.
- Account for sessions crossing midnight when displaying daily activity.
- Preserve original timestamps when clipping sessions to calendar boundaries.

The calendar provides a visual representation of time allocation beyond numerical productivity summaries.

### 📊 Productivity Data

The Data view provides historical productivity information derived from recorded sessions.

- Review accumulated productivity statistics.
- Examine focused work across different reporting periods.
- Calculate productivity using recorded duration and priority snapshots.
- Support historical comparisons and future trend analysis.

The application is being developed toward more comprehensive productivity visualization and trend reporting.

### 🔊 Custom Audio Feedback

The application includes configurable sound effects for important timer events.

- **Completion sound:** `fantastical_app_arp.wav`
- **Logging sound:** `log_sound_effect.wav`
- Independent enable/disable settings.
- Independent volume controls.
- Squared volume scaling for finer adjustment at lower levels.
- Audio playback through Qt's `QSoundEffect`.
- Sound assets loaded through the Qt resource system.

The logging sound uses a custom retro digital/typewriter-inspired effect, providing a distinct auditory confirmation when recording sessions.

### ⚙️ Settings and Persistence

- Local SQLite database for persistent application data.
- Qt settings support for application preferences.
- Configurable audio feedback.
- Database operations separated from interface logic through repository-based persistence.
- Session timestamps stored in UTC for consistent time comparisons.
- Structured task, archetype, and session data.

The application is designed to retain productivity history locally without requiring an external service.

## Application Structure

The interface is organized into five primary views:

| View | Responsibility |
|---|---|
| Timer | Active focus sessions and task selection |
| Data | Productivity summaries and historical statistics |
| Log | Manual session recording |
| Scheduled | Future work and scheduled task management |
| Settings | Application preferences and configuration |

## Technology Stack

- **Language:** C++20
- **GUI Framework:** Qt 6 Widgets
- **Database:** SQLite
- **Database Integration:** Qt SQL
- **Build System:** CMake
- **Audio:** Qt Multimedia / QSoundEffect
- **Development Environment:** CLion
- **Primary Platform:** Windows (MinGW)

## Architecture and Design Principles

The project emphasizes modularity and separation of concerns.

- **Local-first architecture:** Productivity records are stored locally using SQLite.
- **Repository-based persistence:** Database operations are separated from presentation logic.
- **Separation of concerns:** Timer behavior, interface components, persistence, and reusable logic are maintained as distinct responsibilities.
- **Historical consistency:** Priority snapshots and UTC timestamps preserve the context of recorded sessions.
- **Reusable components:** Application logic is structured with future platform expansion in mind.
- **Incremental development:** Features are introduced while maintaining existing application behavior.

## Development Roadmap

Potential areas of continued development include:

- Expanded productivity trend visualization.
- More comprehensive historical comparisons.
- Session overlap detection and conflict warnings.
- Expanded calendar and scheduling functionality.
- Category management and archival workflows.
- Improved task and session organization.
- Android implementation using Kotlin and Jetpack Compose.
- Greater reuse of application-domain logic across platforms.

## Project Status

The Productivity Timer has evolved from a basic countdown application into a multi-view productivity tracking system.

It currently incorporates persistent task management, scheduled work, manual session logging, configurable audio feedback, historical session review, and priority-weighted productivity calculations.

Development continues with an emphasis on improving the application's analytical capabilities, expanding task management, and establishing a foundation for cross-platform development.

The long-term objective is to create a practical productivity system that not only records focused work but also helps users understand how their time is allocated and make better decisions about future activities.