#ifndef SONG_WINDOW_H
#define SONG_WINDOW_H

#include <QWidget>
#include "engine/song.h"
namespace Ui {
    class SongWindow;
}

class SongWindow : public QWidget {
    Q_OBJECT
private:
	Ui::SongWindow *ui;


	Song song;

protected:
    void changeEvent(QEvent *e);


public:



	SongWindow(QWidget *parent = 0);
	~SongWindow();

};

#endif // SONG_WINDOW_H
