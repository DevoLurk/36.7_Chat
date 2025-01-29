#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "connection_form.h"

MainWindow::MainWindow(const std::shared_ptr<Server>& server, const std::shared_ptr<Database>& database, QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->server = server;
    this->database = database;
    this->logger = std::make_shared<Logger>(this->ui->textEdit_svLogs);
    set_styles();
}

MainWindow::~MainWindow()
{
    delete ui;
    if(this->worker_thread.joinable())
        this->worker_thread.join();
}

MainWindow* MainWindow::init()
{
    connection_form start_screeen;
    auto result = start_screeen.exec();

    if(result == QDialog::Rejected)
        return nullptr;

    auto w = new MainWindow(start_screeen.getServer(), start_screeen.getDatabase());
    w->setAttribute(Qt::WA_DeleteOnClose);
    return w;
}

QPixmap MainWindow::createCirclePixmap(int size, QColor color)
{
    QPixmap pixmap(size, size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(color);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(0, 0, size, size);

    return pixmap;
}

void MainWindow::start_func()
{
    std::wstring task, userName;
    QString log_string;

    while (true)
    {
        server->gettask(userName, task);
        if (task.length() == 0)
        {
            ui->label_status_clConn->setPixmap(createCirclePixmap(size_px_clConn, Qt::red));
            is_connected = false;

            server->close_clientSocket();
            break;
        }

        log_string = QString("[%1] userName: %2  task: <b>%3</b>").arg(getTimeStamp()).arg(userName, 10).arg(task);

        if (task == L"updatePrivate")
        {
            int client_msg_cnt, server_msg_cnt;

            server->readInt(client_msg_cnt);
            server_msg_cnt = database->db_getPrivateMsgCnt(userName);

            if (server_msg_cnt == -1)
            {
                log_string += " <span style='color:red;'>ERROR</span> db_getPrivateMsgCnt()";
                continue;
            }

            server->sendInt(server_msg_cnt - client_msg_cnt); // notice! we mean server_msg_cnt allways bigger or simular to client_msg_cnt

            std::queue<std::wstring> senders, recievers, messages;
            std::queue<bool> flags;
            bool send_flag = database->db_getAuthrorAndMessage_priv(senders, recievers, messages, flags, client_msg_cnt, server_msg_cnt, userName);
            server->sendBool(send_flag);

            if(!send_flag)
            {
                log_string += " <span style='color:red;'>ERROR</span> db_getAuthrorAndMessage_priv()";
                continue;
            }

            for(auto i{0}; i < (server_msg_cnt - client_msg_cnt); i++)
            {
                server->sendStr(senders.front());
                senders.pop();
                server->sendStr(recievers.front());
                recievers.pop();
                server->sendStr(messages.front());
                messages.pop();
                server->sendBool(flags.front());
                flags.pop();
            }

            log_string += " <span style='color:green;'>DONE</span>";
        }
        else if (task == L"updatePublic")
        {
            int server_msg_cnt = database->db_getPublicMsgCnt();
            int client_msg_cnt, diff;
            bool send_flag;

            if (server_msg_cnt <= 0)
            {
                log_string += " <span style='color:red;'>ERROR</span> db_getPublicMsgCnt()";
                continue;
            }

            server->readInt(client_msg_cnt);

            diff = server_msg_cnt - client_msg_cnt;
            server->sendInt(diff);

            if(diff <= 0)
            {
                log_string += " <span style='color:green;'>DONE</span> nothing to send";
                continue;
            }

            std::queue<std::wstring> authors, messages;
            send_flag = database->db_getAuthrorAndMessage_pub(authors, messages, client_msg_cnt, server_msg_cnt);
            server->sendBool(send_flag);

            if(!send_flag)
            {
                log_string += " <span style='color:red;'>ERROR</span> db_getAuthrorAndMessage()";
                continue;
            }

            for(auto i{0}; i < diff; i++)
            {
                server->sendStr(messages.front());
                messages.pop();
                server->sendStr(authors.front());
                authors.pop();
            }

            log_string += " <span style='color:green;'>DONE</span>";
        }
        else if (task == L"userExists")
        {
            bool is_user;
            if(!database->db_userExists(userName, is_user))
                log_string += "<span style='color:red;'>ERROR</span> db_userExists()";

            server->sendBool(is_user);
            log_string += " <span style='color:green;'>DONE</span>";
        }
        else if (task == L"checkPass")
        {
            bool hash_compare;
            Hash client_hash, server_hash;
            server->readHash(client_hash);

            if (!database->db_getUserHash(server_hash, userName))
                log_string += " <span style='color:red;'>ERROR</span> db_getUserHash()";

            if (server_hash == client_hash)
                hash_compare = true;
            else
                hash_compare = false;

            server->sendBool(hash_compare);
            log_string += " <span style='color:green;'>DONE</span>";
        }
        else if (task == L"addUser")
        {
            Hash client_hash;
            server->readHash(client_hash);

            if (!database->db_addUser(client_hash, userName))
                log_string += " <span style='color:red;'>ERROR</span> db_addUser()";
            else
                log_string += " <span style='color:green;'>DONE</span>";
        }
        else if (task == L"rememberPUBMail")
        {
            std::wstring pub_message;
            server->readStr(pub_message);

            if (!database->db_addPublicMsg(userName, pub_message))
                log_string += " <span style='color:red;'>ERROR</span> db_addPublicMsg()";
            else
                log_string += " <span style='color:green;'>DONE</span>";
        }
        else if (task == L"sendMessage")
        {
            std::wstring private_message, reciever;
            server->readStr(reciever);
            server->readStr(private_message);

            if (!database->db_addPrivateMsg(userName, reciever, private_message))
                log_string += " <span style='color:red;'>ERROR</span> db_addPrivateMsg()";
            else
                log_string += " <span style='color:green;'>DONE</span>";
        }
        else if (task == L"setMsgReaded")
        {
            int poz;                    // poz = msg position in msg_arr --starts from 0 in db we count from 1
            server->readInt(poz);

            if (!database->db_setMsgReaded(userName, poz + 1))
                log_string += " <span style='color:red;'>ERROR</span> db_setMsgReaded()";
            else
                log_string += " <span style='color:green;'>DONE</span>";
        }

        logger->log(log_string);
    }
}

QString MainWindow::getTimeStamp()
{
    char buf[20];
    struct tm mytime;

    std::time_t now = std::time(nullptr);
    localtime_s(&mytime, &now);

    strftime(buf, 20, "%Y-%m-%d %H:%M:%S", &mytime);

    std::string time(buf);
    QString qtime = QString::fromStdString(time);
    return qtime;
}

void MainWindow::on_button_findUser_clicked()
{
    QString user = ui->lineEdit_findUser->text();
    bool flag = false;

    database->db_userExists(user.toStdWString(), flag);

    if(flag)
    {
        this->user_to_find = user;
        ui->label_findUser->setFixedSize(size_px_clConn, size_px_clConn);
        ui->label_findUser->setPixmap(createCirclePixmap(size_px_clConn, Qt::green));
    }
    else
    {
        ui->label_findUser->setFixedSize(size_px_clConn, size_px_clConn);
        ui->label_findUser->setPixmap(createCirclePixmap(size_px_clConn, Qt::red));
    }
}

void MainWindow::on_button_banUser_clicked()
{
    if(!this->user_to_find.isEmpty())
        if(!baned_users.contains(this->user_to_find))
            this->baned_users.append(this->user_to_find);
}

void MainWindow::on_button_showBanU_clicked()
{
    ui->textEdit_showU->clear();
    for(auto usr : this->baned_users)
        ui->textEdit_showU->append(usr);
}

void MainWindow::on_button_showAllU_clicked()
{
    ui->textEdit_showU->clear();

    std::queue<std::wstring> names_queue;
    int user_cnt = database->db_getUsersCount();

    if(user_cnt <= 0)
        return;

    database->db_getUsernames(names_queue, user_cnt);

    for(int i{0}; i < user_cnt; i++)
    {
        ui->textEdit_showU->append(QString::fromStdWString(names_queue.front()));
        names_queue.pop();
    }
}

void MainWindow::on_button_clListen_clicked()
{

    if(this->is_listeningn)
        return;
    else
    {
        if(th_Uconn.joinable())
            th_Uconn.join();
        if(worker_thread.joinable())
            worker_thread.join();

        this->is_listeningn = true;
        ui->label_status_clConn->setPixmap(createCirclePixmap(size_px_clConn, Qt::yellow));

        th_Uconn = std::thread([this]()
            {
                bool result = this->server->acceptClient(25); // latency 25 sec (default 10)

                QMetaObject::invokeMethod(ui->label_status_clConn,
                    [this, result]() {
                        if (result)
                        {
                            ui->label_status_clConn->setPixmap(createCirclePixmap(size_px_clConn, Qt::green));
                            this->is_connected = true;
                            this->worker_thread =  std::thread([this](){this->start_func();});
                        }
                        else
                            ui->label_status_clConn->setPixmap(createCirclePixmap(size_px_clConn, Qt::red));
                    }
                );
               this->is_listeningn = false;
            });
    }
}

void MainWindow::set_styles()
{
    ui->label_Ucontrol->setAlignment(Qt::AlignCenter);
    ui->label_svLogs->setAlignment(Qt::AlignCenter);
    ui->label_header_clConn->setAlignment(Qt::AlignCenter);

    ui->label_status_clConn->setFixedSize(size_px_clConn, size_px_clConn);
    ui->label_status_clConn->setPixmap(createCirclePixmap(size_px_clConn, Qt::red));
    ui->label_status_clConn->setToolTip(" RED: User disconnected\n YELLOW: Listenin for user connection\n GREEN: User connected");

    QString label_stile = "font-weight: bold; font-size: 14px; padding: 3px;";
    QString bottoms_stile = R"( QPushButton{
                                    color: white;
                                    font-weight: bold;
                                    font-family: Cascadia Code;
                                    background-color:rgb(67, 67, 104);
                                    border: 1px solid black;
                                    border-radius: 3px;
                                    padding:6px 26px;
                                }
                                QPushButton:hover {
                                    background-color: rgb(97, 97, 134);
                                }
                                QPushButton:pressed {
                                    background-color: rgb(50, 50, 95);
                                })";

    ui->button_banUser->setStyleSheet(bottoms_stile);
    ui->button_findUser->setStyleSheet(bottoms_stile);
    ui->button_clListen->setStyleSheet(bottoms_stile);
    ui->button_showAllU->setStyleSheet(bottoms_stile);
    ui->textEdit_showU->setStyleSheet(bottoms_stile);
    ui->button_showBanU->setStyleSheet(bottoms_stile);

    ui->label_header_clConn->setStyleSheet(label_stile);
    ui->label_header_findUser->setStyleSheet("font-weight: bold; padding: 3px;");
    ui->label_svLogs->setStyleSheet(label_stile);
    ui->label_Ucontrol->setStyleSheet(label_stile);
}
