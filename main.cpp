#include "catalog.h"
#include <windows.h>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    catalog w;
    w.show();
    return a.exec();
}
