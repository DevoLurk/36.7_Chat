#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>

class MessageWidget : public QWidget {
    Q_OBJECT
public:
    explicit MessageWidget(const QString& text, const bool& is_backgtound = false, const bool& is_left = false,  QWidget* parent = nullptr);
};

#endif // MESSAGEWIDGET_H
