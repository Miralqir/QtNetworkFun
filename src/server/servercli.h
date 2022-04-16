#pragma once

//#undef QT_NO_INFO_OUTPUT

#include "serverlib.h"
#include <QObject>

class ServerCLI : public QObject
{
    Q_OBJECT
    
public:
    ServerCLI(QObject *parent = nullptr);
    
private:
    Server *m_server;
};
