#ifndef SONG_WINDOW_H
#define SONG_WINDOW_H

#include <QWidget>

namespace Ui {
    class SongWindow;
}

class SongWindow : public QWidget {
    Q_OBJECT
public:
    SongWindow(QWidget *parent = 0);
    ~SongWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::SongWindow *ui;
};

#endif // SONG_WINDOW_H
