#include "kommunicator.h"

// communication structure:
// quint16(command_id) + args
// commands:
// 1 - ping
// args: QString("pong")
// 2 - motd
// args:
// server side: QString(server_motd)
// client side: none

Kommunicator::Kommunicator(Side side, QTcpSocket *socket, UserList *user_list, MessageList *message_list, QObject *parent)
    : QObject(parent)
    , m_side(side)
    , m_socket(socket)
    , m_sock_io(m_socket)
    , m_user_list(user_list)
    , m_message_list(message_list)
{
    connect(m_socket, &QTcpSocket::readyRead, this, &Kommunicator::handleData);

    switch (m_side) {
    case Side::Server: {
        // TODO: idk
        //m_sock_io << quint16(2) << QString("<h1>Welcome to my server!</h1><p>Steps to do after joining the server:<ol><li>Log in or register</li><li>Enjoy yourself</li><li>\?\?\?</li><li>Profit</li></ol></p>");
        //m_socket->flush();
        break;
    }
    case Side::Client: {
        // TODO: is this even needed?
        break;
    }
    }
}

void Kommunicator::handleData()
{
    // TODO: implement things
    static OpCode received = Unknown;

    if (received == Unknown) {
        m_sock_io.startTransaction();
        m_sock_io >> received;
        if (not m_sock_io.commitTransaction()) {
            return; // idk what happened
        }
    }

    switch (received) {
    case Unknown: {
        // TODO: is there anything to do?
        break;
    }
    case Ping: {
        // answer it in place
        QString message;
        quint16 count;

        m_sock_io.startTransaction();
        m_sock_io >> message >> count;
        if (not m_sock_io.commitTransaction()) {
            return; // incomplete message
        }
        received = Unknown;

        if (message == "ping") {
            m_sock_io << QString("pong") << count;
            m_socket->flush();
        } else if (message == "pong") {
            emit pingReceived(count);
        }

        break;
    }
    case Msg: {
        Message *message = m_message_list->addMessage();

        m_sock_io.startTransaction();
        m_sock_io >> *message;
        if (not m_sock_io.commitTransaction()) {
            m_message_list->removeMessage(message);
            return; // incomplete message
        }
        received = Unknown;
        
        User *from = m_user_list->findId(message->getFrom());
        User *to_u = nullptr;
        Group *to_g = nullptr;
        
        if (message->getToGroup()) {
            to_g = nullptr; // TODO: implement groups
        } else {
            to_u = m_user_list->findId(message->getTo());
        }
        
        if (from != nullptr and 
            ((message->getToGroup() and to_g != nullptr) or 
            (not message->getToGroup() and to_u != nullptr))) {
            from->addMessage(message);
            if (message->getToGroup()) {
                // TODO: implement groups
            } else {
                to_u->addMessage(message);
            }
        } else {
            m_message_list->removeMessage(message);
            return;
        }
        
        emit messageReceived(message);

        break;
    }
    }
}

void Kommunicator::sendPing(quint16 count)
{
    m_sock_io << OpCode::Ping << QString("ping") << count;
    m_socket->flush();
}


void Kommunicator::sendMessage(const Message *message)
{
    m_sock_io << OpCode::Msg << *message;
    m_socket->flush();
}

void Kommunicator::sendUserData()
{
    // TODO: implement, this is a stub
}
