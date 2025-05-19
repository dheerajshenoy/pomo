#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

class NotificationWidget : public QWidget
{
    Q_OBJECT
public:
    NotificationWidget(const QString &message, QWidget *parent = nullptr)
    : QWidget(parent)
    {
        // setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip); // No border, floats above

        // setStyleSheet("background-color: #323232; color: white; border-radius: 8px; padding: 10px;");

        // Set unified background and text color
        QString bgColor = "#444";  // Customize your solid color
        QString textColor = "#fff";
        setStyleSheet(QString(R"(
                              QWidget {
                              background-color: %1;
                              color: %2;
                              border-radius: 8px;
                              padding: 10px;
                              }
                              QPushButton {
                              background-color: %1;
                              color: %2;
                              border: none;
                              font-weight: bold;
                              }
                              QPushButton:hover {
                              color: #f88;
                              }
                              )").arg(bgColor, textColor));


        auto *label = new QLabel(message);
        // label->setWordWrap(true);

        auto *closeButton = new QPushButton("Close");
        // closeButton->setStyleSheet("QPushButton { background: transparent; color: white; border: none; font-weight: bold; }"
        //                            "QPushButton:hover { color: red; }");

        connect(closeButton, &QPushButton::clicked, this, &QWidget::close);

        auto *layout = new QHBoxLayout();
        layout->addWidget(label);
        layout->addWidget(closeButton);
        layout->addStretch();
        setLayout(layout);
    }
};

