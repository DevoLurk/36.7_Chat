#include "loginform.h"
#include "ui_loginform.h"

Loginform::Loginform(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Loginform)
{
    ui->setupUi(this);
    set_stiles();
}

Loginform::~Loginform()
{
    delete ui;
}

void Loginform::setServer(const std::shared_ptr<Server>& server)
{
    this->server = server;
}

QString Loginform::get_User()
{
    return this->current_user;
}

void Loginform::set_stiles()
{
    QString lables_stile = "color: white; font-weight: bold;  padding: 3px; ";
    QString bottoms_stile = R"( QPushButton{
                                    color: white;
                                    font-weight: bold;
                                    font-family: Cascadia Code;
                                    background-color:rgb(31, 36, 77);
                                    border: 1px solid black;
                                    border-radius: 3px;
                                    padding:6px 26px;
                                }
                                QPushButton:hover {
                                    background-color: rgb(38, 45, 94);
                                }
                                 QPushButton:pressed {
                                    background-color:rgb(37, 37, 37);
                                })";

    QString line_stile = R"(QLineEdit {
                                color: black;
                                font-family: Microsoft YaHei;
                                font-size: 13px;
                                background-color:rgb(242, 242, 242);
                                border-width: 1px;
                                border-style: solid;
                                border-color:rgb(153, 153, 255);
                                border-radius: 2px;
                                padding: 2px 10px;
                            }
                            QLineEdit:hover {
                                    background-color:rgb(194, 198, 252);
                                }
                            QLineEdit:focus {
                                    background-color:rgb(242, 242, 242);
                                })";

    ui->label_login->setStyleSheet(lables_stile);
    ui->label_pass->setStyleSheet(lables_stile);

    ui->pushButton_goRegistrForm->setStyleSheet(bottoms_stile);
    ui->buttonBox->setStyleSheet(bottoms_stile);

    ui->lineEdit_login->setStyleSheet(line_stile);
    ui->lineEdit_pass->setStyleSheet(line_stile);

    ui->lineEdit_pass->setEchoMode(QLineEdit::EchoMode::Password);
}

void Loginform::on_lineEdit_login_selectionChanged()
{
    ui->lineEdit_login->clear();
}

void Loginform::on_pushButton_goRegistrForm_clicked()
{
    emit req_registrPage();
}

void Loginform::on_buttonBox_accepted()
{
    QString login = ui->lineEdit_login->text();
    QString pass = ui->lineEdit_pass->text();
    std::string pass_salt = pass.toStdString();

    if(login.isEmpty() || pass.isEmpty())
    {
        ui->lineEdit_login->clear();
        ui->lineEdit_login->setText("Login or Passwod is Empty");
        return;
    }

    if(server->userExists(login.toStdWString()))
    {
        my_add_salt(pass_salt);
        Hash hash = sha1((char*)pass_salt.c_str(), pass_salt.size());

        if(this->server->checkPass(login.toStdWString(), hash))
        {
            current_user = login;
            emit accepted();
        }
        else
            ui->lineEdit_login->setText("Incorrect password");
    }
    else
    {
        ui->lineEdit_login->setText("User not founded");
        ui->lineEdit_pass->clear();
    }
}

void Loginform::on_buttonBox_rejected()
{
    emit rejected();
}

