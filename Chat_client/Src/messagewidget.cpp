#include "messagewidget.h"

MessageWidget::MessageWidget(const QString& text, const bool& is_backgtound, const bool& is_left,  QWidget* parent)
    : QWidget(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    QLabel* messageLabel = new QLabel(text);

        //layout->setStretch(1, 4);

    messageLabel->setWordWrap(true);
    messageLabel->setStyleSheet
        (
        is_backgtound
            ? "background-color: lightgreen; border-radius: 6px; padding: 4px;"
            : "background-color: lightblue; border-radius: 6px; padding: 4px;"
        );

    if (is_left)
    {
        layout->addStretch(1);
        layout->addWidget(messageLabel, 4);
    }
    else
    {
        layout->addWidget(messageLabel, 4);
        layout->addStretch(1);
    }

    //layout->setContentsMargins(10, 5, 10, 5);
    setLayout(layout);
}
