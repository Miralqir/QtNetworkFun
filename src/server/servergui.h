#pragma once

#include "serverlib.h"
#include "iconloader.h"

#include <QWidget>
#include <QGridLayout>
#include <QMenuBar>
#include <QMenu>
#include <QLabel>
#include <QPlainTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
//#include <QScopedPointer>
#include <QErrorMessage>

class ServerGUI : public QWidget
{
    Q_OBJECT

public:
    ServerGUI(QWidget *parent = nullptr);

private:
    void serverInitCheck();
    QErrorMessage m_error_message;
    Server *m_server;
    IconLoader *m_icons;
    QGridLayout *m_layout;
    QMenuBar *m_menu_bar;
    QMenu *m_menu1;
    QPlainTextEdit *m_text_output;
    QLineEdit *m_text_input;
    QLabel *m_status;
    QListWidget *m_users;
    QMenu *m_user_menu;
    QPushButton *m_send_button;

private slots:
    void handleStatusChange(bool alive);
    void handleCommand();
    void handleOpenedConnection(QHostAddress addr);
    void handleClosedConnection(QHostAddress addr);
};
