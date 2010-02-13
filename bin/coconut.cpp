#include <QtGui/QApplication>
#include "gui_main/main_window.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
