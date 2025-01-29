#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include <QDialog>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <QMessageBox>

#include "server.h"

namespace Ui {
class Startscreen;
}

class Startscreen : public QDialog
{
    Q_OBJECT

public slots:
    void onRejectRequested();
    void onAcceptRequested_l();
    void onAcceptRequested_r();

public:
    explicit Startscreen(QWidget *parent = nullptr);
    ~Startscreen();
    void setLoginForm();
    void setRegistrationForm();
    std::shared_ptr<Server> get_server();
    QString get_user();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    Ui::Startscreen *ui;
    bool dragging = false;
    QPoint dragStartPos;
    std::shared_ptr<Server> server;
    QString user;
};

#endif // STARTSCREEN_H
