#ifndef PATTERN_EDITOR_H
#define PATTERN_EDITOR_H

#include <QWidget>
#include "engine/song.h"
#include <QScrollBar>

class PatternEditor : public QWidget
{
	Q_OBJECT

	void paintEvent(QPaintEvent *);

	QFont default_font;
	Song *song;

	int zoom;
	int pattern;

	int compute_width();

	QScrollBar *h_scroll;
	QScrollBar *v_scroll;

signals:

public slots:

	void scrollChanged(int);

public:

	void set_song(Song *song);
	void set_h_scroll(QScrollBar *h_scroll);
	void set_v_scroll(QScrollBar *v_scroll);


	explicit PatternEditor(QWidget *parent = 0);

};

#endif // PATTERN_EDITOR_H
