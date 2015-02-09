#include "pattern_editor.h"
#include "gui/base/painter.h"
#include "cskin.h"
#include <stdio.h>
#include "key_bindings.h"


bool PatternEditor::key(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask) {


	if (!p_press)
		return false;

	unsigned long sc = p_scan_code|p_modifier_mask;

	if (sc==KeyBind::get_keybind_code(KB_CURSOR_MOVE_UP)) {
		cursor.row-=cursor.skip;
		_validate_cursor();
	} else if (sc==KeyBind::get_keybind_code(KB_CURSOR_MOVE_DOWN)) {
		cursor.row+=cursor.skip;
		_validate_cursor();
	} else if (sc==KeyBind::get_keybind_code(KB_CURSOR_MOVE_UP_1_ROW)) {
		cursor.row-=1;
		_validate_cursor();
	} else if (sc==KeyBind::get_keybind_code(KB_CURSOR_MOVE_DOWN_1_ROW)) {
		cursor.row+=1;
		_validate_cursor();
	} else if (sc==KeyBind::get_keybind_code(KB_CURSOR_PAGE_UP)) {
		cursor.row-=song->pattern_get_beats_per_bar(current_pattern)*rows_per_beat;
		_validate_cursor();
	} else if (sc==KeyBind::get_keybind_code(KB_CURSOR_PAGE_DOWN)) {
		cursor.row+=song->pattern_get_beats_per_bar(current_pattern)*rows_per_beat;
		_validate_cursor();
	} else {
		return false;
	}

	return true;
}

void PatternEditor::_validate_cursor() {

	if (cursor.row<0)
		cursor.row=0;
	else if (cursor.row>=get_total_rows())
		cursor.row=get_total_rows()-1;

	if (cursor.row<v_offset)
		v_offset=cursor.row;
	if (cursor.row>=v_offset+visible_rows)
		v_offset=cursor.row-visible_rows+1;

	if (cursor.column<h_offset) {
		h_offset=cursor.column;
	}

	update();

}

int PatternEditor::get_total_rows() const {

	return song->pattern_get_beats(current_pattern)*rows_per_beat;
}

void PatternEditor::mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask) {

	if (p_press && p_button==1) {

		//test trackbuttons
		for(List<TrackButton>::Element *E=track_buttons.front();E;E=E->next()) {

			if (E->get().r.has_point(p_pos)) {
				track_menu->set_pos(get_global_pos()+E->get().r.pos+Point(0,E->get().r.size.y));
				track_menu->clear();

				if (song->get_track(E->get().track)->get_column_count()<16)
					track_menu->add_item("Add Column",TRACK_MENU_ADD_COLUMN);
				if (song->get_track(E->get().track)->get_column_count()>1)
					track_menu->add_item("Remove Column",TRACK_MENU_REMOVE_COLUMN);
				track_menu->add_separator();
				track_menu->add_item("Solo",TRACK_MENU_SOLO,NULL,KeyBind::get_keybind_code(KB_PATTERN_SOLO_TOGGLE_CHANNEL),false);
				track_menu->add_check_itemsc("Muted",TRACK_MENU_MUTE,song->get_track(E->get().track)->is_muted(),KeyBind::get_keybind_code(KB_PATTERN_MUTE_TOGGLE_CHANNEL));
				track_menu->add_separator();
				track_menu->add_item("Automations..",TRACK_MENU_EDIT_AUTOMATIONS);
				track_menu->add_separator();
				track_menu->add_item("Rename..",TRACK_MENU_RENAME);
				if (song->get_track_count()>1) {
					track_menu->add_separator();
					if (E->get().track>0)
						track_menu->add_item("Move Left",TRACK_MENU_MOVE_LEFT);
					if (E->get().track<song->get_track_count()-1)
						track_menu->add_item("Move Right",TRACK_MENU_MOVE_RIGHT);
				}
				track_menu->add_separator();
				track_menu->add_item("Remove",TRACK_MENU_REMOVE);
				track_menu->popup();
				current_menu_track=E->get().track;
				return;
			}
		}

		for(List<AutomationButton>::Element *E=automation_buttons.front();E;E=E->next()) {

			if (E->get().r.has_point(p_pos)) {
				auto_menu->set_pos(get_global_pos()+E->get().r.pos+Point(0,E->get().r.size.y));
				auto_menu->clear();
				auto_menu->add_check_item("Visible",AUTOMATION_MENU_VISIBLE,song->get_track(E->get().track)->get_automation(E->get().automation)->is_visible());
				auto_menu->add_separator();
				auto_menu->add_check_item("Numbers",AUTOMATION_MENU_MODE_ROWS,song->get_track(E->get().track)->get_automation(E->get().automation)->get_display_mode()==Automation::DISPLAY_ROWS);
				auto_menu->add_check_item("Small Envelope",AUTOMATION_MENU_MODE_SMALL,song->get_track(E->get().track)->get_automation(E->get().automation)->get_display_mode()==Automation::DISPLAY_SMALL);
				auto_menu->add_check_item("Large Envelope",AUTOMATION_MENU_MODE_LARGE,song->get_track(E->get().track)->get_automation(E->get().automation)->get_display_mode()==Automation::DISPLAY_LARGE);
				if (song->get_track(E->get().track)->get_automation_count()>1) {
					auto_menu->add_separator();
					if (E->get().automation>0)
						auto_menu->add_item("Move Left",AUTOMATION_MENU_MOVE_LEFT);
					if (E->get().automation<song->get_track(E->get().track)->get_automation_count()-1)
						auto_menu->add_item("Move Right",AUTOMATION_MENU_MOVE_RIGHT);
				}
				auto_menu->add_separator();
				auto_menu->add_item("Remove",AUTOMATION_MENU_REMOVE);
				auto_menu->popup();
				current_menu_track=E->get().track;
				current_menu_automation=E->get().automation;
				return;
			}
		}
	}
}



int PatternEditor::get_visible_rows() const {

	return visible_rows;

}

void PatternEditor::draw(const Point& p_global,const Size& p_size,const Rect& p_exposed) {

	Painter *p = get_painter();

	FontID f = font(FONT_PATTERN_EDITOR_ROWS);
	FontID fcol = font(FONT_PATTERN_EDITOR_COLUMN_NAME);
	int fw = p->get_font_char_width( f, 'X' );
	int fh = p->get_font_height( f );
	int fa = p->get_font_ascent( f );
	int sep=1;
	fh+=sep;
	int top_ofs=4;



	Color track_sep_color = color(COLOR_PATTERN_EDITOR_TRACK_SEPARATOR);
	Color cursorcol = color(COLOR_PATTERN_EDITOR_CURSOR);
	int track_sep_w = constant(C_PATTERN_EDITOR_TRACK_SEPARATION);

	track_buttons.clear();
	automation_buttons.clear();



	visible_rows = (p_size.height-top_ofs)/fh;

	int beats_per_bar=song->pattern_get_beats_per_bar(current_pattern);

	for(int i=0;i<visible_rows;i++) {

		int row = v_offset+i;
		int beat = row / rows_per_beat;
		int subbeat = row % rows_per_beat;

		if (subbeat==0 || i==0) {
			if (beat % beats_per_bar == 0)
				p->draw_text(f,Point(0,top_ofs+i*fh+fa),String::num(beat),color(COLOR_PATTERN_EDITOR_ROW_BAR));
			else
				p->draw_text(f,Point(0,top_ofs+i*fh+fa),String::num(beat),color(COLOR_PATTERN_EDITOR_ROW_BEAT));

		} else {
			char text[3]={
				'0'+(subbeat/10),
				'0'+(subbeat%10),
				0
			};
			p->draw_text(f,Point(fw,top_ofs+i*fh+fa),text,color(COLOR_PATTERN_EDITOR_ROW_SUB_BEAT));
		}
	}

	int ofs=fw*4;
	Color bgcol=color(COLOR_PATTERN_EDITOR_BG);
	p->draw_fill_rect(Point(ofs,0),Point(p_size.x-ofs,p_size.y),bgcol);

	int idx=0;

	for(int i=0;i<song->get_track_count();i++) {

		Track *t=song->get_track(i);

		bool drawn=false;

		for(int j=0;j<t->get_column_count();j++) {

			if (idx<h_offset) {
				idx++;
				continue;
			}
			if (j==0) {

				int as = p->get_font_height(fcol);
				p->draw_arrow(Point(ofs,top_ofs),Size(as,as),DOWN,color(COLOR_PATTERN_EDITOR_TRACK_NAME));
				p->draw_text(fcol,Point(ofs+p->get_font_height(fcol)-p->get_font_ascent(fcol),top_ofs+as),t->get_name(),DOWN,color(COLOR_PATTERN_EDITOR_TRACK_NAME));
				TrackButton tb;
				tb.track=i;

				tb.r=Rect(Point(ofs,top_ofs),Point(as,as));
				track_buttons.push_back(tb);
				ofs+=p->get_font_height(fcol);
			}

			drawn=true;
			int extrahl=(j<t->get_column_count()-1)?fw:0;

			for(int k=0;k<visible_rows;k++) {

				char rowstr[7]={
					'.','.','.',' ','.','.',0
				};

				int row = v_offset+k;
				int beat = row / rows_per_beat;
				int subbeat = row % rows_per_beat;

				Color c=color(COLOR_PATTERN_EDITOR_NOTE);
				Color bgc=bgcol;



				if (subbeat==0 || k==0) {
					if ((beat % beats_per_bar) == 0)
						p->draw_fill_rect(Point(ofs,top_ofs+k*fh-sep),Size(fw*6+extrahl,fh),color(COLOR_PATTERN_EDITOR_HL_BAR));
					else
						p->draw_fill_rect(Point(ofs,top_ofs+k*fh-sep),Size(fw*6+extrahl,fh),color(COLOR_PATTERN_EDITOR_HL_BEAT));

				}

				Track::Pos from,to;
				from.tick=row*TICKS_PER_BEAT/rows_per_beat;
				from.column=j;
				Tick to_tick = (row+1)*TICKS_PER_BEAT/rows_per_beat;
				to.tick=to_tick;
				to.column=j;


				List<Track::PosNote> pn;

				t->get_notes_in_range(current_pattern,from,to,&pn);
				Vector<Track::PosNote> valid;
				for (List<Track::PosNote>::Element *E=pn.front();E;E=E->next()) {

					if (E->get().pos.column!=j || E->get().pos.tick>=to_tick)
						continue;
					valid.push_back(E->get());
				}

				if (valid.size()==0) {

					p->draw_text(f,Point(ofs,top_ofs+k*fh+fa),rowstr,c);
				} else if (valid.size()==1) {

					Track::PosNote n=pn.front()->get();
					if (n.pos.tick!=from.tick)
						c=color(COLOR_PATTERN_EDITOR_NOTE_NOFIT);
					if (n.note.note==Track::Note::OFF) {
						rowstr[0]='=';
						rowstr[1]='=';
						rowstr[2]='=';
					} else if (n.note.note<120) {
						static const char* note[12]={
							"C-","C#","D-","D#","E-","F-","F#","G-","G#","A-","A#","B-"
						};
						rowstr[0]=note[n.note.note%12][0];
						rowstr[1]=note[n.note.note%12][1];
						rowstr[2]='0'+n.note.note/12; //octave
					}

					if (n.note.volume<100) {
						rowstr[4]='0'+n.note.volume/10;
						rowstr[5]='0'+n.note.volume%10;
					}

					p->draw_text(f,Point(ofs,top_ofs+k*fh+fa),rowstr,c);

				} else {

					Point base = Point(ofs,top_ofs+k*fh);

					for(int l=0;l<valid.size();l++) {

						int h = (fh-2)/valid.size();
						int w = fw*3-2;
						int vw = fw*2-2;

						if (valid[l].note.note<120) {
							p->draw_fill_rect(base+Point(0,h*l),Size(fw*3,h-1),c);
							p->draw_rect(base+Point(valid[l].note.note*w/120,1+h*l),Size(2,h-2),bgc);
						} else if (valid[l].note.note==Track::Note::OFF) {
							p->draw_rect(base+Point(0,1+h*l),Size(fw*3,1),c);
							p->draw_rect(base+Point(0,1+h*l+h-2),Size(fw*3,1),c);
						}

						if (valid[l].note.volume<100) {
							p->draw_fill_rect(base+Point(fw*4,h*l),Size(fw*2,h-1),c);
							p->draw_rect(base+Point(fw*4+valid[l].note.volume*vw/100,1+h*l),Size(2,h-2),bgc);
						}

					}
				}

				if (has_focus() && idx==cursor.column && cursor.row==row) {

					int field_ofs[4]={0,2,4,5};
					Point cursor_pos(ofs+field_ofs[cursor.field]*fw,top_ofs+k*fh-sep);
					p->draw_rect(cursor_pos-Point(1,1),Size(fw+1,fh+1),cursorcol);

				}

			}

			if (j<t->get_column_count()-1)
				ofs+=constant(C_PATTERN_EDITOR_COLUMN_SEPARATION);
			ofs+=fw*6;
			idx++;

		}


		for(int j=0;j<t->get_automation_count();j++) {

			Automation *a = t->get_automation(j);

			if (idx<h_offset) {
				if (a->is_visible())
					idx++;
				continue;
			}

			{
				int as = p->get_font_height(fcol);
				p->draw_arrow(Point(ofs,top_ofs),Size(as,as),DOWN,color(COLOR_PATTERN_EDITOR_AUTOMATION_NAME));
				p->draw_text(fcol,Point(ofs+p->get_font_height(fcol)-p->get_font_ascent(fcol),top_ofs+as),a->get_control_port()->get_name(),DOWN,color(COLOR_PATTERN_EDITOR_AUTOMATION_NAME));

				AutomationButton ab;
				ab.track=i;
				ab.automation=j;
				ab.r=Rect(Point(ofs,top_ofs),Point(as,as));
				automation_buttons.push_back(ab);

			}
			ofs+=p->get_font_height(fcol);

			if (!a->is_visible()) {
				continue;
			}

			switch(a->get_display_mode()) {
				case Automation::DISPLAY_ROWS: {

					for(int k=0;k<visible_rows;k++) {

						char rowstr[3]={
							'.','.',0
						};

						int row = v_offset+k;
						int beat = row / rows_per_beat;
						int subbeat = row % rows_per_beat;

						Color c=color(COLOR_PATTERN_EDITOR_AUTOMATION_VALUE);
						Color bgc=bgcol;


						if (subbeat==0 || k==0) {
							if ((beat % beats_per_bar) == 0)
								p->draw_fill_rect(Point(ofs,top_ofs+k*fh-sep),Size(fw*2,fh),color(COLOR_PATTERN_EDITOR_AUTOMATION_HL_BAR));
							else
								p->draw_fill_rect(Point(ofs,top_ofs+k*fh-sep),Size(fw*2,fh),color(COLOR_PATTERN_EDITOR_AUTOMATION_HL_BEAT));

						}

						Tick from =row*TICKS_PER_BEAT/rows_per_beat;
						Tick to = (row+1)*TICKS_PER_BEAT/rows_per_beat;

						int first;
						int count;

						a->get_points_in_range(current_pattern,from,to,first,count);


						if (count==0) {

							p->draw_text(f,Point(ofs,top_ofs+k*fh+fa),rowstr,c);
						} else if (count==1) {

							int val = a->get_point_by_index(current_pattern,first);
							val=val*99/255;

							if (a->get_point_tick_by_index(current_pattern,first)!=from)
								c=color(COLOR_PATTERN_EDITOR_AUTOMATION_VALUE_NOFIT);

							rowstr[0]='0'+val/10;
							rowstr[1]='0'+val%10;

							p->draw_text(f,Point(ofs,top_ofs+k*fh+fa),rowstr,c);

						} else {

							Point base = Point(ofs,top_ofs+k*fh);

							for(int l=0;l<count;l++) {

								int h = (fh-2)/count;
								int w = fw*2-2;
								int val = a->get_point_by_index(current_pattern,first+l);

								p->draw_fill_rect(base+Point(0,h*l),Size(fw*2,h-1),c);
								p->draw_rect(base+Point(val*w/255,1+h*l),Size(2,h-2),bgc);
							}
						}

					}

					ofs+=fw*2;

				} break;
				case Automation::DISPLAY_SMALL:
				case Automation::DISPLAY_LARGE: {

					int w = a->get_display_mode()==Automation::DISPLAY_SMALL?4:8;
					w*=fw;

					Color c=color(COLOR_PATTERN_EDITOR_AUTOMATION_VALUE);
					Color cpoint=color(COLOR_PATTERN_EDITOR_AUTOMATION_POINT);

					for(int k=0;k<visible_rows;k++) {

						int row = v_offset+k;
						int beat = row / rows_per_beat;
						int subbeat = row % rows_per_beat;

						if (subbeat==0 || k==0) {
							if ((beat % beats_per_bar) == 0)
								p->draw_fill_rect(Point(ofs,top_ofs+k*fh-sep),Size(w,fh),color(COLOR_PATTERN_EDITOR_AUTOMATION_HL_BAR));
							else
								p->draw_fill_rect(Point(ofs,top_ofs+k*fh-sep),Size(w,fh),color(COLOR_PATTERN_EDITOR_AUTOMATION_HL_BEAT));

						} else {

							p->draw_fill_rect(Point(ofs,top_ofs+k*fh-sep+fh),Size(w,1),color(COLOR_PATTERN_EDITOR_AUTOMATION_HL_BEAT));
							p->draw_fill_rect(Point(ofs,top_ofs+k*fh-sep),Size(1,fh),color(COLOR_PATTERN_EDITOR_AUTOMATION_HL_BEAT));
							p->draw_fill_rect(Point(ofs+w-1,top_ofs+k*fh-sep),Size(1,fh),color(COLOR_PATTERN_EDITOR_AUTOMATION_HL_BEAT));
						}

						float prev=-1;
						for(int l=0;l<fh;l++) {

							Tick at = (fh*row+l)*(TICKS_PER_BEAT/rows_per_beat)/fh;

							float tofs=a->interpolate_offset(current_pattern,at);
							if (prev==-1)
								prev=tofs;

							if (tofs>=0)
								p->draw_fill_rect(Point(ofs+tofs*(w-2),top_ofs+k*fh-sep+l),Size(2,1),c);

							prev=tofs;
						}


					}

					Tick pfrom=(v_offset)*(TICKS_PER_BEAT/rows_per_beat);
					Tick pto=(v_offset+visible_rows)*(TICKS_PER_BEAT/rows_per_beat);
					int first;
					int count;
					a->get_points_in_range(current_pattern,pfrom,pto,first,count);
					for(int l=first;l<count;l++) {
						int x=(a->get_point_by_index(current_pattern,l)/255.0)*(w-2);
						int y=a->get_point_tick_by_index(current_pattern,l)*fh/(TICKS_PER_BEAT/rows_per_beat);
						p->draw_fill_rect(Point(ofs+x-2,top_ofs+y-2-sep),Size(5,5),cpoint);

					}

					ofs+=w;
				} break;

			}

			idx++;
		}

		if (drawn) {

			p->draw_fill_rect(Point(ofs,0),Point(track_sep_w,p_size.y),track_sep_color);
			ofs+=track_sep_w;
		}

	}

	if (has_focus())
		p->draw_rect(Point(),p_size,color(COLOR_PATTERN_EDITOR_CURSOR));

}


void PatternEditor::set_current_pattern(int p_pattern) {

	current_pattern=p_pattern;
	update();
}

int PatternEditor::get_current_pattern() const {

	return current_pattern;
}

void PatternEditor::set_in_window() {

	track_menu = new PopUpMenu(get_window());
	track_menu->selected_id_signal.connect(this,&PatternEditor::_menu_option);
	track_menu->checked_id_signal.connect(this,&PatternEditor::_menu_optionc);

	auto_menu = new PopUpMenu(get_window());
	auto_menu->selected_id_signal.connect(this,&PatternEditor::_menu_option);
	auto_menu->checked_id_signal.connect(this,&PatternEditor::_menu_optionc);

}

void PatternEditor::_redraw() {

	update();
}
void PatternEditor::_menu_optionc(int p_option,bool) {

	_menu_option(p_option);
}

void PatternEditor::_menu_option(int p_option) {

	switch(p_option) {

		case TRACK_MENU_ADD_COLUMN: {

			undo_redo->begin_action("Add Column");
			undo_redo->do_method(song->get_track(current_menu_track),&Track::set_columns,song->get_track(current_menu_track)->get_column_count()+1);
			undo_redo->undo_method(song->get_track(current_menu_track),&Track::set_columns,song->get_track(current_menu_track)->get_column_count());
			undo_redo->do_method(this,&PatternEditor::_redraw);
			undo_redo->undo_method(this,&PatternEditor::_redraw);
			undo_redo->commit_action();
		} break;
		case TRACK_MENU_REMOVE_COLUMN: {

			undo_redo->begin_action("Remove Column");
			undo_redo->do_method(song->get_track(current_menu_track),&Track::set_columns,song->get_track(current_menu_track)->get_column_count()-1);
			undo_redo->undo_method(song->get_track(current_menu_track),&Track::set_columns,song->get_track(current_menu_track)->get_column_count());
			undo_redo->do_method(this,&PatternEditor::_redraw);
			undo_redo->undo_method(this,&PatternEditor::_redraw);
			undo_redo->commit_action();


		} break;
		case TRACK_MENU_SOLO: {

			undo_redo->begin_action("Solo");
			for(int i=0;i<song->get_track_count();i++) {

				undo_redo->do_method(song->get_track(i),&Track::set_muted,i!=current_menu_track);
				undo_redo->undo_method(song->get_track(i),&Track::set_muted,song->get_track(i)->is_muted());

			}
			undo_redo->do_method(this,&PatternEditor::_redraw);
			undo_redo->undo_method(this,&PatternEditor::_redraw);
			undo_redo->commit_action();

		} break;
		case TRACK_MENU_MUTE: {

			undo_redo->begin_action("Mute");
			undo_redo->do_method(song->get_track(current_menu_track),&Track::set_muted,!song->get_track(current_menu_track)->is_muted());
			undo_redo->undo_method(song->get_track(current_menu_track),&Track::set_muted,song->get_track(current_menu_track)->is_muted());
			undo_redo->do_method(this,&PatternEditor::_redraw);
			undo_redo->undo_method(this,&PatternEditor::_redraw);
			undo_redo->commit_action();

		} break;
		case TRACK_MENU_EDIT_AUTOMATIONS: {


		} break;
		case TRACK_MENU_RENAME: {


		} break;
		case TRACK_MENU_MOVE_LEFT: {

			undo_redo->begin_action("Track Move Left");
			undo_redo->do_method(song,&Song::swap_tracks,current_menu_track,current_menu_track-1);
			undo_redo->undo_method(song,&Song::swap_tracks,current_menu_track,current_menu_track-1);
			undo_redo->do_method(this,&PatternEditor::_redraw);
			undo_redo->undo_method(this,&PatternEditor::_redraw);
			undo_redo->commit_action();

		} break;
		case TRACK_MENU_MOVE_RIGHT: {

			undo_redo->begin_action("Track Move Right");
			undo_redo->do_method(song,&Song::swap_tracks,current_menu_track,current_menu_track+1);
			undo_redo->undo_method(song,&Song::swap_tracks,current_menu_track,current_menu_track+1);
			undo_redo->do_method(this,&PatternEditor::_redraw);
			undo_redo->undo_method(this,&PatternEditor::_redraw);
			undo_redo->commit_action();

		} break;
		case TRACK_MENU_REMOVE: {

			undo_redo->begin_action("Remove");
			undo_redo->do_method(song,&Song::remove_track,current_menu_track);
			undo_redo->undo_method(song,&Song::add_track_at_pos,song->get_track(current_menu_track),current_menu_track);
			undo_redo->do_method(this,&PatternEditor::_redraw);
			undo_redo->undo_method(this,&PatternEditor::_redraw);
			undo_redo->undo_data(song->get_track(current_menu_track));
			undo_redo->commit_action();

		} break;
		case AUTOMATION_MENU_VISIBLE: {

			Automation *a = song->get_track(current_menu_track)->get_automation(current_menu_automation);
			undo_redo->begin_action("Toggle Automation Visibility");
			undo_redo->do_method(a,&Automation::set_visible,!a->is_visible());
			undo_redo->undo_method(a,&Automation::set_visible,a->is_visible());
			undo_redo->do_method(this,&PatternEditor::_redraw);
			undo_redo->undo_method(this,&PatternEditor::_redraw);
			undo_redo->commit_action();

		} break;
		case AUTOMATION_MENU_MODE_ROWS: {

			Automation *a = song->get_track(current_menu_track)->get_automation(current_menu_automation);
			undo_redo->begin_action("Automation Display Numbers");
			undo_redo->do_method(a,&Automation::set_display_mode,Automation::DISPLAY_ROWS);
			undo_redo->undo_method(a,&Automation::set_display_mode,a->get_display_mode());
			undo_redo->do_method(this,&PatternEditor::_redraw);
			undo_redo->undo_method(this,&PatternEditor::_redraw);
			undo_redo->commit_action();

		} break;
		case AUTOMATION_MENU_MODE_SMALL: {

			Automation *a = song->get_track(current_menu_track)->get_automation(current_menu_automation);
			undo_redo->begin_action("Automation Display Small");
			undo_redo->do_method(a,&Automation::set_display_mode,Automation::DISPLAY_SMALL);
			undo_redo->undo_method(a,&Automation::set_display_mode,a->get_display_mode());
			undo_redo->do_method(this,&PatternEditor::_redraw);
			undo_redo->undo_method(this,&PatternEditor::_redraw);
			undo_redo->commit_action();

		} break;
		case AUTOMATION_MENU_MODE_LARGE: {

			Automation *a = song->get_track(current_menu_track)->get_automation(current_menu_automation);
			undo_redo->begin_action("Automation Display Large");
			undo_redo->do_method(a,&Automation::set_display_mode,Automation::DISPLAY_LARGE);
			undo_redo->undo_method(a,&Automation::set_display_mode,a->get_display_mode());
			undo_redo->do_method(this,&PatternEditor::_redraw);
			undo_redo->undo_method(this,&PatternEditor::_redraw);
			undo_redo->commit_action();

		} break;
		case AUTOMATION_MENU_MOVE_LEFT: {

			undo_redo->begin_action("Automation Move Left");
			undo_redo->do_method(song->get_track(current_menu_track),&Track::swap_automations,current_menu_automation,current_menu_automation-1);
			undo_redo->undo_method(song->get_track(current_menu_track),&Track::swap_automations,current_menu_automation,current_menu_automation-1);
			undo_redo->do_method(this,&PatternEditor::_redraw);
			undo_redo->undo_method(this,&PatternEditor::_redraw);
			undo_redo->commit_action();

		} break;
		case AUTOMATION_MENU_MOVE_RIGHT: {

			undo_redo->begin_action("Automation Move Right");
			undo_redo->do_method(song->get_track(current_menu_track),&Track::swap_automations,current_menu_automation,current_menu_automation+1);
			undo_redo->undo_method(song->get_track(current_menu_track),&Track::swap_automations,current_menu_automation,current_menu_automation+1);
			undo_redo->do_method(this,&PatternEditor::_redraw);
			undo_redo->undo_method(this,&PatternEditor::_redraw);
			undo_redo->commit_action();

		} break;
		case AUTOMATION_MENU_REMOVE: {

			Automation *a = song->get_track(current_menu_track)->get_automation(current_menu_automation);
			undo_redo->begin_action("Remove Automation");
			undo_redo->do_method(song->get_track(current_menu_track),&Track::remove_automation,current_menu_automation);
			undo_redo->undo_method(song->get_track(current_menu_track),&Track::add_automation,a,current_menu_automation);
			undo_redo->do_method(this,&PatternEditor::_redraw);
			undo_redo->undo_method(this,&PatternEditor::_redraw);
			undo_redo->commit_action();

		} break;


	}
}

PatternEditor::PatternEditor(Song *p_song, UndoRedo *p_undo_redo) {

	undo_redo=p_undo_redo;

	v_offset=0;
	h_offset=0;

	song=p_song;
	current_pattern=0;
	rows_per_beat=4;

	Track *t = new Track;
	t->set_columns(2);
	song->add_track(t);
	visible_rows=4;

	cursor.row=0;
	cursor.field=0;
	cursor.column=0;
	cursor.skip=1;

	t->set_note(0,Track::Pos(0,0),Track::Note(60,99));
	t->set_note(0,Track::Pos(TICKS_PER_BEAT,1),Track::Note(88,Track::Note::EMPTY));
	t->set_note(0,Track::Pos(TICKS_PER_BEAT*4,1),Track::Note(Track::Note::OFF));
	t->set_note(0,Track::Pos(66,1),Track::Note(63,33));
	t->set_note(0,Track::Pos(TICKS_PER_BEAT*3,0),Track::Note(28,80));
	t->set_note(0,Track::Pos(TICKS_PER_BEAT*3+1,0),Track::Note(Track::Note::OFF,30));
	t->add_automation( new Automation(t->get_volume_port() ) );
	t->add_automation( new Automation(t->get_swing_port() ) );
	t->add_automation( new Automation(t->get_pan_port() ) );
	t->get_automation(0)->set_display_mode(Automation::DISPLAY_ROWS);
	t->get_automation(0)->set_point(0,0,160);
	t->get_automation(0)->set_point(0,TICKS_PER_BEAT*3+2,22);
	t->get_automation(0)->set_point(0,TICKS_PER_BEAT*5,22);
	t->get_automation(0)->set_point(0,TICKS_PER_BEAT*5+2,180);
	t->get_automation(0)->set_point(0,TICKS_PER_BEAT*5+4,200);
	t->get_automation(1)->set_display_mode(Automation::DISPLAY_SMALL);
	t->get_automation(2)->set_display_mode(Automation::DISPLAY_LARGE);
	t->get_automation(2)->set_point(0,0,22);
	t->get_automation(2)->set_point(0,TICKS_PER_BEAT*2,250);
	t->get_automation(2)->set_point(0,TICKS_PER_BEAT*3,128);
	set_focus_mode(FOCUS_ALL);


}
