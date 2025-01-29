#ifndef REGISTRFORM_H
#define REGISTRFORM_H

#include <QWidget>
#include "server.h"

namespace Ui {
class Registrform;
}

class Registrform : public QWidget
{
    Q_OBJECT

public:
    explicit Registrform(QWidget *parent = nullptr);
    ~Registrform();
    void setServer(const std::shared_ptr<Server>& server);
    QString get_User();

signals:
    void req_loginPage();
    void rejected();
    void accepted();

private slots:
    void on_pushButton_goLoginPage_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_lineEdit_login_editingFinished();

private:
    void set_stiles();

private:
    Ui::Registrform *ui;
    std::shared_ptr<Server> server;
    QString current_user;
};

#endif // REGISTRFORM_H
