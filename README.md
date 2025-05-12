# pomo

A simple and beautiful pomodoro app.

## Installation

Install by compiling from source. You just need the following dependency: ``Qt6``

Compile the source code by running the following commands:

```
cd pomo &&
mkdir build &&
cd build &&
cmake -DCMAKE_INSTALL_TYPE=Release &&
make -j$(nproc) &&
sudo make install
```

## Features

- Configurable through TOML

## Configuration

Pomo uses TOML for configuration. Pomo tries to read the file ``~/.config/pomo/config.toml`` and loads
it if it exists.

Example configuration:

```toml
[pomodoro]

focus = "25m"
short-break = "5m"
long-break = "15m"
ding-file = "/home/neo/Downloads/ding.mp3"
notification = true
notify-command = "notify-send dd"

[time]

font = "Roboto Mono"
font-size = 80
bold = true
italic = false
color = "#FF5000"
hide-hour = true

[state]

font = "Roboto Mono"
font-size = 25
bold = true
italic = true
color = "#888888"

# [keybindings]
# toggle-timer = "space"
```
