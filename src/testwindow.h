#pragma once

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QSlider>
#include <QLCDNumber>
//#include <QScopedPointer>

class TestWindow : public QWidget
{
    Q_OBJECT

public:
    TestWindow(QWidget *parent = nullptr);
    ~TestWindow() override;

private:
    QVBoxLayout *m_layout;
    QSlider *m_slider;
    QLCDNumber *m_display;
};
