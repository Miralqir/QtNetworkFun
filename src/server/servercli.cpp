#include "servercli.h"

#include <QCoreApplication>
#include <QtDebug>

ServerCLI::ServerCLI(QObject *parent)
    : QObject(parent)
    , m_server(new Server(this))
{
    qInfo() << QString("%1 server starting...").arg(QCoreApplication::applicationName());
    
    connect(m_server, &Server::statusChanged, this, [](bool alive){qInfo() << QString("Server status has changed: '%1'").arg(alive);});
    connect(m_server, &Server::connectionOpened, this, [](QHostAddress addr){qInfo() << QString("Opened connection to '%1'").arg(addr.toString());});
    connect(m_server, &Server::connectionClosed, this, [](QHostAddress addr){qInfo() << QString("Closed connection to '%1'").arg(addr.toString());});
}
