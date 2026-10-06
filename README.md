# Productivity Timer

A C++20 / Qt 6 productivity timer for tracking focused work and understanding how time is spent.

The project starts as a desktop application, with an eventual Android version and optional remote backups. The initial priority is a reliable, understandable timer—not a large feature set.

## Status

Early prototype / architecture scaffold. Not yet a working productivity timer.

The current source includes an application entry point, a Qt Widgets view scaffold, and an unfinished timer implementation. The most recent reported commit is `writes some of the core architecture of the timer`.

Known issues in the shared snapshot:

- `Timer_View` is declared in its header and defined again as a class in its implementation file; the implementation should instead define the header's declared member functions.
- The view constructor declared in the header does not have a matching out-of-class implementation in the shared source.
- Buttons and the timer label have no initial text, layout, or connected behavior.
- The timer implementation contains an unfinished blocking loop and an undefined `slowOperation1()` placeholder.
- `timer.h` has not been reviewed, so the complete model interface and build status remain unverified.

Do not treat the current snapshot as a verified build.

## Goals

### First working version

- Display a configurable countdown, initially 25 minutes.
- Start, pause, resume, and reset a session.
- Stop at zero and report completion once.
- Keep the interface responsive while the countdown runs.
- Calculate remaining time from measured elapsed time rather than counting display refreshes.
- Keep timer state and rules separate from presentation.

### Later milestones

- Associate sessions with task names.
- Store completed sessions locally and show focused-time totals.
- Define how stopped, interrupted, and paused sessions contribute to totals.
- Support Android with a mobile-appropriate interface.
- Handle Android background completion, screen-off behavior, and state restoration explicitly.
- Add optional backups to a website/backend service.

Recorded focus time is a measure of time spent, not proof of work quality or output.

## Design

Intended responsibilities:

- **Timer core:** session state, duration, pause/resume behavior, and remaining-time calculations.
- **View:** display and user controls; no ownership of countdown rules.
- **Controller/integration:** connect user commands, timer updates, persistence, and platform behavior as needed.
- **Persistence:** local session records, introduced after the basic timer works.
- **Backup integration:** optional export/upload behavior, separate from timer execution.

C++ is chosen for hands-on systems development, explicit control, and a reusable native backend. It does not by itself guarantee timing accuracy or hard real-time execution.

Qt's `QElapsedTimer` measures elapsed time; `QTimer` can schedule interface refreshes. Refresh callbacks can arrive late, so callback count must not be the authoritative elapsed-time measurement. Mobile suspend/restart behavior needs additional platform-aware design.

Documentation:

- [QElapsedTimer](https://doc.qt.io/qt-6/qelapsedtimer.html)
- [QTimer](https://doc.qt.io/qt-6/qtimer.html)
- [Qt for Android](https://doc.qt.io/qt-6/android.html)

## Source layout

```text
main.cpp
CMakeLists.txt
src/
  model/
    timer.h
    timer.cpp
  view/
    timer_view.h
    timer_view.cpp
```

`main.cpp` constructs the Qt application, shows `Timer_View`, and starts the application event loop.

## Build and run

Current CMake configuration requires:

- CMake 3.30 or newer.
- A C++20-capable compiler compatible with the installed Qt kit.
- Qt 6 Core, Gui, and Widgets.

The previous IDE build directory is unknown. In CLion, inspect the active CMake profile's build-directory setting rather than assuming its location.

Example configuration from the repository root, using a new build directory:

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH="<path-to-your-Qt-kit>"
cmake --build build
```

For a multi-configuration generator, select a configuration when building:

```sh
cmake --build build --config Debug
```

These commands are examples, not a verified build procedure for the current snapshot. Fix the known source issues first. Executable location depends on the generator and build configuration; inspect the build output or IDE run configuration.

The current Windows post-build steps copy selected Qt DLLs and the Windows platform plugin. Their Qt-path detection must match the local installation; deployment has not been validated here.

Android requires a separate Android toolchain and platform configuration. A Windows executable cannot be installed as an Android app.

## Backup roadmap

Local operation is the initial design goal. Remote backup should be optional and should not be required to run a timer.

A future backend may live in a separate repository. The timer app would create a defined backup/export format and communicate with that service through a documented interface.

Whether the service should back up other small databases is an open design question, not a committed feature. Start with this app's session data before designing a general database-backup platform.

Before implementing uploads, define authentication, user consent, transport security, backup consistency, format versioning, and restore behavior. A backup feature is not complete until restoration is tested.

## Immediate milestone

Repair the view/header implementation boundary, then show a static `25:00` label with Start, Pause, and Reset controls. Build and run that screen before adding countdown behavior.

 
 