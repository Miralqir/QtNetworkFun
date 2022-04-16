#include "servergui.h"

#include <QIcon>
#include <QDebug>

const QString STATUS_TEXT("Status: %1");
const QString STATUS_ALIVE("Alive");
const QString STATUS_DEAD("Dead");
const QString STATUS_STYLE_ALIVE("color: rgb(0, 255, 0);");
const QString STATUS_STYLE_DEAD("color: rgb(255, 0, 0);");

ServerGUI::ServerGUI(QWidget *parent)
    : QWidget(parent)
    , m_error_message(QErrorMessage::qtHandler())
    , m_server(new Server(this))
    , m_icons(new IconLoader(this))
    , m_layout(new QGridLayout(this))
    , m_menu_bar(new QMenuBar(this))
    , m_menu1(new QMenu("test menu", this))
    , m_text_output(new QPlainTextEdit("Welcome to the server console!\nUse 'help' to print help message.\n", this))
    , m_text_input(new QLineEdit(this))
    , m_status(new QLabel(this))
    , m_users(new QListWidget(this))
    , m_user_menu(new QMenu("User actions popup", this))
    , m_send_button(new QPushButton(QIcon::fromTheme("utilities-terminal"), "Send command", this))
{
    setWindowTitle("Server Console");
    setWindowIcon(QIcon::fromTheme("preferences-system-network-server"));
    setMinimumSize(300, 200);
    resize(500, 400);

    // create layout (by hand)
    m_layout->setMenuBar(m_menu_bar);
    m_layout->addWidget(m_text_output, 0, 0, 3, 2);
    m_layout->addWidget(m_text_input, 3, 0, 1, 2);
    m_layout->addWidget(m_status, 0, 2, 1, 1);
    m_layout->addWidget(new QLabel("Users:", this), 1, 2, 1, 1);
    m_layout->addWidget(m_users, 2, 2, 1, 1);
    m_layout->addWidget(m_send_button, 3, 2, 1, 1);

    // TODO: add actual menus?
    m_menu_bar->addMenu(m_menu1);
    m_menu1->addAction("test action");

    static const int sidebarMaxW = 150;

    m_status->setMaximumWidth(sidebarMaxW);
    m_users->setMaximumWidth(sidebarMaxW);
    m_send_button->setMaximumWidth(sidebarMaxW);

    m_text_output->setReadOnly(true);
    m_text_input->setPlaceholderText("Command...");
    m_text_input->setClearButtonEnabled(true);

    m_users->addItem("Not implemented...");
    m_users->addItem("...yet!");


    m_user_menu->addAction("Show information", this,
    [this] {
        static const QString INFO_CMD("user show %1");
        m_text_input->insert(INFO_CMD.arg(m_users->currentItem()->text()));
    }
                          );

    m_user_menu->addAction("Kick", this,
    [this] {
        static const QString KICK_CMD("user kick %1");
        m_text_input->insert(KICK_CMD.arg(m_users->currentItem()->text()));
    }
                          );

    m_user_menu->addAction("Ban", this,
    [this] {
        static const QString BAN_CMD("user ban %1");
        m_text_input->insert(BAN_CMD.arg(m_users->currentItem()->text()));
    }
                          );

    connect(m_send_button, &QPushButton::clicked, this, &ServerGUI::handleCommand);
    connect(m_text_input, &QLineEdit::returnPressed, this, &ServerGUI::handleCommand);
    connect(m_users, &QListWidget::itemClicked, this, [this] {m_user_menu->popup(QCursor::pos());});

    serverInitCheck();
}

void ServerGUI::serverInitCheck()
{
    connect(m_server, &Server::statusChanged, this, &ServerGUI::handleStatusChange);
    connect(m_server, &Server::connectionOpened, this, &ServerGUI::handleOpenedConnection);
    connect(m_server, &Server::connectionClosed, this, &ServerGUI::handleClosedConnection);

    if (m_server != nullptr and m_server->isAlive()) {
        m_status->setText(STATUS_TEXT.arg(STATUS_ALIVE));
        m_status->setStyleSheet(STATUS_STYLE_ALIVE);
        m_text_output->appendPlainText(QString("Server started successfully! Port: %1").arg(m_server->serverPort()));
    } else {
        m_text_output->appendPlainText("Server startup failed!");
        m_status->setText(STATUS_TEXT.arg(STATUS_DEAD));
        m_status->setStyleSheet(STATUS_STYLE_DEAD);
    }
}

void ServerGUI::handleStatusChange(bool alive)
{
    //if (m_server != nullptr and m_server->isAlive()) {
    if (alive) {
        m_status->setText(STATUS_TEXT.arg(STATUS_ALIVE));
        m_status->setStyleSheet(STATUS_STYLE_ALIVE);
    } else {
        m_status->setText(STATUS_TEXT.arg(STATUS_DEAD));
        m_status->setStyleSheet(STATUS_STYLE_DEAD);
    }
}

void ServerGUI::handleCommand()
{
    QString command = m_text_input->text().toLower().simplified();
    m_text_input->clear();
    m_text_output->appendPlainText(command);

    // figure out command
    if (command == "help") {
        m_text_output->appendPlainText(
            "Available commands:\n"
            "help - print this help text\n"
            "threads - print how many threads the server is using\n"
            "stop - stop the server\n"
            "restart - restart the server"
        );
        return;
    }

    if (command == "threads") {
        if (m_server != nullptr && m_server->isAlive())
            m_text_output->appendPlainText(QString("Connection threads: %1").arg(m_server->serverThreads()));
        else
            m_text_output->appendPlainText("Server not alive");
        return;
    }

    if (command == "stop") {
        m_text_output->appendPlainText("Stopping");
        m_server->deleteLater();
        m_server = nullptr;
        return;
    }

    if (command == "restart") {
        m_text_output->appendPlainText("Restarting");
        if (m_server != nullptr) m_server->deleteLater();
        m_server = new Server(this);
        serverInitCheck();
        return;
    }

    m_text_output->appendPlainText(QString("Unknown command: '%1'").arg(command));
}

void ServerGUI::handleOpenedConnection(QHostAddress addr)
{
    QString addrStr = addr.toString();
    m_text_output->appendPlainText(QString("Opened connection to '%1'").arg(addrStr));
    m_users->addItem(addrStr);
}

void ServerGUI::handleClosedConnection(QHostAddress addr)
{
    QString addrStr = addr.toString();
    m_text_output->appendPlainText(QString("Closed connection to '%1'").arg(addrStr));
    auto list = m_users->findItems(addrStr, Qt::MatchFixedString);
    if (not list.isEmpty()) {
        delete list.first();
    }
}
