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
#include <QFile>
#include <map>
#include <regex>
#include <unordered_map>

#include "BlinkingLabel.hpp"
#include "toml.hpp"
#include "miniaudio.h"
#include "Notification.hpp"

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

    void initActionMap() noexcept;
    void resetTimer() noexcept;
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
    void setupKeybindings(const QString &key, const QString &action) noexcept;
    void advanceState() noexcept;
    void updateStateLabel() noexcept;
    void updateRemainingLabel() noexcept;
    std::string replacePlaceholder(std::string input, const std::string& key,
                                   const std::string& value) noexcept;

    QFont m_timer_font, m_state_font, m_remaining_font, m_paused_font;
    bool m_timer_is_active { false },
    m_hide_hour,
    m_show_notif,
    m_has_audio { false },
    m_confirm_on_exit,
    m_state_shown,
    m_remaining_shown,
    m_paused_shown;

    int m_totalSeconds,
    m_pomodoro_count = 0,
    m_pomodoros_before_long_break = 4;

    BlinkingLabel *m_timer_label = new BlinkingLabel("00:00");
    BlinkingLabel *m_state_label = new BlinkingLabel("Start");
    BlinkingLabel *m_remaining_label = new BlinkingLabel("Remaining");
    BlinkingLabel *m_paused_label = new BlinkingLabel("PAUSED");

    QVBoxLayout *m_layout = new QVBoxLayout();
    std::string m_audio_file, m_notify_cmd;
    QTimer m_timer;

    // This holds the current state of the pomodoro
    PomodoroState m_current_state = PomodoroState::FOCUS;

    // this holds the time corresponding to each of the state
    std::unordered_map<PomodoroState, int> m_state_time_map;
    std::unordered_map<PomodoroState, std::string> m_state_str_map;
    std::unordered_map<std::string, std::function<void()>> m_action_map;

    ma_result m_audio_result;
    ma_engine m_audio_engine;


};
