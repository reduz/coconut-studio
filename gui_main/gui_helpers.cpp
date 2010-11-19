#include "gui_helpers.h"

QColor get_default_color(const QString& p_string, const QColor& p_default) {

	QSettings settings;
	return settings.value(p_string,p_default).value<QColor>();
}


QFont get_default_font(const QString& p_string, const QFont& p_default) {

	QSettings settings;
	return settings.value(p_string,p_default).value<QFont>();
}
