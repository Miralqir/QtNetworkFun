#pragma once

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QList>
#include <QStringList>
#include <QByteArray>
#include <QDataStream>
#include <QMutex>

// forward declarations
class User;
class Group;
class Message;

// special users
// 0 = server
// 1 = unregistered client (server messages only)

class UserList : public QObject
{
    Q_OBJECT
    
public:
    UserList(QObject *parent = nullptr);
    
public slots:
    //const QList<User *> &getList();
    User *append();
    User *findId(quint64 id);
    User *findName(const QString &name);
    void removeId(quint64 id);
    void removeName(const QString &name);
    
private:
    QList<User *> m_list;
    QMutex m_list_mutex;
};

class User : public QObject
{
    Q_OBJECT
    
public:
    // pending = I asked them
    // request = they asked me
    enum FriendType : quint8 {Normal, Pending, Request};
    enum Visibility : quint8 {Public, FoF, Private};
    struct Friend
    {
        quint64 id;
        FriendType type;
    };
    
    User(UserList *parent = nullptr);
    //User(quint64 id, QObject *parent = nullptr);
    
public slots:
    quint64 getId();
    void setId(const quint64 id);
    QString getName();
    void setName(const QString &name);
    QList<Friend> getFriends();
    QStringList getFriendNames();
    
    void setFriends(const QList<Friend> &friends);
    void addFriend(const quint64 id, const FriendType type);
    void removeFriend(const quint64 id);
    QList<Message *> getMessages();
    void setMessages(const QList<Message *> &messages);
    void addMessage(Message *const message);
    
public:
    friend QDataStream &operator<<(QDataStream &stream, const Friend &f);
    friend QDataStream &operator>>(QDataStream &stream, Friend &f);
    
    friend QDataStream &operator<<(QDataStream &stream, const User &user);
    friend QDataStream &operator>>(QDataStream &stream, User &user);
    
private:
    quint64 m_id;
    QString m_name;
    Visibility m_visibility;
    QByteArray salt;
    QByteArray pass;
    QList<Friend> m_friends;
    QList<quint64> m_groups;
    //QList<quint64> m_pending_groups;
    //QList<quint64> m_request_groups;
    QList<Message *> m_messages;
};

// TODO: implement groups properly
class Group : public QObject
{
    Q_OBJECT

public:
    enum Visibility : quint8 {Public, FoM, Private};
    
    Group(QObject *parent = nullptr);
    Group(quint64 id, QObject *parent = nullptr);
    
    friend QDataStream &operator<<(QDataStream &stream, const Group &group);
    friend QDataStream &operator>>(QDataStream &stream, Group &group);

private:
    quint64 m_id;
    QString m_name;
    Visibility m_visibility;
    QList<quint64> m_members;
    QList<Message *> m_messages;
};

class MessageList : public QObject
{
    Q_OBJECT
    
public:
    MessageList(QObject *parent = nullptr);
    
    Message *addMessage();
    void removeMessage(Message *const message);
    
private:
    QList<Message *> m_list;
    QMutex m_list_mutex;
};

class Message : public QObject
{
    Q_OBJECT
    
public:
    Message(MessageList *parent = nullptr);
    
public slots:
    QDateTime getTimestamp();
    QString getData();
    quint64 getFrom();
    quint64 getTo();
    bool getToGroup();
    
    void setTimestamp(const QDateTime timestamp);
    void setData(const QString &data);
    void setFrom(const quint64 from);
    void setTo(const quint64 to);
    void setToGroup(const bool to_group);
    
public:
    friend QDataStream &operator<<(QDataStream &stream, const Message &message);
    friend QDataStream &operator>>(QDataStream &stream, Message &message);
    
private:
    QDateTime m_timestamp;
    QString m_data;
    //Qidkyet m_extra_data;
    quint64 m_from;
    quint64 m_to;
    bool m_to_group;
};
