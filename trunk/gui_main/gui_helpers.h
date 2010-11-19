#ifndef GUI_HELPERS_H
#define GUI_HELPERS_H

#include <QSettings>
#include <QColor>
#include <QFont>


QColor get_default_color(const QString& p_string, const QColor& p_default);
QFont get_default_font(const QString& p_string, const QFont& p_default);

#endif // GUI_HELPERS_H
