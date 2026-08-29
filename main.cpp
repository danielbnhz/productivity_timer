#include <QApplication>
#include <QPushButton>
#include "src/view/timer_view.h"
int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    Timer_View t;
    t.show();
    return QApplication::exec();
}