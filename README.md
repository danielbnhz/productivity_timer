# Productivity Timer

A C++20 / Qt 6 productivity timer for tracking focused work and understanding how time is spent.

The project starts as a local-first desktop application, with an eventual Android version and optional remote backups. Its priorities reflect the developer’s own business, career, and maintenance needs—not universal judgments about the value of activities.

## Status and goals

### Verified functionality

The desktop countdown works, and the SQLite database structure has been created and inspected.

- Display a countdown, normally configured for 25 minutes.
- Start, pause, resume, and reset the countdown.
- Exclude paused time from the countdown.
- Stop at zero and display session completion.
- Start a new countdown after completion.
- Keep the interface responsive while running.
- Calculate remaining time from elapsed-time measurements rather than counting refresh callbacks.
- Detect and load the `QSQLITE` driver.
- Open an application-owned SQLite database.
- Create the `task_archetypes`, `tasks`, and `sessions` tables.
- Read and report the number of saved sessions.

The countdown was tested with a five-second duration. The database was inspected through the SQLite command-line shell, confirming all three tables and the session schema.

### Not implemented yet

- Creating and editing archetypes and tasks through repository methods or the interface.
- Selecting a task for a timed session.
- Saving completed sessions.
- Displaying session history or focused-time totals.
- Scheduling behavior, priority-based task lists, or reminders.
- Duration-dependent productivity scoring.
- Recovery of unfinished sessions after closing or crashing.
- Android deployment and background completion.
- Remote backups.

**A completed countdown currently does not create a database record.** The timer and database initialization work, but session insertion has not been connected.

Recorded focus time measures time spent. It does not prove work quality, output, or attention.

### Next milestone

Create an archetype and a task, select that task, and save one completed five-second session.

The implementation must:

1. Capture the timestamp when a new session begins.
2. Preserve that timestamp through pause/resume.
3. Capture the task’s effective priority at session start.
4. Save the completed session through the repository.
5. Report save success or the actual database error.
6. Verify that the record persists after restarting the app.

For the initial accounting policy:

- Paused time does not count.
- Reset discards the unfinished session.
- Closing the app discards the unfinished session.
- Interrupted-session recovery is deferred.

## Design and priorities

### Responsibilities

- **Timer model:** countdown state, duration, pause/resume behavior, and remaining-time calculations.
- **View:** display and controls; no ownership of countdown rules or SQL.
- **Controller/integration:** currently the connections in `main.cpp`; coordinates user requests and model updates.
- **Persistence:** `SessionRepository` opens the database, initializes the schema, and queries records.
- **Backup integration:** future optional export/upload behavior, separate from timer execution.

The application uses an MVC-inspired separation without requiring a separate controller class yet.

Current countdown flow:

```text
Button click
    → view emits a request
    → application connection invokes a model method
    → model updates state
    → model emits an update
    → view refreshes the display
```

C++ is chosen for hands-on development, explicit control, and a reusable native backend. It does not by itself guarantee timing accuracy or hard real-time execution.

`QElapsedTimer` measures elapsed time. `QTimer` schedules interface refreshes. Callback count is not the authoritative elapsed-time measurement.

System sleep, mobile suspension, process termination, and persistent timer recovery require additional design and testing.

### Priority scale

The current implementation uses integer priorities from **0 through 4**:

| Value | Meaning |
|---|---|
| 0 | Leisure / nonproductive by default |
| 1 | Low priority |
| 2 | Normal priority |
| 3 | High priority |
| 4 | Emergency / needs attention ASAP |

These classifications reflect Daniel’s personal business, career, and maintenance priorities. They are not universal judgments about the worth of activities, professions, or people.

Intended examples—not automatically inserted database records:

- Music: default priority 1.
- Programming: default priority 2.
- Exercise: default priority 2.
- An assignment approaching its deadline: task override 3.
- An emergency: task override 4.
- A paid music commission: explicit task override when appropriate.

Each task belongs to one archetype and may override its default priority:

```text
Effective priority =
    task override, when present
    otherwise archetype default
```

An explicit override of 0 is valid. An absent override is represented by `NULL`.

Scheduling a task does not automatically increase its priority.

Sessions store an effective-priority snapshot so later priority changes do not silently reclassify previously recorded time.

Priority-based ranking and snapshot capture are planned behavior; the schema supports them, but application integration is not implemented yet.

### Deferred scoring

Priority determines scheduling importance. It is not automatically a multiplier for productive minutes.

Future reporting may apply configurable duration-dependent scoring—for example, diminishing returns or a peaked curve for exercise and chores.

These formulas are deferred until task creation and session recording work. Actual durations should remain available alongside any calculated score.

A proposed chores priority of 2.5 is **not supported by the current integer scale**. Fractional priorities or separate scoring weights require an explicit design and schema change.

Programming is not assumed to provide unlimited real-world benefit simply because more time is recorded.

## Database and source layout

### Database structure

```text
Task archetype
    → many tasks
        → many recorded sessions
```

| Table | Main fields |
|---|---|
| `task_archetypes` | Name, default priority, archived flag |
| `tasks` | Archetype reference, title, optional priority override, optional scheduled timestamp, status, creation timestamp |
| `sessions` | Task reference, start/end timestamps, planned duration, focused duration, priority snapshot |

Durations are stored as integer milliseconds. Minutes are calculated for display.

Timestamps are intended to be UTC text values using a consistent format when insertion is implemented.

The repository enables and verifies SQLite foreign-key enforcement before creating tables. Table creation is grouped in a transaction.

Archiving is preferred to deleting tasks and archetypes that have historical records. Foreign keys restrict deletion of referenced records.

### Database location

The application uses:

```cpp
QStandardPaths::AppLocalDataLocation
```

with:

```text
Organization: DanielHernandez
Application: ProductivityTimer
Database: productivity-v1.sqlite
```

The exact database path is printed at startup. It is outside the source and build directories.

Keep the organization and application names consistent once real records are stored.

The `productivity-v1.sqlite` filename separates the current three-table design from the earlier experimental `productivity.sqlite` database. The earlier file is not deleted or migrated.

`CREATE TABLE IF NOT EXISTS` does not update an existing table definition. Future schema changes require migrations or an explicitly chosen reset of disposable development data.

### Source layout

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
  persistence/
    session_repository.h
    session_repository.cpp
```

`main.cpp` creates the application, initializes the repository, constructs the timer and view, connects their signals and methods, and starts the event loop.

## Build and verification

### Requirements

- CMake 3.30 or newer.
- A C++20-capable compiler compatible with the installed Qt kit.
- Qt 6 Core, Gui, Widgets, and Sql.
- The `QSQLITE` driver plugin.

The currently tested environment is Windows with CLion, a MinGW toolchain, and Qt 6.11.2.

Current local build directory:

```text
G:\Program_File_Folder\cplusplus\productivity_timer\cmake-build-debug
```

This is a developer-specific path, not a required project location.

### Build

After changing `CMakeLists.txt`, reload CMake in CLion.

Example commands from the repository root:

```sh
cmake -S . -B build -DCMAKE_PREFIX_PATH="<path-to-your-Qt-kit>"
cmake --build build
```

For a multi-configuration generator:

```sh
cmake --build build --config Debug
```

The existing CLion build profile has been tested. The example commands above must be adapted to the local compiler, generator, and Qt installation.

Current Windows post-build steps copy the required Qt module DLLs, the Windows platform plugin, and the SQLite driver plugin. Deployment outside the development environment is not yet fully validated.

### Countdown test

Temporarily construct:

```cpp
Timer timer(5000);
```

Verify:

1. Initial display is `00:05`.
2. Start begins the countdown.
3. Pause freezes it.
4. Resume continues from the remaining time.
5. Reset restores the full duration.
6. Completion stops at zero.
7. Start after completion begins a new session.

Restore the normal duration afterward:

```cpp
Timer timer(25 * 60 * 1000);
```

The duration is currently configured in code, not through a settings interface.

### Database verification

Startup output should include:

```text
Available SQL drivers: QList("QSQLITE")
SQLite available: true
Database ready: ".../productivity-v1.sqlite"
Saved sessions: 0
```

Zero is expected until session insertion is implemented.

If the standalone SQLite shell is available, inspect the printed path:

```sh
sqlite3 --readonly "<printed-database-path>"
```

Then:

```sql
.tables
.schema sessions
SELECT COUNT(*) FROM sessions;
.quit
```

Expected tables:

```text
sessions  task_archetypes  tasks
```

The standalone SQLite shell is optional; it is separate from the Qt driver used by the application.

## Roadmap and references

### Later milestones

- Archetype and task creation/editing.
- Task selection and completed-session persistence.
- Session history and time totals.
- Priority-ranked task lists.
- Configurable scoring policies with actual time shown separately.
- Schema versioning and migrations.
- Unfinished-session recovery.
- Android interface, deployment, background alarms, and state restoration.
- Optional remote backups and tested restoration.

### Backup direction

The application should remain usable without a website or network connection.

A future backend may live in a separate repository. The timer app would communicate through a documented interface and use a defined backup/export format.

Backing up unrelated small databases remains an open idea, not a committed feature.

Before implementing uploads, define authentication, consent, transport security, backup consistency, format versioning, and restoration. A backup feature is not complete until restoration is tested.

### Documentation

- [QElapsedTimer](https://doc.qt.io/qt-6/qelapsedtimer.html)
- [QTimer](https://doc.qt.io/qt-6/qtimer.html)
- [Qt SQL](https://doc.qt.io/qt-6/qtsql-index.html)
- [QSqlDatabase](https://doc.qt.io/qt-6/qsqldatabase.html)
- [QSqlQuery](https://doc.qt.io/qt-6/qsqlquery.html)
- [QStandardPaths](https://doc.qt.io/qt-6/qstandardpaths.html)
- [Qt for Android](https://doc.qt.io/qt-6/android.html)