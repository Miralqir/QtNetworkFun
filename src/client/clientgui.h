#pragma once

#include "clientlib.h"
#include "iconloader.h"

#include <QWidget>
#include <QMainWindow>
#include <QDialog>
#include <QGridLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QComboBox>
#include <QScrollArea>
#include <QLabel>
#include <QToolBar>
#include <QAction>
//#include <QScopedPointer>
#include <QErrorMessage>

class ServerChooserDialog : public QDialog
{
    Q_OBJECT

public:
    ServerChooserDialog(QWidget *parent = nullptr);
    
private:
    QFormLayout *m_layout;
    QLabel *m_host_label;
    QLabel *m_port_label;
    QComboBox *m_host_input;
    QComboBox *m_port_input;
    QPushButton *m_return;
    
signals:
    void requestConnect(QString host, QString port);
};

class ClientGUI : public QMainWindow
{
    Q_OBJECT

public:
    ClientGUI(QWidget *parent = nullptr);

private:
    QErrorMessage m_error_message;
    Client *m_client;
    IconLoader *m_icons;
    ServerChooserDialog *m_dialog;
    QWidget *m_central_widget;
    QGridLayout *m_central_layout;
    QTabWidget *m_tab_widget;
    QTextEdit *m_welcome_page;
    QLabel *m_connect_label;
    QToolBar *m_main_toolbar;
    QAction *m_dis_connect;
    
    int getChatTab(const quint64 user_id);
    
private slots:
    void disConnectButtonHandle();
    void connectHandler(QString host, QString port);
    void handleConnectionStatus(QTcpSocket::SocketState socket_state);
    void handleMessage(Message *message);
    void handleCloseTab(int index);
};
