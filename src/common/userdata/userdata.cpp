#include "userdata.h"

#include <QMutexLocker>

UserList::UserList(QObject *parent)
    : QObject(parent)
{
    User *server = append();
    server->setId(0);
    server->setName("Server");
    User *unregistered = append();
    unregistered->setId(1);
    unregistered->setName("Unregistered user");
}

//const QList<User *> &UserList::getList()
//{
//    return m_list;
//}

User *UserList::append()
{
    QMutexLocker locker(&m_list_mutex);
    m_list.append(new User(this));
    return m_list.last();
}

User *UserList::findId(quint64 id)
{
    QMutexLocker locker(&m_list_mutex);
    
    for (int c = 0; c < m_list.size(); ++c) {
        if (m_list.at(c)->getId() == id) {
            return m_list.at(c);
        }
    }
    
    return nullptr;
}

User *UserList::findName(const QString &name)
{
    QMutexLocker locker(&m_list_mutex);
    
    for (int c = 0; c < m_list.size(); ++c) {
        if (m_list.at(c)->getName() == name) {
            return m_list.at(c);
        }
    }
    
    return nullptr;
}

void UserList::removeId(quint64 id)
{
    QMutexLocker locker(&m_list_mutex);
    
    for (int c = 0; c < m_list.size(); ++c) {
        if (m_list.at(c)->getId() == id) {
            m_list.takeAt(c)->deleteLater();
        }
    }
}

void UserList::removeName(const QString &name)
{
    QMutexLocker locker(&m_list_mutex);
    
    for (int c = 0; c < m_list.size(); ++c) {
        if (m_list.at(c)->getName() == name) {
            m_list.takeAt(c)->deleteLater();
        }
    }
}

User::User(UserList *parent)
    : QObject(parent)
{}

quint64 User::getId()
{
    return m_id;
}

void User::setId(const quint64 id)
{
    m_id = id;
}

QString User::getName()
{
    return m_name;
}

void User::setName(const QString &name)
{
    m_name = name;
}

QList<User::Friend> User::getFriends()
{
    return m_friends;
}

QStringList User::getFriendNames()
{
    QStringList names;
    for (int c = 0; c < m_friends.size(); ++c) {
        User *res = qobject_cast<UserList *>(parent())->findId(m_friends.at(c).id);
        if (res != nullptr) {
            names += res->getName();
        }
    }
    
    return names;
}

void User::setFriends(const QList<Friend> &friends)
{
    m_friends = friends;
}

void User::addFriend(const quint64 id, const User::FriendType type)
{
    m_friends.append({id, type});
}

void User::removeFriend(const quint64 id)
{
    for (int c = 0; c < m_friends.size(); ++c) {
        if (m_friends.at(c).id == id) {
            m_friends.removeAt(c);
        }
    }
}

QList<Message *> User::getMessages()
{
    return m_messages;
}

void User::setMessages(const QList<Message *> &messages)
{
    m_messages = messages;
}

void User::addMessage(Message *const message)
{
    m_messages.append(message);
}

QDataStream &operator<<(QDataStream &stream, const User::Friend &f)
{
    stream << f.id << f.type;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, User::Friend &f)
{
    stream >> f.id >> f.type;
    return stream;
}

QDataStream &operator<<(QDataStream &stream, const User &user)
{
    stream << user.m_id << user.m_name << user.m_visibility << user.m_friends << user.m_groups;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, User &user)
{
    stream >> user.m_id >> user.m_name >> user.m_visibility >> user.m_friends >> user.m_groups;
    return stream;
}

MessageList::MessageList(QObject *parent)
    : QObject(parent)
{}

Message *MessageList::addMessage()
{
    QMutexLocker locker(&m_list_mutex);
    m_list.append(new Message(this));
    return m_list.last();
}

void MessageList::removeMessage(Message *const message)
{
    QMutexLocker locker(&m_list_mutex);
    m_list.takeAt(m_list.indexOf(message))->deleteLater();
}

Message::Message(MessageList *parent)
    : QObject(parent)
    , m_timestamp(QDateTime::currentDateTimeUtc())
    , m_from(1)
    , m_to(1)
    , m_to_group(false)
{}

QDateTime Message::getTimestamp()
{
    return m_timestamp;
}

QString Message::getData()
{
    return m_data;
}

quint64 Message::getFrom()
{
    return m_from;
}

quint64 Message::getTo()
{
    return m_to;
}

bool Message::getToGroup()
{
    return m_to_group;
}

void Message::setTimestamp(const QDateTime timestamp)
{
    m_timestamp = timestamp;
}

void Message::setData(const QString &data)
{
    m_data = data;
}

void Message::setFrom(const quint64 from)
{
    m_from = from;
}

void Message::setTo(const quint64 to)
{
    m_to = to;
}

void Message::setToGroup(const bool to_group)
{
    m_to_group = to_group;
}

QDataStream &operator<<(QDataStream &stream, const Message &message)
{
    stream << message.m_timestamp << message.m_data << message.m_from << message.m_to << message.m_to_group;
    return stream;
}

QDataStream &operator>>(QDataStream &stream, Message &message)
{
    stream >> message.m_timestamp >> message.m_data >> message.m_from >> message.m_to >> message.m_to_group;
    return stream;
}
