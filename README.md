# pomo

A simple and beautiful pomodoro app.

![This is how the UI looks](./demo.png)

## Installation

Install by compiling from source. You just need the following dependency: ``Qt6``

Compile the source code by running the following commands:

```bash
cd pomo &&
mkdir build &&
cd build &&
cmake -DCMAKE_INSTALL_TYPE=Release &&
make -j$(nproc) &&
sudo make install
```

## Features

- Configurable through TOML
- Notification on task complete

## Configuration

Pomo uses TOML for configuration. Pomo tries to read the file ``~/.config/pomo/config.toml`` and loads
it if it exists.

Example configuration:

```toml

[pomodoro]

# Duration (format: "25m10s" for 25 minutes and 10 seconds. Works with commas too! (25m, 10s))
focus = "25m"
short-break = "5m"
long-break = "15m"

# Testing
# focus = "2s"
# short-break = "2s"
# long-break = "10s"

# Path to an audio file that plays when state changes
ding-file = "/home/neo/Downloads/ding.mp3"

# Enable/Disable notifications
notification = true

# Notification command run when state changes (this works only if notification is enabled)
notify-command = "notify-send 'Pomo' '{state}'"

# Ask for confirmation on exit (prevents accidental closing of window)
confirm-on-exit = true

# Number of tasks after which long break is started
long-break-after-tasks = 4

[time]

font = "Roboto Mono"
font-size = 80
bold = true
italic = false
color = "#FF5000"

# Hides the hour if it is 0
hide-hour = true

# This shows the current pomodoro state
[state]

shown = true
font = "Roboto Mono"
font-size = 25
bold = true
italic = false
color = "#888888"

# This shows the remaining tasks before long break
[remaining]

shown = true
font = "JetBrainsMono Nerd Font Mono"
font-size = 15
bold = true
# color = "#888888"

[keybindings]

toggle = "space"
reset = "r"
```
