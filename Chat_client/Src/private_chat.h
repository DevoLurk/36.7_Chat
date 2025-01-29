#ifndef PRIVATE_CHAT_H
#define PRIVATE_CHAT_H

#include <QWidget>
#include <vector>
#include "server.h"
#include "messagewidget.h"
#include "messageprivate.h"

namespace Ui {
class Private_chat;
}

class Private_chat : public QWidget
{
    Q_OBJECT

public:
    explicit Private_chat(QWidget *parent = nullptr);
    ~Private_chat();
    void setServer(const std::shared_ptr<Server>& server);
    void setUser(const QString& user);
    void preload_data();

private slots:
    void on_lineEdit_findU_editingFinished();
    void on_pushButton_msgSend_clicked();

private:
    void set_styles();
    void update_OnScr_private();
    void add_OnScr_msg(const QString& message, const bool& is_backgroung, const bool& is_left);

private:
    Ui::Private_chat *ui;
    std::shared_ptr<Server> server;
    QString user;
    std::vector<MessagePrivate> msgArr;
};

#endif // PRIVATE_CHAT_H
