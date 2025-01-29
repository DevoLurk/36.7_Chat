#ifndef CONNECTION_FORM_H
#define CONNECTION_FORM_H

#include <QDialog>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>
#include <QMessageBox>
#include <fstream>
#include "database.h"
#include "server.h"

namespace Ui {
class connection_form;
}

class connection_form : public QDialog
{
    Q_OBJECT

public:
    explicit connection_form(QWidget *parent = nullptr);
    ~connection_form();
    std::shared_ptr<Server> getServer() const { return server; }
    std::shared_ptr<Database> getDatabase() const { return database; }

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void on_start_button_clicked();
    void on_close_button_clicked();
    void on_dbConnect_botton_clicked();
    void on_svConnect_button_clicked();

private:
    Ui::connection_form *ui;
    std::string config_path{ "dbconnection.conf" };
    std::wstring config;
    std::shared_ptr<Database> database;
    std::shared_ptr<Server> server;
    bool sv_status = false;
    bool db_status = false;
    bool dragging = false;
    QPoint dragStartPos;

private:
    void set_styles();
    bool check_ctatus();
    void processTextEdit();
    void save_config();
    bool load_config();
    void set_config_to_default();
    void set_onScreen_config();

    std::wstring get_short_config();
    std::wstring get_dbname_from_conf();
    std::wstring processFirstLine(const QString& line);
    std::wstring processOtherLines(const QString& line);
};

#endif // CONNECTION_FORM_H
