#include "main_window.h"
#include "ui_main_window.h"
#include "song_window.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	SongWindow *sw = new SongWindow(ui->mdiArea);
	ui->mdiArea->addSubWindow(sw);
}

MainWindow::~MainWindow()
{

	delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
		break;
		default:
		break;
	}
}
