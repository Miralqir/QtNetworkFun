#include "project_information.h"
#include "testwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName(CMAKE_PROJECT_NAME);
    app.setApplicationVersion(CMAKE_PROJECT_VERSION);

    TestWindow window;
    window.show();

    return app.exec();
}

