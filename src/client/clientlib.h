#pragma once

#include "kommunicator.h"

#include <QObject>
#include <QTcpSocket>
//#include <QScopedPointer>

class Client : public QTcpSocket
{
    Q_OBJECT

public:
    Client(QObject *parent = nullptr);
    
    UserList *const getUserList();
    MessageList *const getMessageList();
    User *getUser();
    
private:
    UserList *m_user_list;
    MessageList *m_message_list;
    User *m_user;
    Kommunicator *m_kommunicator;

private slots:
    //void handleReceivedData();
    
signals:
    //void recvDataUpdate(QString);
    void pingReceived(quint16 count);
    void messageReceived(Message *message);
};
