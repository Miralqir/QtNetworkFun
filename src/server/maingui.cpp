#include "project_information.h"
#include "servergui.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName(CMAKE_PROJECT_NAME);
    app.setApplicationVersion(CMAKE_PROJECT_VERSION);
    app.setOrganizationDomain(CMAKE_PROJECT_HOMEPAGE_URL);
    app.setOrganizationName(CPACK_PACKAGE_VENDOR);

    ServerGUI gui;
    gui.show();

    return app.exec();
}
