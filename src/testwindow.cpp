#include "testwindow.h"

#include <QIcon>
#include <QDebug>

TestWindow::TestWindow(QWidget *parent) 
    : QWidget(parent)
    , m_layout(new QVBoxLayout(this))
    , m_slider(new QSlider(Qt::Horizontal, this))
    , m_display(new QLCDNumber(this))
{
    setWindowTitle(QString("%1 test").arg(QApplication::applicationName()));
    setWindowIcon(QIcon::fromTheme("diaspora"));
    //setGeometry(100, 100, 500, 500);
    setMinimumSize(300, 300);

    connect(m_slider, &QSlider::valueChanged, m_display, qOverload<int>(&QLCDNumber::display));

    m_layout->addWidget(m_slider);
    m_layout->addWidget(m_display);

    qDebug() << QApplication::applicationName();
}

TestWindow::~TestWindow() = default;
