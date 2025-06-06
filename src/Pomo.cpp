#include "Pomo.hpp"

#include <qnamespace.h>
#include <qt6/QtWidgets/qsystemtrayicon.h>

Pomo::Pomo()
{
    m_timer_font     = m_timer_label->font();
    m_state_font     = m_state_label->font();
    m_remaining_font = m_remaining_label->font();
    m_paused_font    = m_paused_label->font();

    initActionMap();
    initConfiguration();
    initAudioEngine();
    initPomodoro();
    initGui();
    updateRemainingLabel();
    connect(&m_timer, &QTimer::timeout, this, &Pomo::updateCountdown);
    m_timer.setInterval(1000);
    m_timer.setTimerType(Qt::TimerType::PreciseTimer);
}

void
Pomo::initActionMap() noexcept
{
    m_action_map["reset"] = [this]()
    {
        resetTimer();
    };
    m_action_map["toggle"] = [this]()
    {
        toggleTimer();
    };
    m_action_map["next-task"] = [this]()
    {
        advanceState();
    };

    m_action_map["next-session"] = [this]()
    {
        nextSession();
    };
}

void
Pomo::initConfiguration() noexcept
{
    QString xdg_config       = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString config_file_path = QDir(xdg_config).filePath("pomo/config.toml");
    QFile config_file(config_file_path);

    if (!config_file.exists())
    {
        return;
    }

    auto config = toml::parse_file(config_file_path.toStdString());

    auto font_name = config["time"]["font"].value_or("Noto Sans");
    auto font_size = config["time"]["font-size"].value_or(80);
    auto bold      = config["time"]["bold"].value_or(true);
    auto italic    = config["time"]["italic"].value_or(false);
    auto color     = config["time"]["color"].value<std::string>();
    m_timer_font.setFamily(QString::fromStdString(font_name));
    m_timer_font.setPixelSize(font_size);
    m_timer_font.setBold(bold);
    m_timer_font.setItalic(italic);
    if (color.has_value())
    {
        auto palette = m_timer_label->palette();
        palette.setColor(QPalette::WindowText, QString::fromStdString(color.value()));
        m_timer_label->setPalette(palette);
    }

    m_state_shown        = config["state"]["shown"].value_or(true);
    auto state_font_name = config["state"]["font"].value_or("Noto Sans");
    auto state_font_size = config["state"]["font-size"].value_or(80);
    auto state_bold      = config["state"]["bold"].value_or(true);
    auto state_italic    = config["state"]["italic"].value_or(false);
    auto state_color     = config["state"]["color"].value<std::string>();
    m_state_font.setFamily(QString::fromStdString(state_font_name));
    m_state_font.setPixelSize(state_font_size);
    m_state_font.setBold(state_bold);
    m_state_font.setItalic(state_italic);

    if (state_color.has_value())
    {
        auto state_palette = m_timer_label->palette();
        state_palette.setColor(QPalette::WindowText, QString::fromStdString(state_color.value()));
        m_timer_label->setPalette(state_palette);
    }

    auto remaining_font_name = config["remaining"]["font"].value_or("Noto Sans");
    auto remaining_font_size = config["remaining"]["font-size"].value_or(80);
    auto remaining_bold      = config["remaining"]["bold"].value_or(true);
    auto remaining_italic    = config["remaining"]["italic"].value_or(false);
    auto remaining_color     = config["remaining"]["color"].value<std::string>();
    m_remaining_font.setFamily(QString::fromStdString(remaining_font_name));
    m_remaining_font.setPixelSize(remaining_font_size);
    m_remaining_font.setBold(remaining_bold);
    m_remaining_font.setItalic(remaining_italic);

    if (remaining_color.has_value())
    {
        auto remaining_palette = m_timer_label->palette();
        remaining_palette.setColor(QPalette::WindowText, QString::fromStdString(remaining_color.value()));
        m_timer_label->setPalette(remaining_palette);
    }

    auto paused_font_name = config["paused"]["font"].value_or("Noto Sans");
    auto paused_font_size = config["paused"]["font-size"].value_or(80);
    auto paused_bold      = config["paused"]["bold"].value_or(true);
    auto paused_italic    = config["paused"]["italic"].value_or(false);
    auto paused_color     = config["paused"]["color"].value<std::string>();
    m_paused_font.setFamily(QString::fromStdString(paused_font_name));
    m_paused_font.setPixelSize(paused_font_size);
    m_paused_font.setBold(paused_bold);
    m_paused_font.setItalic(paused_italic);

    if (paused_color.has_value())
    {
        auto paused_palette = m_paused_label->palette();
        paused_palette.setColor(QPalette::WindowText, QString::fromStdString(paused_color.value()));
        m_paused_label->setPalette(paused_palette);
    }
    m_paused_shown = config["paused"]["shown"].value_or(true);

    m_remaining_shown = config["remaining"]["shown"].value_or(true);

    m_hide_hour = config["general"]["hide-hour"].value_or(true);

    auto focus_time       = config["pomodoro"]["focus"].value_or("25m");
    auto short_break_time = config["pomodoro"]["short-break"].value_or("5m");
    auto long_break_time  = config["pomodoro"]["long-break"].value_or("15m");

    m_state_time_map[PomodoroState::FOCUS]       = parse_duration(focus_time);
    m_state_time_map[PomodoroState::SHORT_BREAK] = parse_duration(short_break_time);
    m_state_time_map[PomodoroState::LONG_BREAK]  = parse_duration(long_break_time);

    m_show_notif = config["pomodoro"]["notification"].value_or(true);
    auto cmd     = config["pomodoro"]["notify-command"].value<std::string>();

    if (cmd.has_value())
    {
        m_notify_cmd = cmd.value();
    }

    auto ding_file = config["pomodoro"]["ding-file"].value<std::string>();
    if (ding_file.has_value())
    {
        m_audio_file = ding_file.value();
        if (QFile::exists(QString::fromStdString(m_audio_file)))
        {
            m_has_audio = true;
        }
    }

    m_confirm_on_exit = config["pomodoro"]["confirm-on-exit"].value_or(true);

    m_pomodoros_before_long_break = config["pomodoro"]["long-break-after-tasks"].value_or(4);

    // Keybindings
    if (config["keybindings"].is_table())
    {
        auto keybindings = config["keybindings"].as_table();

        for (auto &[k, v] : *keybindings)
        {
            auto action = QString::fromStdString(k.str().data());
            auto key    = QString::fromStdString(v.value<std::string>()->data());
            setupKeybindings(key, action);
        }
    }
    else
    {
        // Default keybindings
    }
}

void
Pomo::initGui() noexcept
{
    QWidget *widget = new QWidget();
    widget->setLayout(m_layout);
    this->setCentralWidget(widget);

    if (m_paused_shown)
    {
        m_layout->addWidget(m_paused_label);
    }
    m_layout->addStretch(0);

    m_layout->addWidget(m_timer_label);
    m_paused_label->setVisible(false);

    if (m_state_shown)
    {
        m_layout->addWidget(m_state_label);
    }

    m_layout->addStretch(0);

    if (m_remaining_shown)
    {
        m_layout->addWidget(m_remaining_label);
    }

    m_paused_label->setAlignment(Qt::AlignCenter);
    m_timer_label->setAlignment(Qt::AlignCenter);
    m_state_label->setAlignment(Qt::AlignCenter);
    m_remaining_label->setAlignment(Qt::AlignCenter);
    m_layout->setAlignment(Qt::AlignCenter);

    m_paused_label->setFont(m_paused_font);
    m_timer_label->setFont(m_timer_font);
    m_state_label->setFont(m_state_font);
    m_remaining_label->setFont(m_remaining_font);

    if (!m_has_audio)
    {
        auto *notif = new NotificationWidget("Could not find audio file for bell", this);
        notif->show();
        m_layout->addWidget(notif);
    }
}

void
Pomo::resetTimer() noexcept
{
    m_timer.stop();
    m_pomodoro_count = 0;
}

void
Pomo::toggleTimer() noexcept
{
    m_timer_is_active = !m_timer_is_active;

    if (m_timer_is_active)
    {
        playSound();
        m_timer.start();
        updateStateLabel();
        m_paused_label->stopBlinking();
    }
    else
    {
        m_timer.stop();
        m_paused_label->startBlinking();
    }
    m_paused_label->setVisible(!m_timer_is_active);
}

void
Pomo::updateCountdown() noexcept
{
    if (m_totalSeconds > 0)
    {
        int total = --m_totalSeconds;
        int h     = total / 3600;
        int m     = (total % 3600) / 60;
        int s     = total % 60;

        QString timeStr = QString::asprintf("%02d:%02d:%02d", h, m, s);

        if (h == 0 && m_hide_hour)
        {
            timeStr = timeStr.right(5); // Get "mm:ss"
        }

        m_timer_label->setText(timeStr);
    }
    else
    {
        playSound();
        advanceState();
        showNotification();
    }
}

void
Pomo::setupKeybindings(const QString &key, const QString &action) noexcept
{

    QShortcut *shortcut = new QShortcut(QKeySequence(key), this);

    connect(shortcut, &QShortcut::activated, [this, action]()
    {
        if (m_action_map.find(action.toStdString()) != m_action_map.end())
        {
            m_action_map[action.toStdString()]();
        }
    });
}

// Initialize audio engine
void
Pomo::initAudioEngine() noexcept
{
    if (!m_has_audio)
        return;

    m_audio_result = ma_engine_init(NULL, &m_audio_engine);
    if (m_audio_result != MA_SUCCESS)
    {
        qCritical("Failed to initialize engine\n");
        return;
    }
}

// Play audio file
void
Pomo::playSound() noexcept
{
    if (!m_has_audio)
        return;

    m_audio_result = ma_engine_play_sound(&m_audio_engine, m_audio_file.c_str(), NULL);
    if (m_audio_result != MA_SUCCESS)
    {
        qWarning("Failed to play sound\n");
        return;
    }
}

void
Pomo::showNotification() noexcept
{
    if (!m_show_notif)
        return;

    auto str = replacePlaceholder(m_notify_cmd, "{state}", m_state_str_map[m_current_state]);

    system(str.c_str());
}

std::string
Pomo::replacePlaceholder(std::string input, const std::string &key, const std::string &value) noexcept
{
    size_t pos = input.find(key);
    while (pos != std::string::npos)
    {
        input.replace(pos, key.length(), value);
        pos = input.find(key,
                         pos + value.length()); // Continue search after replacement
    }
    return input;
}

int
Pomo::parse_duration(const std::string &input) noexcept
{
    std::regex re(R"((\d+)([hms]))");
    std::sregex_iterator it(input.begin(), input.end(), re), end;

    int total_seconds = 0;

    while (it != end)
    {
        int value = std::stoi((*it)[1].str());
        char unit = (*it)[2].str()[0];

        switch (unit)
        {
            case 'h':
                total_seconds += value * 3600;
                break;
            case 'm':
                total_seconds += value * 60;
                break;
            case 's':
                total_seconds += value;
                break;
            default:
                break; // skip unknown units
        }
        ++it;
    }

    return total_seconds;
}

void
Pomo::initPomodoro() noexcept
{
    m_totalSeconds = m_state_time_map[m_current_state];
    auto str_secs  = secondsToFlexibleString(m_totalSeconds);
    m_timer_label->setText(QString::fromStdString(str_secs));

    m_state_str_map[PomodoroState::FOCUS]       = "Focus";
    m_state_str_map[PomodoroState::SHORT_BREAK] = "Short Break";
    m_state_str_map[PomodoroState::LONG_BREAK]  = "Long Break";
}

std::string
Pomo::secondsToFlexibleString(const int &totalSeconds) noexcept
{
    int hours   = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;

    std::ostringstream oss;

    if (hours > 0)
    {
        oss << std::setw(2) << std::setfill('0') << hours << ":";
    }

    oss << std::setw(2) << std::setfill('0') << minutes << ":" << std::setw(2) << std::setfill('0') << seconds;

    return oss.str();
}

void
Pomo::closeEvent(QCloseEvent *e)
{
    if (!m_confirm_on_exit)
    {
        e->accept();
        return;
    }

    auto msg = QMessageBox::question(this, "Exiting Pomo", "Do you want to exit pomo ?");

    if (msg == QMessageBox::StandardButton::Yes)
        e->accept();
    else
        e->ignore();
}

void
Pomo::advanceState() noexcept
{
    switch (m_current_state)
    {
        case PomodoroState::FOCUS:
            m_pomodoro_count++;
            if ((m_pomodoro_count = m_pomodoro_count % m_pomodoros_before_long_break) == 0)
                m_current_state = PomodoroState::LONG_BREAK;
            else
                m_current_state = PomodoroState::SHORT_BREAK;
            break;

        case PomodoroState::SHORT_BREAK:
        case PomodoroState::LONG_BREAK:
            m_current_state = PomodoroState::FOCUS;
            break;
    }

    // Reset timer for new state
    m_totalSeconds = m_state_time_map[m_current_state];
    m_timer_label->setText(QString::fromStdString(secondsToFlexibleString(m_totalSeconds)));

    updateStateLabel();
    updateRemainingLabel();
}

void
Pomo::updateStateLabel() noexcept
{
    switch (m_current_state)
    {
        case PomodoroState::FOCUS:
            m_state_label->setText("Focus");
            break;
        case PomodoroState::SHORT_BREAK:
            m_state_label->setText("Short Break");
            break;
        case PomodoroState::LONG_BREAK:
            m_state_label->setText("Long Break");
            break;
    }
}

void
Pomo::updateRemainingLabel() noexcept
{
    m_remaining_label->setText(
        QString("%1 tasks before long break").arg(m_pomodoros_before_long_break - m_pomodoro_count));
}

Pomo::~Pomo()
{
    if (m_has_audio)
    {
        ma_engine_uninit(&m_audio_engine);
    }
}

void
Pomo::nextSession() noexcept
{
    m_current_state               = PomodoroState::LONG_BREAK;
    m_pomodoros_before_long_break = 4;
    m_pomodoro_count              = 0;
    advanceState();
}
