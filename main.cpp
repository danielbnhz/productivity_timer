#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QLabel label("Hello world again!");

    label.resize(800,800);
    label.show();

    return app.exec();
}