#include "song_window.h"
#include "ui_song_window.h"

SongWindow::SongWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SongWindow)
{
    ui->setupUi(this);

    ui->pattern_editor->set_h_scroll(ui->pattern_h_scroll);
    ui->pattern_editor->set_v_scroll(ui->pattern_v_scroll);
    ui->pattern_editor->set_song(&song);

}

SongWindow::~SongWindow()
{
    delete ui;
}

void SongWindow::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
