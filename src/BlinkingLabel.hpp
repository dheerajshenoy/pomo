#pragma once

#include <QLabel>
#include <QTimer>


class BlinkingLabel : public QLabel
{
    public:
    BlinkingLabel(const QString &text, QWidget *parent = nullptr)
    : QLabel(text, parent)
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
        QPalette palette = this->palette();
        QColor color = palette.color(QPalette::WindowText);
        color.setAlpha(m_isBlinkShown ? 0 : 255);
        palette.setColor(QPalette::WindowText, color);
        this->setPalette(palette);
        m_isBlinkShown = !m_isBlinkShown;
    }

    void setBlinkInterval(int msec) noexcept
    {
        m_blink_timer.setInterval(msec);
    }

    void stopBlinking() noexcept
    {
        if (m_blink_timer.isActive())
            m_blink_timer.stop();
    }

private:
    QTimer m_blink_timer;
    bool m_isBlinkShown { true };
};
