#include "serverlib.h"

#include <QMetaObject>
#include <QCryptographicHash>

const QString server_motd("# Welcome to my server!\nSteps to do after joining the server:\n1. Log in or register\n2. Enjoy yourself\n3. \?\?\?\n4. Profit");

Server::Server(QObject *parent, quint16 port)
    : QTcpServer(parent)
    , m_alive(false)
    , m_threadCount(0)
    , m_user_list(new UserList(this))
    , m_message_list(new MessageList(this))
{
    qRegisterMetaType<QHostAddress>();

    // TODO: are these 2 needed
    User *server = m_user_list->findId(0);
    User *unregistered = m_user_list->findId(1);
    // TODO: load user list somehow
    
    if (not this->listen(QHostAddress::Any, port)) {
        m_alive = false;
        return;
    }

    m_alive = true;
    
    //connect(this, &QTcpServer::newConnection, this, &Server::acceptConnection);
}

Server::~Server()
{
    m_alive = false;
    emit statusChanged(false);
    //emit serverStopped();
}

void Server::incomingConnection(qintptr socketDescriptor)
{    
    QThread *thread = new QThread(this);
    ServerWorker *worker = new ServerWorker(socketDescriptor, m_user_list, m_message_list);
    worker->moveToThread(thread);
    connect(worker, &ServerWorker::connectionOpened, this, &Server::connectionOpened);
    connect(worker, &ServerWorker::connectionClosed, this, &Server::connectionClosed);
    connect(thread, &QThread::finished, this, &Server::threadFinished);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(thread, &QThread::finished, worker, &ServerWorker::deleteLater);
    connect(thread, &QThread::started, worker, &ServerWorker::start);
    m_threadCount++;
    thread->start();
}

void Server::threadFinished()
{
    m_threadCount--;
}

quint64 Server::serverThreads()
{
    return m_threadCount;
}

bool Server::isAlive()
{
    return m_alive;
}

void Server::setAlive(bool t_alive)
{
    m_alive = t_alive;
    emit statusChanged(m_alive);
}

ServerWorker::ServerWorker(qintptr socketDescriptor, UserList *user_list, MessageList *message_list)
    : m_socket_descriptor(socketDescriptor)
    , m_user_list(user_list)
    , m_message_list(message_list)
{}

void ServerWorker::start()
{
    if (not setSocketDescriptor(m_socket_descriptor)) {
        emit error(QTcpSocket::error());
        return;
    }
    
    m_address = peerAddress();
    
    connect(this, &QTcpSocket::disconnected, this, &ServerWorker::stop);
    
    //setSocketOption(QTcpSocket::LowDelayOption, 1);
    
    m_kommunicator = new Kommunicator(Kommunicator::Side::Server, this, m_user_list, m_message_list, this);
    Message *motd_message;
    QMetaObject::invokeMethod(m_message_list, &MessageList::addMessage, Qt::BlockingQueuedConnection, &motd_message);
    motd_message->setData(server_motd);
    motd_message->setFrom(0);
    m_kommunicator->sendMessage(motd_message);
    
    // easter egg? nah.
    QThread::sleep(1);
    QMetaObject::invokeMethod(m_message_list, &MessageList::addMessage, Qt::BlockingQueuedConnection, &motd_message);
    motd_message->setData(QString("Have a cookie for reading everything :)"));
    motd_message->setFrom(0);
    m_kommunicator->sendMessage(motd_message);
    
    // long text test
    QThread::sleep(1);
    QMetaObject::invokeMethod(m_message_list, &MessageList::addMessage, Qt::BlockingQueuedConnection, &motd_message);
    motd_message->setData(QString("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));
    motd_message->setFrom(0);
    m_kommunicator->sendMessage(motd_message);

    emit connectionOpened(m_address);
}

void ServerWorker::stop()
{
    emit connectionClosed(m_address);
    QThread::currentThread()->quit();
}

