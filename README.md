# pomo

A simple and beautiful pomodoro timer.

![UI Demo](./demo.png)

## Installation

Build from source

```bash
git clone https://github.com/dheerajshenoy/pomo
cd pomo
mkdir build
cd build
cmake -DCMAKE_INSTALL_TYPE=Release ..
make -j$(nproc)
sudo make install
```

## Features

- Clean and minimal interface
- Fully configurable via ``TOML``
- System notifications on state change
- Custom audio alerts

## Configuration

Pomo loads settings from ``~/.config/pomo/config.toml`` if it exists.

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
# {state} in notify-command is replaced with the state name (e.g. "Focus", "Break").
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
