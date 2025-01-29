#ifndef PUBLICK_CHAT_H
#define PUBLICK_CHAT_H

#include <QWidget>
#include <vector>
#include "server.h"
#include "messagewidget.h"

namespace Ui {
class Publick_chat;
}

class Publick_chat : public QWidget
{
    Q_OBJECT

public:
    explicit Publick_chat(QWidget *parent = nullptr);
    ~Publick_chat();
    void setServer(const std::shared_ptr<Server>& server);
    void setUser(const QString& user);
    void preload_data();

private slots:
    void on_pushButton_msgSend_clicked();

private:
    void set_slyles();
    void update_OnScr_public();
    void add_OnScr_msg(const QString& message, const bool& is_backgroung = false);

private:
    Ui::Publick_chat *ui;
    QString user;
    std::shared_ptr<Server> server;
    std::vector<Message> msgArr;
};

#endif // PUBLICK_CHAT_H
