#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include "server.h"

namespace Ui {
class Loginform;
}

class Loginform : public QWidget
{
    Q_OBJECT

public:
    explicit Loginform(QWidget *parent = nullptr);
    ~Loginform();
    void setServer(const std::shared_ptr<Server>& server);
    QString get_User();

signals:
    void req_registrPage();
    void rejected();
    void accepted();

private slots:
    void on_pushButton_goRegistrForm_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void on_lineEdit_login_selectionChanged();

private:
    void set_stiles();

private:
    Ui::Loginform *ui;
    std::shared_ptr<Server> server;
    QString current_user;
};

#endif // LOGINFORM_H
