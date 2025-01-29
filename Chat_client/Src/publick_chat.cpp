#include "publick_chat.h"
#include "ui_publick_chat.h"

Publick_chat::Publick_chat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Publick_chat)
{
    ui->setupUi(this);
    set_slyles();
}

Publick_chat::~Publick_chat()
{
    delete ui;
}

void Publick_chat::setServer(const std::shared_ptr<Server>& server)
{
    this->server = server;
}

void Publick_chat::setUser(const QString& user)
{
    this->user = user;
}

void Publick_chat::preload_data()
{
    update_OnScr_public();
}

void Publick_chat::update_OnScr_public()
{
    int msgCnt_before = this->msgArr.size();

    this->server->updatePublic(this->user.toStdWString(), this->msgArr);

    int difference = this->msgArr.size() - msgCnt_before;
    if(difference == 0)
        return;


    int i = 0;
    for(auto msg : this->msgArr)
    {
        QString message = QString::fromStdWString(msg.getAuthor()) + " : " + QString::fromStdWString(msg.getMsg());
        bool is_author = msg.getAuthor() == this->user.toStdWString();
        add_OnScr_msg(message, is_author);

        if(i == difference)
            break;

        i++;
    }
}

void Publick_chat::add_OnScr_msg(const QString &message, const bool &is_backgroung)
{
    QListWidgetItem* item = new QListWidgetItem();
    ui->listWidget_msgs->addItem(item);
    MessageWidget* widget = new MessageWidget(message, is_backgroung);
    item->setSizeHint(widget->sizeHint());
    ui->listWidget_msgs->setItemWidget(item, widget);
}

void Publick_chat::on_pushButton_msgSend_clicked()
{
    QString message = ui->lineEdit_msgToSend->text();
    if(message.isEmpty())
        return;

    this->msgArr.emplace_back(this->user.toStdWString(), message.toStdWString());
    server->rememberMail(this->user.toStdWString(), message.toStdWString());

    ui->lineEdit_msgToSend->clear();
    add_OnScr_msg(this->user + " : " + message, true);
}

void Publick_chat::set_slyles()
{
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
    ui->lineEdit_msgToSend->setStyleSheet(line_stile);

}



