#include "startscreen.h"
#include "ui_startscreen.h"

Startscreen::Startscreen(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Startscreen)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint| Qt::Window);

    this->server = std::make_shared<Server>();
    if(!this->server->setConnection())
    {
        QMessageBox::critical(this, "Connection Failed", "Failed to establish connection.");
    }

    auto login_w = qobject_cast<Loginform*>(ui->stackedWidget->widget(0));
    auto reg_w = qobject_cast<Registrform*>(ui->stackedWidget->widget(1));
    if (login_w) {login_w->setServer(this->server);}
    if (reg_w) {reg_w->setServer(this->server);}

    connect(ui->page_login, &Loginform::req_registrPage, this, &Startscreen::setRegistrationForm);
    connect(ui->page_register, &Registrform::req_loginPage, this, &Startscreen::setLoginForm);

    connect(ui->page_login, &Loginform::rejected, this, &Startscreen::onRejectRequested);
    connect(ui->page_register,&Registrform::rejected, this, &Startscreen::onRejectRequested);
    connect(ui->page_login, &Loginform::accepted, this, &Startscreen::onAcceptRequested_l);
    connect(ui->page_register, &Registrform::accepted, this, &Startscreen::onAcceptRequested_r);
}

Startscreen::~Startscreen()
{
    delete ui;
}

void Startscreen::setLoginForm()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void Startscreen::setRegistrationForm()
{
    ui->stackedWidget->setCurrentIndex(1);
}

std::shared_ptr<Server> Startscreen::get_server()
{
    return this->server;
}

QString Startscreen::get_user()
{
    return this->user;
}

void Startscreen::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect rect = this->rect();
    QPen pen(QColor(153, 153, 255), 0.85, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin); // border
    QBrush brush(QColor(37, 37, 37));                                                 // backgroubd

    auto offset = 6;
    auto radius = 3;

    painter.setPen(pen);
    painter.fillRect(rect.adjusted(0, 0, 0, 0), brush);
    painter.drawRoundedRect(rect.adjusted(offset, offset, -offset, -offset), radius, radius, Qt::AbsoluteSize);
}

void Startscreen::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragStartPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
        dragging = true;
        event->accept();
    }
}

void Startscreen::mouseMoveEvent(QMouseEvent *event)
{
    if (dragging && (event->buttons() & Qt::LeftButton))
    {
        move(event->globalPosition().toPoint() - dragStartPos);
        event->accept();
    }
}

void Startscreen::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        dragging = false;
        event->accept();
    }
}

void Startscreen::onRejectRequested()
{
    reject();
}

void Startscreen::onAcceptRequested_l()
{
    auto login_w = qobject_cast<Loginform*>(ui->stackedWidget->widget(0));
    if (login_w)
        this->user = login_w->get_User();

    accept();
}

void Startscreen::onAcceptRequested_r()
{
    auto reg_w = qobject_cast<Registrform*>(ui->stackedWidget->widget(1));
    if (reg_w)
        this->user = reg_w->get_User();

    accept();
}
