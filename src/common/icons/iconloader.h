#pragma once

#include <QResource>

class IconLoader : public QObject
{
    Q_OBJECT

public:
    IconLoader(QObject *parent = nullptr);

private:
    QResource m_icons;
};
