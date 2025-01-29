#include "connection_form.h"
#include "ui_connection_form.h"

connection_form::connection_form(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::connection_form)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    set_styles();

    if(!load_config())
        set_config_to_default();
    set_onScreen_config();
}

connection_form::~connection_form()
{
    delete ui;
}

void connection_form::on_start_button_clicked()
{
    if(check_ctatus())
    {
        accept();
    }
    else
        ui->start_button->setStyleSheet(" background-color: red;");
}

void connection_form::on_close_button_clicked()
{
    reject();
}

void connection_form::on_dbConnect_botton_clicked()
{
    this->db_status = false;
    processTextEdit();
    std::wstring dbname;


    if(ui->dbNew_checkBox->isChecked())
    {
        std::wstring short_config;

        dbname = get_dbname_from_conf();
        short_config = get_short_config();
        this->database =  std::make_shared<Database>(short_config);
    }
    else
        this->database = std::make_shared<Database>(this->config);

    database->db_connect();

    QString status_msg;
    if(!database->db_is_active())
    {
        status_msg = "<span style='color:red;'>" + QString::fromStdWString(database->get_SQLError()) + "</span>";

        ui->dbStatus_label->setText(status_msg);
        return;
    }

    if(ui->dbNew_checkBox->isChecked())
        if(!database->db_create(dbname))
        {
            status_msg = "<span style='color:orange;'>Creation failed but connection is good</span>";
            ui->dbStatus_label->setText(status_msg);
            return;
        }

    this->db_status = true;
    status_msg = "<span style='color:green;'>Connection success</span>";
    ui->dbStatus_label->setText(status_msg);
}

bool connection_form::check_ctatus()
{
    return (db_status && sv_status);
}

void connection_form::processTextEdit()
{
    this->config.clear();

    QString allText = ui->dbconf_textEdit->toPlainText();
    QStringList lines = allText.split('\n');

    for (int i = 0; i < lines.size(); ++i)
    {
        QString line = lines[i].trimmed(); // Убираем начальные и конечные пробелы

        if (line.isEmpty()) // Пропускаем пустые строки
            continue;  

        if (i == 0)
            this->config += processFirstLine(line);
        else
            this->config += processOtherLines(line);
    }

    if(ui->save_checkBox->isChecked())
        save_config();
}

void connection_form::save_config()
{
    std::ofstream conf_out(config_path, std::ios::trunc);
    if (!conf_out.is_open())
        return;

    std::string conf(this->config.begin(), this->config.end());

    conf_out << conf;
    conf_out.close();
}

void connection_form::set_config_to_default()
{
    this->config += L"DRIVER={MySQL ODBC 9.2 ANSI Driver};";
    this->config += L"SERVER=localhost;" ;
    this->config += L"PORT=3306;";
    this->config += L"DATABASE=your_dbname;";
    this->config += L"UID=your_username;";
    this->config += L"PWD=your_passsword;";
}

bool connection_form::load_config()
{
    std::ifstream conf_in(config_path);
    if (!conf_in.is_open())
        return false;

    std::string conf;
    std::getline(conf_in, conf);

    std::wstring wconf(conf.begin(), conf.end());
    this->config = wconf;

    conf_in.close();
    return true;
}

void connection_form::set_onScreen_config()
{
    QString connectionString = QString::fromStdWString(this->config);
    QStringList parts = connectionString.split(';', Qt::SkipEmptyParts);
    QString formattedString = parts.join(";\n");
    formattedString += ';';

    ui->dbconf_textEdit->setPlainText(formattedString);
}

std::wstring connection_form::get_short_config()
{
    std::wstring key = L"DATABASE=";
    std::wstring new_conf = this->config;

    size_t start_pos = new_conf.find(key);
    size_t end_pos = new_conf.find(L";", start_pos);
    end_pos++;
    new_conf.erase(start_pos, end_pos - start_pos);

    return new_conf;
}

std::wstring connection_form::get_dbname_from_conf()
{
    std::wstring key = L"DATABASE=";
    std::wstring dbname;

    size_t start_pos = this->config.find(key);
    start_pos += key.size();

    size_t end_pos = this->config.find(L";", start_pos);

    dbname.append(this->config, start_pos, end_pos - start_pos);

    return dbname;
}

std::wstring connection_form::processFirstLine(const QString &line)
{
    std::wstring result = line.toStdWString();
    std::wstring processed;

    bool insideBraces = false;

    for (wchar_t ch : result)// Удаляем пробелы только если не внутри фигурных скобок
    {
        if (ch == L'{')
            insideBraces = true;
        else if(ch == L'}')
            insideBraces = false;

        if (ch != L' ' || insideBraces)
            processed += ch;
    }

    return processed;
}

std::wstring connection_form::processOtherLines(const QString &line)
{
    std::wstring result = line.toStdWString();
    result.erase(remove(result.begin(), result.end(), L' '), result.end());
    return result;
}

void connection_form::on_svConnect_button_clicked()
{
    QString status_msg;

    this->server = std::make_shared<Server>();
    this->server->startAndListen();

    if(!this->server->sv_is_active())
    {
        status_msg = "<span style='color:red;'>" + QString::fromStdWString(this->server->get_svError()) + "</span>";
    }
    else
    {
        this->sv_status = true;
        status_msg = "<span style='color:green;'>Server is listenin for new connectins</span>";
    }

    ui->svStatus_label->setText(status_msg);
}

void connection_form::set_styles()
{
    ui->dbStatus_label->setAlignment(Qt::AlignCenter);
    ui->svStatus_label->setAlignment(Qt::AlignCenter);
    ui->save_checkBox->setToolTip("Saves config on dbconfig.conf. Will use it on next startup.");
    ui->dbNew_checkBox->setToolTip("Create new database with your_db_name");

    QString text_stile = "font-size: 16px; padding:6px";
    QString label_stile = "font-weight: bold; font-size: 14px; padding: 3px;";
    QString box_stile = "color: rgb(67, 67, 104); font-weight: bold; padding: 3px;";
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

    ui->close_button->setStyleSheet(bottoms_stile);
    ui->start_button->setStyleSheet(bottoms_stile);
    ui->svConnect_button->setStyleSheet(bottoms_stile);
    ui->dbConnect_botton->setStyleSheet(bottoms_stile);
    ui->dbNew_checkBox->setStyleSheet(box_stile);
    ui->save_checkBox->setStyleSheet(box_stile);
    ui->svStatus_label->setStyleSheet(label_stile);
    ui->dbStatus_label->setStyleSheet(label_stile);
    ui->dbconf_textEdit->setStyleSheet(text_stile);
}

void connection_form::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect rect = this->rect();
    QPen pen(QColor(34, 38, 71), 1.5, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin); // border
    QBrush brush(QColor(247, 247, 247));                                                 // backgroubd

    auto offset = 0;
    auto radius = 0;

    painter.setPen(pen);
    painter.fillRect(rect.adjusted(0, 0, 0, 0), brush);
    painter.drawRoundedRect(rect.adjusted(offset, offset, -offset, -offset), radius, radius, Qt::AbsoluteSize);
}

void connection_form::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragStartPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
        dragging = true;
        event->accept();
    }
}

void connection_form::mouseMoveEvent(QMouseEvent *event)
{
    if (dragging && (event->buttons() & Qt::LeftButton))
    {
        move(event->globalPosition().toPoint() - dragStartPos);
        event->accept();
    }
}

void connection_form::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragging = false;
        event->accept();
    }
}
