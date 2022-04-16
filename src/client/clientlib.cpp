#include "clientlib.h"

#include <QDataStream>
#include <QDebug>

Client::Client(QObject *parent)
    : QTcpSocket(parent)
    , m_user_list(new UserList(this))
    , m_message_list(new MessageList(this))
    , m_user(m_user_list->findId(1))
    , m_kommunicator(new Kommunicator(Kommunicator::Side::Client, this, m_user_list, m_message_list, this))
{
    connect(this, &QTcpSocket::readyRead, m_kommunicator, &Kommunicator::handleData);
    connect(m_kommunicator, &Kommunicator::pingReceived, this, &Client::pingReceived);
    connect(m_kommunicator, &Kommunicator::messageReceived, this, &Client::messageReceived);
    
    //connect(this, &QTcpSocket::readyRead, this, &Client::handleReceivedData);
    //connect(this, &QTcpSocket::disconnected, this, [this]{m_recv.clear();});
}

UserList *const Client::getUserList()
{
    return m_user_list;
}

MessageList *const Client::getMessageList()
{
    return m_message_list;
}

User *Client::getUser()
{
    return m_user;
}

/*
void Client::handleReceivedData()
{
    static QDataStream sock_io(this);
    QString buffer;
    static quint16 t = 0;
    
    sock_io.startTransaction();
    if (t == 0) {
        sock_io >> t;
    }
    if (not sock_io.commitTransaction()) {
        return;
    }
    
    sock_io.startTransaction();
    sock_io >> buffer;
    if (not sock_io.commitTransaction()) {
        return;
    }
    
    m_recv.append(buffer);
    
    emit recvDataUpdate(m_recv);
}
*/
