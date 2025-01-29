#include "private_chat.h"
#include "ui_private_chat.h"

Private_chat::Private_chat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Private_chat)
{
    ui->setupUi(this);

    set_styles();
}

Private_chat::~Private_chat()
{
    delete ui;
}

void Private_chat::setServer(const std::shared_ptr<Server>& server)
{
    this->server = server;
}

void Private_chat::setUser(const QString& user)
{
    this->user = user;
}

void Private_chat::preload_data()
{
    update_OnScr_private();
}

void Private_chat::on_lineEdit_findU_editingFinished()
{
    QString user = ui->lineEdit_findU->text();
    bool user_exists = server->userExists(user.toStdWString());

    if(user_exists)
    {
        ui->checkBox_findU->setChecked(true);
        ui->lineEdit_msg->setEnabled(true);
        ui->pushButton_msgSend->setEnabled(true);
    }
    else
    {
        ui->checkBox_findU->setChecked(false);
        ui->lineEdit_msg->setEnabled(false);
        ui->pushButton_msgSend->setEnabled(false);
    }
}


void Private_chat::on_pushButton_msgSend_clicked()
{
    QString message = ui->lineEdit_msg->text();

    if(message.isEmpty())
        return;

    QString reciever = ui->lineEdit_findU->text();

    this->server->sendMessage(this->user.toStdWString(), reciever.toStdWString(), message.toStdWString());
    ui->lineEdit_msg->clear();

    this->msgArr.emplace_back(this->user.toStdWString(), reciever.toStdWString(), message.toStdWString());

    if(this->user == reciever)
    {
        this->msgArr.front().setReaded();
        this->server->setMsgReaded(this->user.toStdWString(), 0);
        add_OnScr_msg(this->user + " :\n" + message, true, true);
    }
    else
    {
        add_OnScr_msg(this->user + " :\n" + message, false, true);
    }
}

void Private_chat::update_OnScr_private()
{
    int msgCnt_before = this->msgArr.size();
    this->server->updatePrivate(this->user.toStdWString(), this->msgArr);

    int difference = this->msgArr.size() - msgCnt_before;
    if(difference == 0)
        return;

    int i = 0, pos = 0;
    for(auto msg : this->msgArr)
    {
        QString message;
        bool is_author = msg.getAuthor() == this->user.toStdWString();

        if(msg.getReciever() == this->user.toStdWString()) // if this user is reciever thin meswsage is readed
        {
            if(msg.getFlag() == false)
            {
                msg.setReaded();
                this->server->setMsgReaded(this->user.toStdWString(), pos);
            }
            pos++;
        }

        if(is_author)
            message = QString::fromStdWString(msg.getReciever()) + " :\n " + QString::fromStdWString(msg.getMsg());
        else
            message = QString::fromStdWString(msg.getAuthor()) + " :\n " + QString::fromStdWString(msg.getMsg());

        add_OnScr_msg(message, msg.getFlag(), is_author);

        if(i == difference)
            break;

        i++;
    }
}

void Private_chat::add_OnScr_msg(const QString &message, const bool &is_backgroung, const bool &is_left)
{
    QListWidgetItem* item = new QListWidgetItem();
    ui->listWidget_msgs->addItem(item);
    MessageWidget* widget = new MessageWidget(message, is_backgroung, is_left);
    item->setSizeHint(widget->sizeHint());
    ui->listWidget_msgs->setItemWidget(item, widget);
}

void Private_chat::set_styles()
{
    ui->lineEdit_msg->setEnabled(false);
    ui->pushButton_msgSend->setEnabled(false);

    QString label_header = "font-weight: bold; font-size: 14px;";
    QString bottoms_stile = R"( QPushButton{
                                    color: white;
                                    font-weight: bold;
                                    font-family: Cascadia Code;
                                    background-color:rgb(67, 67, 104);
                                    border: 1px solid black;
                                    border-radius: 3px;
                                    padding:7px 26px;
                                }
                                QPushButton:hover {
                                    background-color: rgb(97, 97, 134);
                                }
                                QPushButton:pressed {
                                    background-color: rgb(50, 50, 95);
                                })";

    QString line_stile = R"(QLineEdit {
                                color: black;
                                font-family: Microsoft YaHei;
                                font-size: 13px;
                                background-color:rgb(242, 242, 242);
                                border-width: 1px;
                                border-style: solid;
                                border-color:black;
                                border-radius: 3px;
                                padding: 4px 10px;
                            }
                            QLineEdit:hover {
                                    background-color:rgb(209, 211, 240);
                                }
                            QLineEdit:focus {
                                    background-color:rgb(242, 242, 242);
                                })";

    ui->label_header->setAlignment(Qt::AlignCenter);
    ui->label_header->setStyleSheet(label_header);
    ui->pushButton_msgSend->setStyleSheet(bottoms_stile);
    ui->lineEdit_findU->setStyleSheet(line_stile);
    ui->lineEdit_msg->setStyleSheet(line_stile);

    ui->label_findU->setStyleSheet("font-weight: bold; font-size: 12px;");
    ui->checkBox_findU->setStyleSheet("font-weight: bold; font-size: 12px;");

}
