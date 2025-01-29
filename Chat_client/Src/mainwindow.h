#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "server.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const std::shared_ptr<Server>& server, const QString& user, QWidget *parent = nullptr);
    ~MainWindow();
    static MainWindow* init();

private slots:
    void on_actionPrivate_triggered();
    void on_actionPublic_triggered();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<Server> server;
    QString user;
    bool dragging = false;
    QPoint dragStartPos;
};

#endif // MAINWINDOW_H
