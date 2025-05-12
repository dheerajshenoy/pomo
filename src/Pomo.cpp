#include "Pomo.hpp"

Pomo::Pomo()
{
    m_timer_font = m_timer_label->font();
    m_state_font = m_state_label->font();
    initConfiguration();
    initAudioEngine();
    initPomodoro();
    initGui();
    initKeybinds();

    connect(&m_timer, &QTimer::timeout, this, &Pomo::updateCountdown);
    m_timer.setInterval(1000);
}

void Pomo::initConfiguration() noexcept
{
    QString xdg_config = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString config_file_path = QDir(xdg_config).filePath("pomo/config.toml");
    QFile config_file(config_file_path);

    if(!config_file.exists())
    {
        return;
    }

    auto config = toml::parse_file(config_file_path.toStdString());

    auto font_name = config["time"]["font"].value_or("Noto Sans");
    auto font_size = config["time"]["font-size"].value_or(80);
    auto bold = config["time"]["bold"].value_or(true);
    auto italic = config["time"]["italic"].value_or(false);
    auto color = config["time"]["color"].value_or("#000000");
    m_timer_font.setFamily(QString::fromStdString(font_name));
    m_timer_font.setPixelSize(font_size);
    m_timer_font.setBold(bold);
    m_timer_font.setItalic(italic);
    auto palette = m_timer_label->palette();
    palette.setColor(QPalette::WindowText, color);
    m_timer_label->setPalette(palette);

    auto state_font_name = config["state"]["font"].value_or("Noto Sans");
    auto state_font_size = config["state"]["font-size"].value_or(80);
    auto state_bold = config["state"]["bold"].value_or(true);
    auto state_italic = config["state"]["italic"].value_or(false);
    auto state_color = config["state"]["color"].value_or("#000000");
    m_state_font.setFamily(QString::fromStdString(state_font_name));
    m_state_font.setPixelSize(state_font_size);
    m_state_font.setBold(state_bold);
    m_state_font.setItalic(state_italic);
    auto state_palette = m_timer_label->palette();
    state_palette.setColor(QPalette::WindowText, color);
    m_timer_label->setPalette(state_palette);

    m_hide_hour = config["general"]["hide-hour"].value_or(true);

    m_focus_time = config["pomodoro"]["focus"].value_or(25 * 60); // 25 min
    m_short_break_time = config["pomodoro"]["short-break"].value_or(5 * 60); // 5 min
    m_long_break_time = config["pomodoro"]["long-break"].value_or(15 * 60); // 15 min
    m_show_notif = config["pomodoro"]["notification"].value_or(true);
    auto cmd = config["pomodoro"]["notify-cmd"].value<std::string>();

    if (cmd.has_value())
    {
        m_notify_cmd = cmd.value();
    }

    auto ding_file = config["pomodoro"]["ding-file"].value<std::string>();
    if (ding_file.has_value())
    {
        m_audio_file = ding_file.value();
        m_has_audio = true;
    }

    m_confirm_on_exit = config["pomodoro"]["confirm-on-exit"].value_or(true);

}

void Pomo::initGui() noexcept
{
    QWidget *widget = new QWidget();
    widget->setLayout(m_layout);
    m_layout->addWidget(m_timer_label);

    m_state_label->setAlignment(Qt::AlignCenter);
    m_layout->addWidget(m_state_label);
    this->setCentralWidget(widget);

    m_timer_label->setFont(m_timer_font);
    m_state_label->setFont(m_state_font);

    m_layout->setAlignment(Qt::AlignCenter);

}

void Pomo::initKeybinds() noexcept
{
    QShortcut *st__toggle_timer = new QShortcut(QKeySequence("space"),
                                                this);

    connect(st__toggle_timer, &QShortcut::activated,
            this, &Pomo::toggleTimer);

}

void Pomo::toggleTimer() noexcept
{
    m_timer_is_active = !m_timer_is_active;

    if (m_timer_is_active)
    {
        m_timer.start();
    } else {
        m_timer.stop();
    }
}

void Pomo::updateCountdown() noexcept
{
    if (m_totalSeconds > 0)
    {
        int h = m_totalSeconds / 3600;
        int m = (m_totalSeconds % 3600) / 60;
        int s = m_totalSeconds % 60;

        if (h > 0 || !m_hide_hour)
        {
            m_timer_label->setText(QString("%1:%2:%3")
                                   .arg(h, 2, 10, QChar('0'))
                                   .arg(m, 2, 10, QChar('0'))
                                   .arg(s, 2, 10, QChar('0')));
        } else {
            m_timer_label->setText(QString("%1:%2")
                                   .arg(m, 2, 10, QChar('0'))
                                   .arg(s, 2, 10, QChar('0')));
        }
        m_totalSeconds--;
    } else {
        m_timer.stop();
        playSound();
        showNotification();
        m_timer_label->setText("00:00");
    }
}

// Initialize audio engine
void Pomo::initAudioEngine() noexcept
{
    if (!m_has_audio)
        return;

    m_audio_result = ma_engine_init(NULL, &m_audio_engine);
    if (m_audio_result != MA_SUCCESS) {
        qCritical("Failed to initialize engine\n");
        return;
    }

}

// Play audio file
void Pomo::playSound() noexcept
{
    if (!m_has_audio)
        return;

    m_audio_result = ma_engine_play_sound(&m_audio_engine, m_audio_file.c_str(), NULL);
    if (m_audio_result != MA_SUCCESS) {
        qWarning("Failed to play sound\n");
        return;
    }
}

void Pomo::showNotification() noexcept
{
    if (!m_show_notif)
        return;

    system(m_notify_cmd.c_str());
}

int Pomo::parse_duration(const std::string& input) noexcept
{
    std::regex re(R"((\d+)([hms]))");
    std::sregex_iterator it(input.begin(), input.end(), re), end;

    int total_seconds = 0;

    while (it != end) {
        int value = std::stoi((*it)[1].str());
        char unit = (*it)[2].str()[0];

        switch (unit) {
            case 'h': total_seconds += value * 3600; break;
            case 'm': total_seconds += value * 60; break;
            case 's': total_seconds += value; break;
            default: break; // skip unknown units
        }
        ++it;
    }

    return total_seconds;
}


Pomo::~Pomo()
{
    if (m_has_audio)
    {
        ma_engine_uninit(&m_audio_engine);
    }
}

