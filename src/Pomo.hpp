#pragma once

#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QWidget>
#include <QVBoxLayout>
#include <QTimer>
#include <QTime>
#include <QShortcut>
#include <QKeySequence>
#include <QDir>
#include <QStandardPaths>
#include <QCloseEvent>
#include <QMessageBox>
#include <map>
#include <regex>
#include <unordered_map>

#include "BlinkingLabel.hpp"
#include "toml.hpp"
#include "miniaudio.h"

class Pomo : public QMainWindow
{

public:
    Pomo();
    ~Pomo();

    void closeEvent(QCloseEvent *e) override;

private:
    enum class PomodoroState
    {
        FOCUS = 0,
        SHORT_BREAK,
        LONG_BREAK
    };

    void initPomodoro() noexcept;
    void initAudioEngine() noexcept;
    void initConfiguration() noexcept;
    void initKeybinds() noexcept;
    void initGui() noexcept;
    void toggleTimer() noexcept;
    void updateCountdown() noexcept;
    void playSound() noexcept;
    void showNotification() noexcept;
    int parse_duration(const std::string& input) noexcept;
    std::string secondsToFlexibleString(const int &totalSeconds) noexcept;

    QFont m_timer_font, m_state_font;
    bool m_timer_is_active { false },
    m_hide_hour,
    m_show_notif,
    m_has_audio,
    m_confirm_on_exit;

    int m_totalSeconds;
    BlinkingLabel *m_timer_label = new BlinkingLabel("00:00");
    QLabel *m_state_label = new QLabel("Focus");

    QVBoxLayout *m_layout = new QVBoxLayout();
    std::string m_audio_file, m_notify_cmd;
    QTimer m_timer;

    // This holds the current state of the pomodoro
    PomodoroState m_current_state = PomodoroState::FOCUS;

    // this holds the time corresponding to each of the state
    std::unordered_map<PomodoroState,int> m_state_time_map;

    ma_result m_audio_result;
    ma_engine m_audio_engine;

};
