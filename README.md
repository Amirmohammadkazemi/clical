# clical – Command-Line Calendar (Scheduler)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![Linux](https://img.shields.io/badge/Linux-FCC624?logo=linux&logoColor=black)](https://www.linux.org/)

**clical** (CLI Calendar) is a powerful, feature-rich command-line calendar and Scheduler application written in modern C++17. It provides full CRUD operations, recurring events, persistent storage, and a terminal-like interactive interface with readline support.

---

## Features

- 📅 **Full CRUD** – Add, list, edit, and remove events
- 🔁 **Recurring Events** – Daily, weekly, and monthly patterns with optional end dates
- ⚡ **Smart Conflict Detection** – Warns only when start and end times exactly match an existing event
- 💾 **Persistent Storage** – Auto-saves to JSON and restores on startup
- 🎨 **Colorized Output** – Clear visual feedback for errors, warnings, and success
- 🖥️ **Readline Integration** – Arrow keys, command history, copy/paste, and line editing
- 🔍 **Search & Filter** – Search by title, list by date or date range
- 🧪 **Comprehensive Tests** – 58+ unit tests with Google Test

---

## Installation

### Option 1: Quick Install (Recommended)

```bash
git clone https://github.com/Amirmohammadkazemi/clical.git
cd clical
chmod +x install.sh
./install.sh
```

This script automatically detects your Linux distribution, installs dependencies, and builds/installs `clical`.

### Option 2: Download Pre-built Package

Download the appropriate package from the [Releases](https://github.com/Amirmohammadkazemi/clical/releases) page:

| Distribution | Package | Install Command |
|--------------|---------|-----------------|
| Debian/Ubuntu | `clical-*.deb` | `sudo dpkg -i clical-*.deb` |
| Fedora/RHEL   | `clical-*.rpm` | `sudo rpm -i clical-*.rpm` |
| Other Linux   | `clical-*.tar.gz` | Extract and `sudo make install` |

> **Note:** Dependencies (`readline` and `nlohmann-json`) are automatically resolved on Debian/Ubuntu and Fedora/RHEL.

#### Option 2-1: You can build packages
```bash
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
make
make package
```

### Option 3: Manual Build from Source

```bash
git clone https://github.com/Amirmohammadkazemi/clical.git
cd clical
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
make
sudo make install
```

---

## Usage

After installation, run:

```bash
clical
```

You'll see a prompt (`> `) where you can type commands. Use arrow keys to navigate command history, and Ctrl+Shift+C/V for copy/paste.

### Available Commands

> **Both uppercase and lowercase letters can be used for commands (not flags).**

| Command | Description |
|---------|-------------|
| `ADD --title "Title" --start-date YYYY-MM-DD --start-time HH:MM --end-date YYYY-MM-DD --end-time HH:MM` | Add a new event (normal or recurring) |
| `--recurrence daily/weekly/monthly --interval N --recurrence-end YYYY-MM-DD` | Option for ADD command|
| `LIST` | List all events sorted by start time |
| `LIST --date YYYY-MM-DD` | List events on a specific date |
| `LIST --range "YYYY-MM-DD YYYY-MM-DD"` | List events within a date range |
| `REMOVE <number>` | Remove event by list number |
| `REMOVE --title "Title"` | Remove all events with given title |
| `REMOVE --range "YYYY-MM-DD YYYY-MM-DD"` | Remove events within a date range |
| `EDIT <index> --title "New Title" --start-date ...` | Edit an existing event |
| `SEARCH "Title"` | Find event indices by title |
| `HELP` | Show available commands |
| `EXIT` or `quit` | Exit the program |

### Example Session

```bash
$ clical
> ADD --title "Team Meeting" --start-date 2025-12-25 --start-time 10:00 --end-date 2025-12-25 --end-time 11:00
Event 'Team Meeting' added successfully.

> ADD --title "Daily Standup" --start-date 2025-12-25 --start-time 09:00 --end-date 2025-12-25 --end-time 09:30 --recurrence daily --interval 1 --recurrence-end 2026-01-01
Event 'Daily Standup' added successfully.

> LIST
1. Title: Daily Standup
   Start: 2025-12-25 09:00
   End:   2025-12-25 09:30
   Recurrence: Daily (every 1) until 2026-01-01

2. Title: Team Meeting
   Start: 2025-12-25 10:00
   End:   2025-12-25 11:00

> SEARCH "Meeting"
Events with title 'Meeting' found at indices: 2

> REMOVE 1
Event 'Daily Standup' removed.

> EXIT
Goodbye!
```

---

## Storage

All events are saved to `.config/clical/calendar_data.json`

If you did not install it on the system and just compiled it `./build/calendar_data.json`

The file is human-readable and can be manually edited:

```json
[
    {
        "title": "Team Meeting",
        "start": "2025-12-25 10:00",
        "end": "2025-12-25 11:00",
        "recurrence": "none"
    },
    {
        "title": "Daily Standup",
        "start": "2025-12-25 09:00",
        "end": "2025-12-25 09:30",
        "recurrence": "daily",
        "interval": 1,
        "recurrenceEnd": "2026-01-01 00:00"
    }
]
```

---

## Testing

We use Google Test for unit testing. To run tests:

```bash
cd build
make
./clical_tests
```

Generate XML/JSON test reports:

```bash
./clical_tests --gtest_output=xml:test_results.xml
```

---

## Building from Source

For developers who want to build from source:

```bash
git clone https://github.com/Amirmohammadkazemi/clical.git
cd clical
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
make
```

### Build Packages (DEB, RPM, TGZ)

After building, you can create installation packages:

```bash
cpack -G RPM
```

This generates `clical-*.deb`, `clical-*.rpm`, and `clical-*.tar.gz` in the `build` directory.

---

## Contributing

We welcome contributions! Please follow these guidelines:

1. **Fork** the repository and create a feature branch.
2. **Write unit tests** for any new feature or bug fix – we require test coverage for all changes.
3. **Ensure all existing tests pass** before submitting a pull request.
4. Follow the existing code style (C++17, consistent indentation, meaningful variable names).
5. Update the README if you add or change a feature.

### Testing Requirements

- New features **must** include corresponding unit tests.
- Bug fixes **must** include a test that reproduces the bug.
- All tests must pass in the CI environment (we use GitHub Actions).

---

## License

This project is licensed under the MIT License – see the [LICENSE](LICENSE) file for details.

---

## Author

**Amirmohammad** – [GitHub](https://github.com/Amirmohammadkazemi)

---

## Acknowledgments

- [Readline Library](https://tiswww.case.edu/php/chet/readline/rltop.html) – Terminal input handling
- [nlohmann/json](https://github.com/nlohmann/json) – JSON serialization
- [Google Test](https://github.com/google/googletest) – Unit testing framework

---

**Enjoy organizing your time with clical!** 🗓️💻
```
