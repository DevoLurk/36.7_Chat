#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "startscreen.h"

MainWindow::MainWindow(const std::shared_ptr<Server>& server, const QString& user, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->server = server;
    this->user = user;

    auto pri_w = qobject_cast<Private_chat*>(ui->stackedWidget->widget(1));
    auto pub_w = qobject_cast<Publick_chat*>(ui->stackedWidget->widget(0));

    if (pri_w)
    {
        pri_w->setServer(this->server);
        pri_w->setUser(this->user);
    }

    if (pub_w)
    {
        pub_w->setServer(this->server);
        pub_w->setUser(this->user);
        pub_w->preload_data();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow *MainWindow::init()
{
    Startscreen scr;
    auto result = scr.exec();

    if(result == QDialog::Rejected)
        return 0;

    auto w = new MainWindow(scr.get_server(), scr.get_user());
    w->setAttribute(Qt::WA_DeleteOnClose);
    return w;
}

void MainWindow::on_actionPrivate_triggered()
{
    ui->stackedWidget->setCurrentIndex(1);
    auto pri_w = qobject_cast<Private_chat*>(ui->stackedWidget->widget(1));
    pri_w->preload_data();
}

void MainWindow::on_actionPublic_triggered()
{
    ui->stackedWidget->setCurrentIndex(0);
    auto pub_w = qobject_cast<Publick_chat*>(ui->stackedWidget->widget(0));
    pub_w->preload_data();
}


