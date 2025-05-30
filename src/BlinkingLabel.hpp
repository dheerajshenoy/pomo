#pragma once

#include <QLabel>
#include <QTimer>

class BlinkingLabel : public QLabel
{
public:
    BlinkingLabel(const QString &text, QWidget *parent = nullptr) : QLabel(text, parent)
    {
        connect(&m_blink_timer, &QTimer::timeout, this, &BlinkingLabel::toggleVisibility);
        m_blink_timer.setInterval(500);
    }

    void startBlinking() noexcept
    {
        m_blink_timer.start();
    }

    void toggleVisibility() noexcept
    {
        m_isBlinkShown   = !m_isBlinkShown;
        QPalette palette = this->palette();
        QColor color     = palette.color(QPalette::WindowText);
        color.setAlpha(m_isBlinkShown ? 255 : 0);
        palette.setColor(QPalette::WindowText, color);
        this->setPalette(palette);
    }

    void setBlinkInterval(int msec) noexcept
    {
        m_blink_timer.setInterval(msec);
    }

    void stopBlinking() noexcept
    {
        if (m_blink_timer.isActive())
            m_blink_timer.stop();
        QPalette palette = this->palette();
        QColor color     = palette.color(QPalette::WindowText);
        color.setAlpha(255);
        palette.setColor(QPalette::WindowText, color);
        this->setPalette(palette);
    }

private:
    QTimer m_blink_timer;
    bool m_isBlinkShown{true};
};
