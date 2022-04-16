#include "iconloader.h"

#include <QCoreApplication>
#include <QString>
#include <QFile>
#include <QIcon>
#include <QPalette>
#include <QDebug>

IconLoader::IconLoader(QObject *parent)
    : QObject(parent)
{
    // try and find breeze icons
    const QString ICON_PATH_RUNTIME(QCoreApplication::applicationDirPath() + QString("/../usr/share/%1/").arg(QCoreApplication::applicationName()));
    const QString ICON_PATH_DEVELOPMENT(QCoreApplication::applicationDirPath() + "/../src/common/icons/");
    static const QString ICONS_ROOT("/icons/");
    static const QString ICONS_LIGHT_FILE("breeze-icons.rcc");
    static const QString ICONS_DARK_FILE("breeze-icons-dark.rcc");
    static const QString ICONS_LIGHT_THEME("breeze");
    static const QString ICONS_DARK_THEME("breeze-dark");

    QString icons_file;
    QString icons_theme;
    if (QPalette().base().color().lightness() >= 150) {
        icons_file = ICONS_LIGHT_FILE;
        icons_theme = ICONS_LIGHT_THEME;
    } else {
        icons_file = ICONS_DARK_FILE;
        icons_theme = ICONS_DARK_THEME;
    }

    // not sure if I want this
    //QIcon::setThemeName(icons_theme);

    // check install dir first
    if (QFile::exists(ICON_PATH_RUNTIME + icons_file))
        m_icons.registerResource(ICON_PATH_RUNTIME + icons_file, ICONS_ROOT + icons_theme);
    // then check dev dir
    else if (QFile::exists(ICON_PATH_DEVELOPMENT + icons_file))
        m_icons.registerResource(ICON_PATH_DEVELOPMENT + icons_file, ICONS_ROOT + icons_theme);
    else
        qWarning() << "Could not find " << icons_file;
}
