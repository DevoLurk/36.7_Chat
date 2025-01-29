#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QPainter>

#include <thread>
#include <atomic>

#include "server.h"
#include "database.h"
#include "logger.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const std::shared_ptr<Server>& server, const std::shared_ptr<Database>& database, QWidget *parent = nullptr);
    ~MainWindow();
    static MainWindow* init();

private slots:
    void on_button_clListen_clicked();
    void on_button_findUser_clicked();
    void on_button_banUser_clicked();
    void on_button_showBanU_clicked();
    void on_button_showAllU_clicked();

private:
    Ui::MainWindow *ui;
    std::shared_ptr<Server> server;
    std::shared_ptr<Database> database;
    std::shared_ptr<Logger> logger;
    QString user_to_find;
    QList<QString> baned_users;
    int size_px_clConn = 25;
    std::atomic<bool> is_listeningn = false;
    std::atomic<bool> is_connected = false; // working but not used
    std::thread worker_thread;

private:
    void set_styles();
    void start_func();
    QString getTimeStamp();
    QPixmap createCirclePixmap(int size, QColor color);
    std::thread th_Uconn;
};
#endif // MAINWINDOW_H
