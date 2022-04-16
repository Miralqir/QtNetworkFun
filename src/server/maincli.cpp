#include "project_information.h"
#include "servercli.h"

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    app.setApplicationName(CMAKE_PROJECT_NAME);
    app.setApplicationVersion(CMAKE_PROJECT_VERSION);
    app.setOrganizationDomain(CMAKE_PROJECT_HOMEPAGE_URL);
    app.setOrganizationName(CPACK_PACKAGE_VENDOR);

    ServerCLI cli;

    return app.exec();
}
