#include <QApplication>
#include <QStyleFactory>

#include "mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setOrganizationName("CompGeom");
    QApplication::setApplicationName("GeometryWorkbench");
    QApplication::setStyle(QStyleFactory::create("Fusion"));

    MainWindow window;
    window.show();

    return app.exec();
}