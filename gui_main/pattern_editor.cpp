#include "pattern_editor.h"
#include <QPainter>
#include <QSettings>
#include "gui_main/gui_helpers.h"
#include "engine/pattern_track.h"
#include <stdio.h>

int PatternEditor::compute_width() {
#if 0
	int w=0;
	QFont f = get_default_font("pattern_editor/font",default_font);
	QFontMetrics fm(f);
	int fw = fm.width('X');
	int fh = fm.height();

	for(int i=0;i<song->get_track_count();i++) {

		Track *t = song->get_track(i);
		w+=fh+fw*2; //separate

		switch(t->get_type()) {


			case Track::TYPE_GLOBAL: { break; }
			case Track::TYPE_PATTERN: {

				PatternTrack *pt = dynamic_cast<PatternTrack*>(t);

				int note_colums = pt->get_note_columns();
				w+=note_colums*fw*4;

				int command_colums = pt->get_command_columns();
				w+=command_colums*fw*4;

			} break;
		}


		for(int i=0;i<t->get_automation_count();i++) {

			Automation *a = t->get_automation(i);
			if (!a->is_visible())
				continue;

			switch(a->get_display_mode()) {
				case Automation::DISPLAY_ROWS:
				case Automation::DISPLAY_SMALL: w+=fw*4; break;
				case Automation::DISPLAY_LARGE: w+=fw*7; break;
			}
		}
	}

#endif
}


void PatternEditor::scrollChanged(int) {

	update();
}

void PatternEditor::paintEvent(QPaintEvent *) {

	QPainter painter(this);
	QSettings settings;

	QColor bg_color = get_default_color("pattern_editor/bg_color",QColor(0,0,0));
	QColor time_zoom_color = get_default_color("pattern_editor/time_beat_color",QColor(80,80,80));
	QColor time_beat_color = get_default_color("pattern_editor/time_beat_color",QColor(160,160,160));
	QColor time_bar_color = get_default_color("pattern_editor/time_beat_color",QColor(255,255,255));
	int time_w = settings.value("pattern_editor/time_width",4).toInt();


	QFont f = get_default_font("pattern_editor/font",default_font);

	painter.setFont(f);

	QFontMetrics fm(f);
	int fw = fm.width('X');
	int fh = fm.height();
	int row_h = fh + settings.value("pattern_editor/vseparator",2).toInt();


	painter.fillRect(0,0,width(),height(),bg_color);

	int available_w = width() - fw * 5; // 5?
	int max_w = compute_width();

	int pattern_bpb = song->pattern_get_beats_per_bar(pattern);
	int pattern_bars = song->pattern_get_bars(pattern);

	int rows = zoom * pattern_bars * pattern_bpb;
	int visible_rows = height() / row_h;

	//h_scroll->setMaximum(max_w);
	//h_scroll->setPageStep(available_w);
	v_scroll->setMinimum(0);
	v_scroll->setMaximum(MAX(0,rows-visible_rows));
	v_scroll->setPageStep(visible_rows);

	int ofs_x = h_scroll->value();
	int ofs_y = v_scroll->value();

	//paint bar/beat/bar markers - always

	for(int i=0;i<visible_rows;i++) {

		int idx = i + ofs_y;
		int x=0;
		QColor color;
		QString text;

		if (idx%(zoom*pattern_bpb) == 0) {
			// bar
			text = QString::number(1+idx/zoom/pattern_bpb);
			color = time_bar_color;

		} else if (idx%(zoom) == 0) {
			//beat
			int n = 1+(idx/zoom)%pattern_bpb;
			text = QString::number(n);
			color = time_beat_color;
			x = time_w-2;
			if (n>=10)
				x-=1;
			if (n>=100)
				x-=1;
		} else  {
			//tick
			int n = 1+idx%zoom;
			text = QString::number(n);
			color = time_zoom_color;
			x = time_w-1;
			if (n>=10)
				x-=1;
			if (n>=100)
				x-=1;
		}

		x*=fw;

		painter.setPen(color);
		painter.drawText(x,i*row_h+fm.ascent()+(row_h-fh)/2,text);
		painter.drawLine(0,i*row_h,time_w*fw,i*row_h);

	}

	//draw all tracks (At least the visible ones)
	int x = (time_w + 1 ) * fw;

#if 0
	for(int i=0;i<song->get_track_count();i++) {

		Track *t = song->get_track(i);
//		w+=fh+fw*2; //separate



		for(int j=0;j<visible_rows;j++) {

			int idx = i + ofs_y;
			int lx=0;
			QColor color;
			QString text;
			Tick from_tick = idx * TICKS_PER_BEAT / zoom;
			Tick to_tick = (idx+1) * TICKS_PER_BEAT / zoom;

			switch(t->get_type()) {


				case Track::TYPE_GLOBAL: { break; }
				case Track::TYPE_PATTERN: {


					PatternTrack *pt = dynamic_cast<PatternTrack*>(t);

					int note_colums = pt->get_note_columns();

					for(int k=0;k<note_colums;k++) {

						List<PatternTrack::PosNote> notes;
						PatternTrack::Pos from,to;
						from.column=k;
						from.tick=from_tick;
						from.column=k;
						to.tick=to_tick;
						pt->get_notes_in_range(pattern,from,to,&notes);

						if (notes.size()>1) {

							//draw multiple notes! (zoom too large)

						} else if (notes.size()==1) {

							//draw single note
						}
					}

					int command_colums = pt->get_command_columns();
					//w+=command_colums*fw*4;

				} break;
			}
		}

/*
		for(int i=0;i<t->get_automation_count();i++) {

			Automation *a = t->get_automation(i);
			if (!a->is_visible())
				continue;

			switch(a->get_display_mode()) {
				case Automation::DISPLAY_ROWS:
				case Automation::DISPLAY_SMALL: w+=fw*4; break;
				case Automation::DISPLAY_LARGE: w+=fw*7; break;
			}
		}
		*/
	}
#endif


}



void PatternEditor::set_song(Song *p_song) {
	song=p_song;
}


void PatternEditor::set_h_scroll(QScrollBar *p_h_scroll) {

	h_scroll=p_h_scroll;
}

void PatternEditor::set_v_scroll(QScrollBar *p_v_scroll) {

	v_scroll=p_v_scroll;
	connect(v_scroll,SIGNAL(valueChanged(int)),this,SLOT(scrollChanged(int)));
}


PatternEditor::PatternEditor(QWidget *parent) :
    QWidget(parent)    
{

	zoom=4;
	pattern=0;
	default_font = QFont("fixed",10);
}
