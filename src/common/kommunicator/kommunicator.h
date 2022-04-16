#pragma once

#include "userdata.h"

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>

class Kommunicator : public QObject
{
    Q_OBJECT

public:
    enum Side {
        Server,
        Client
    };

    Kommunicator(Side side, QTcpSocket *socket, UserList *user_list, MessageList *message_list, QObject *parent = nullptr);

private:
    enum OpCode : quint16 {
        Unknown = 0,
        Ping,
        Msg,
        UserData,
        Register,
        Login,
    };
    
    Side m_side;
    int m_status;
    QTcpSocket *m_socket;
    QDataStream m_sock_io;
    UserList *m_user_list;
    MessageList *m_message_list;

public slots:
    void handleData();
    void sendPing(quint16 count = 0);
    void sendMessage(const Message *message);
    void sendUserData();
    
signals:
    void pingReceived(quint16 count);
    void messageReceived(Message *message);
};
