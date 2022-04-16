#pragma once

#include "userdata.h"
#include "kommunicator.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QThread>

Q_DECLARE_METATYPE(QHostAddress);

class Server : public QTcpServer
{
    Q_OBJECT

public:
    Server(QObject *parent = nullptr, quint16 port = 9000);
    ~Server();
    bool isAlive();
    quint64 serverThreads();

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:
    void setAlive(bool t_alive);
    bool m_alive;
    quint64 m_threadCount;
    UserList *m_user_list;
    MessageList *m_message_list;

signals:
    //void serverStopped();
    void statusChanged(bool);
    void connectionAccepted();
    void connectionOpened(QHostAddress); // TODO: remove this placeholder
    void connectionClosed(QHostAddress);

private slots:
    void threadFinished();
    //void acceptConnection();
};

class ServerWorker : public QTcpSocket
{
    Q_OBJECT
    
public:
    ServerWorker(qintptr socketDescriptor, UserList *user_list, MessageList *message_list);
    
public slots:
    void start();
    void stop();
    
private:
    qintptr m_socket_descriptor;
    QHostAddress m_address;
    UserList *m_user_list;
    MessageList *m_message_list;
    User *m_user;
    Kommunicator *m_kommunicator;
    
signals:
    void connectionOpened(QHostAddress);
    void connectionClosed(QHostAddress);
    void error(QTcpSocket::SocketError);
};
