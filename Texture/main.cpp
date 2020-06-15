#include <QApplication>
#include"corefunctionwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CoreFunctionWidget w;
    w.show();

    return a.exec();
}
