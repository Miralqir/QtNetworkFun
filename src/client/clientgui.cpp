#include "clientgui.h"

#include <QApplication>
#include <QIcon>
#include <QLocale>
#include <QStatusBar>
//#include <QVBoxLayout>
#include <QSizePolicy>
#include <QIntValidator>
#include <QThread>
#include <QDebug>

const QString welcome_text("# Welcome to %1!\n\nHere are some short tips to get you started:\n\n1. Press the Connect icon from the toolbar above to connect to a new server\n\n");
const QString connect_text("Connect to server");
const QString disconnect_text("Disconnect from server");

ClientGUI::ClientGUI(QWidget *parent)
    : QMainWindow(parent)
    , m_error_message(QErrorMessage::qtHandler())
    , m_client(new Client(this))
    , m_icons(new IconLoader(this))
    , m_dialog(new ServerChooserDialog(this))
    , m_central_widget(new QWidget(this))
    , m_central_layout(new QGridLayout(m_central_widget))
    , m_tab_widget(new QTabWidget())
    , m_welcome_page(new QTextEdit())
    , m_connect_label(new QLabel("Hi!", this))
    , m_main_toolbar(new QToolBar(this))
{
    setWindowTitle(QString("%1 Client").arg(QApplication::applicationName()));
    //setWindowIcon(QIcon::fromTheme("applications-internet"));
    setWindowIcon(QIcon::fromTheme("kopete"));
    setMinimumSize(300, 200);
    resize(500, 400);

    //setCentralWidget(m_central_widget);
    //m_central_layout->addWidget(m_tab_widget, 0, 0, 1, 2);
    // experimental
    setCentralWidget(m_tab_widget);

    m_tab_widget->setMovable(true);
    m_tab_widget->setTabsClosable(true);
    m_tab_widget->addTab(m_welcome_page, "Welcome page");
    connect(m_tab_widget, &QTabWidget::tabCloseRequested, this, &ClientGUI::handleCloseTab);

    m_welcome_page->setReadOnly(true);
    //m_welcome_page->setHtml(welcome_text.arg(QApplication::applicationName()));
    m_welcome_page->setMarkdown(welcome_text.arg(QApplication::applicationName()));
    connect(m_welcome_page, &QObject::destroyed, this, [this] {m_welcome_page = nullptr;});

    addToolBar(m_main_toolbar);
    m_main_toolbar->setMovable(false);
    m_main_toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    m_dis_connect = m_main_toolbar->addAction(QIcon::fromTheme("network-connect"), connect_text, this, &ClientGUI::disConnectButtonHandle);
    //connect(m_dis_connect, &QAction::triggered, this, &ClientUI::disConnectButtonHandle);
    connect(m_dialog, &ServerChooserDialog::requestConnect, this, &ClientGUI::connectHandler);
    connect(m_client, &Client::messageReceived, this, &ClientGUI::handleMessage);

    statusBar()->addWidget(m_connect_label);

    connect(m_client, &QTcpSocket::stateChanged, this, &ClientGUI::handleConnectionStatus);
}

int ClientGUI::getChatTab(const quint64 user_id)
{
    // TODO: add code; stub
    // window should have:
    // QLabel with username
    // QScrollArea with a QWidget with QLabel children for text messages
    // move input bar to individual tabs instead of having it global perhaps? idfk
    
    // step 1: see if tab already exists
    
    QString user_id_s = QString::number(user_id);
    int index = -1;
    for (int i = 0; i < m_tab_widget->count() and index == -1; i++) {
        QLabel *label = m_tab_widget->widget(i)->findChild<QLabel *>();
        
        if (label != nullptr and label->text().endsWith(user_id_s)) {
            index = i;
        }
    }
    
    if (index != -1) {
        return index; // great we found it
    }
    
    // crap we gotta make it
    // step 2: create actual tab
    User *user = m_client->getUserList()->findId(user_id);
    
    QString username;
    if (user != nullptr) {
        username = user->getName();
    } else {
        username = QString("Unknown user");
    }
    
    QWidget *central = new QWidget();
    QGridLayout *layout = new QGridLayout(central);
    QLabel *label = new QLabel(QString("%1 - %2").arg(username).arg(user_id_s));
    QScrollArea *messages = new QScrollArea();
    QWidget *scroll_central = new QWidget();
    QFormLayout *scroll_layout = new QFormLayout(scroll_central);
    QLineEdit *input_text = new QLineEdit();
    QPushButton *input_send = new QPushButton("Send");
    
    messages->setWidget(scroll_central);
    messages->setWidgetResizable(true);
    scroll_layout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    scroll_layout->setFormAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    
    layout->addWidget(label, 0, 0, 1, 2);
    layout->addWidget(messages, 1, 0, 1, 2);
    layout->addWidget(input_text, 2, 0, 1, 1);
    layout->addWidget(input_send, 2, 1, 1, 1);
    
    return m_tab_widget->addTab(central, label->text());
}

void ClientGUI::disConnectButtonHandle()
{
    auto state = m_client->state();
    if (state == QTcpSocket::UnconnectedState) {
        m_dialog->open();
    } else if (state == QTcpSocket::ConnectedState) {
        m_client->close();
    } else {
        m_client->abort();
    }
}

void ClientGUI::connectHandler(QString host, QString port)
{
    m_client->connectToHost(host, port.toUInt());
}

void ClientGUI::handleConnectionStatus(QTcpSocket::SocketState socket_state)
{
    if (socket_state == QTcpSocket::UnconnectedState) {
        m_dis_connect->setIcon(QIcon::fromTheme("network-connect"));
        m_dis_connect->setText(connect_text);
    } else {
        m_dis_connect->setIcon(QIcon::fromTheme("network-disconnect"));
        m_dis_connect->setText(disconnect_text);
    }

    switch (socket_state) {
    case QTcpSocket::UnconnectedState:
        m_connect_label->setText("Not connected to server");
        break;
    case QTcpSocket::HostLookupState:
        m_connect_label->setText("Performing host lookup...");
        break;
    case QTcpSocket::ConnectingState:
        m_connect_label->setText("Connecting...");
        break;
    case QTcpSocket::ConnectedState:
        m_connect_label->setText(QString("Connected to '%1'!").arg(m_client->peerName()));
        break;
    case QTcpSocket::BoundState:
        m_connect_label->setText("Client is bound to a specific address or port");
        break;
    case QTcpSocket::ClosingState:
        m_connect_label->setText("Connection is closing...");
        break;
    default:
        m_connect_label->setText(QString("Unknown connection status: '%1'").arg(socket_state));
        break;
    }
}

void ClientGUI::handleMessage(Message *message)
{
    // TODO: check for groups someday
    if (message->getTo() != 1 and message->getTo() != m_client->getUser()->getId()) {
        qDebug() << "alien message from " << message->getFrom();
        return;
    }
    
    int index = getChatTab(message->getFrom());
    QFormLayout *scroll_layout = m_tab_widget->widget(index)->findChild<QFormLayout *>();
    QLabel *data_label = new QLabel(message->getData());
    QLabel *timestamp_label = new QLabel(QLocale::system().toString(message->getTimestamp().toLocalTime(), QLocale::system().timeFormat(QLocale::ShortFormat)));
    data_label->setWordWrap(true);
    timestamp_label->setWordWrap(true);
    timestamp_label->setToolTip(QLocale::system().toString(message->getTimestamp().toLocalTime(), QLocale::system().dateFormat()));
    // TODO: add spacer in between messages separated by a long time
    scroll_layout->addRow(timestamp_label, data_label);
}


void ClientGUI::handleCloseTab(int index)
{
    if (m_tab_widget->count() > 1) {
        m_tab_widget->widget(index)->deleteLater();
        m_tab_widget->removeTab(index);
    }
}

ServerChooserDialog::ServerChooserDialog(QWidget *parent)
    : QDialog(parent)
    , m_layout(new QFormLayout(this))
    , m_host_label(new QLabel("Hostname:", this))
    , m_port_label(new QLabel("Port:", this))
    , m_host_input(new QComboBox(this))
    , m_port_input(new QComboBox(this))
    , m_return(new QPushButton("Connect", this))
{
    setWindowTitle("Server Chooser");
    setWindowIcon(QIcon::fromTheme("kopete"));
    setModal(true);

    m_layout->addRow(m_host_label, m_host_input);
    m_layout->addRow(m_port_label, m_port_input);
    m_layout->addRow(m_return);

    m_host_input->setEditable(true);
    m_host_input->addItem("vintilammm.ddns.net");
    m_host_input->addItem("localhost");
    m_port_input->setEditable(true);
    m_port_input->addItem("9000");
    m_port_input->setValidator(new QIntValidator(1, 65535, this));

    connect(m_return, &QPushButton::clicked, this, [this] {emit requestConnect(m_host_input->currentText(), m_port_input->currentText()); done(QDialog::Accepted);});
}
