#include <QApplication>

#include "gui/glwidget.hpp"
#include "gui/mainwindow.hpp"
#include "gui/cli.h"

int main(int argc, char *argv[])
{
    cli theCli(argc, argv);
    if(!theCli.continueRun())
	return 1;
    
    QApplication app(argc, argv);

    // set gl format
    GLWidget::setGLFormat();

    // load the main window
    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
