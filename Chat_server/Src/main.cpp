#include "mainwindow.h"

#include <QApplication>

#ifdef _WIN32
#include<WinSock2.h> // pleaze include <WinSock2.h> before <Windows.h>... bla bla bla
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auto w = MainWindow::init();
    if(w)
        w->show();
    else
        return 0;

    return a.exec();
}
