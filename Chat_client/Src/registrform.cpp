#include "registrform.h"
#include "ui_registrform.h"

Registrform::Registrform(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Registrform)
{
    ui->setupUi(this);
    set_stiles();
}

Registrform::~Registrform()
{
    delete ui;
}

void Registrform::set_stiles()
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
    ui->label_password->setStyleSheet(lables_stile);
    ui->label_confirmPass->setStyleSheet(lables_stile);

    ui->buttonBox->setStyleSheet(bottoms_stile);
    ui->pushButton_goLoginPage->setStyleSheet(bottoms_stile);

    ui->lineEdit_login->setStyleSheet(line_stile);
    ui->lineEdit_confirmPass->setStyleSheet(line_stile);
    ui->lineEdit_password->setStyleSheet(line_stile);


    ui->lineEdit_password->setEchoMode(QLineEdit::EchoMode::Password);
    ui->lineEdit_confirmPass->setEchoMode(QLineEdit::EchoMode::Password);
}

void Registrform::setServer(const std::shared_ptr<Server>& server)
{
    this->server = server;
}

QString Registrform::get_User()
{
    return this->current_user;
}

void Registrform::on_pushButton_goLoginPage_clicked()
{
    emit req_loginPage();
}

void Registrform::on_buttonBox_accepted()
{
    QString login  = ui->lineEdit_login->text();
    QString pass_1 = ui->lineEdit_password->text();
    QString pass_2 = ui->lineEdit_confirmPass->text();


    if(login.isEmpty() || pass_1.isEmpty() || pass_2.isEmpty())
    {
        ui->lineEdit_login->setText("Empty field!");
        return;
    }

    if(pass_1 != pass_2)
    {
        ui->lineEdit_login->setText("Password doesn't mach");
        return;
    }

    std::string pass_salt = pass_1.toStdString();
    my_add_salt(pass_salt);
    Hash hash = sha1((char*)pass_salt.c_str(), pass_salt.size());

    server->addUser(login.toStdWString(), hash);

    current_user = login;

    emit accepted();
}

void Registrform::on_buttonBox_rejected()
{
    emit rejected();
}

void Registrform::on_lineEdit_login_editingFinished()
{
    QString login  = ui->lineEdit_login->text();
    if(login.isEmpty())
        return;

    if(server->userExists(login.toStdWString()))
        ui->lineEdit_login->setText("User already exists");
}

