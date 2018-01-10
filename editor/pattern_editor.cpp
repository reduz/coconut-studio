#include "pattern_editor.h"
#include "cskin.h"
#include "gui/base/painter.h"
#include "key_bindings.h"
#include <stdio.h>
#include <cmath>
void PatternEditor::_cursor_advance() {

	cursor.row += cursor_advance;
	if (cursor.row >= get_total_rows() - 1)
		cursor.row = get_total_rows() - 1;
}

void PatternEditor::_field_clear() {

	Track::Pos from;
	from.tick = cursor.row * TICKS_PER_BEAT / rows_per_beat;
	from.column = cursor.column;

	Track::Pos to;
	to.tick = from.tick + TICKS_PER_BEAT / rows_per_beat;
	to.column = cursor.column;

	List<Track::PosEvent> events;

	song->get_events_in_range(current_pattern, from, to, &events);

	printf("evcount: %i\n", events.size());
	if (events.size() == 0) {
		_cursor_advance();
		update();
		return;
	}

	if (cursor.field == 0 || cursor.field == 1) { //just clear whathever

		undo_redo->begin_action("Clear Event");

		for (List<Track::PosEvent>::Element *E = events.front(); E; E = E->next()) {
			Track::Event ev = E->get().event;
			Track::Event old_ev = ev;
			ev.a = Track::Note::EMPTY;
			ev.b = 0xFF;
			undo_redo->do_method(
					song, &Song::set_event, current_pattern, cursor.column,
					E->get().pos.tick, ev);
			undo_redo->undo_method(
					song, &Song::set_event, current_pattern, cursor.column,
					E->get().pos.tick, old_ev);
			undo_redo->do_method(this, &PatternEditor::updatef);
			undo_redo->undo_method(this, &PatternEditor::updatef);
		}
		undo_redo->commit_action();
	} else {
		undo_redo->begin_action("Clear Volume");

		for (List<Track::PosEvent>::Element *E = events.front(); E; E = E->next()) {
			Track::Event ev = E->get().event;
			Track::Event old_ev = ev;
			ev.b = Track::Note::EMPTY;
			undo_redo->do_method(
					song, &Song::set_event, current_pattern, cursor.column,
					E->get().pos.tick, ev);
			undo_redo->undo_method(
					song, &Song::set_event, current_pattern, cursor.column,
					E->get().pos.tick, old_ev);
			undo_redo->do_method(this, &PatternEditor::updatef);
			undo_redo->undo_method(this, &PatternEditor::updatef);
		}
		undo_redo->commit_action();
	}
	update();
	_cursor_advance();
}

bool PatternEditor::key(unsigned long p_unicode, unsigned long p_scan_code,
		bool p_press, bool p_repeat, int p_modifier_mask) {

	if (!p_press)
		return false;

	unsigned long sc = p_scan_code | p_modifier_mask;

	if (sc == KeyBind::get_keybind_code(KB_CURSOR_MOVE_UP)) {
		cursor.row -= cursor.skip;
		_validate_cursor();
	} else if (sc == KeyBind::get_keybind_code(KB_CURSOR_MOVE_DOWN)) {
		cursor.row += cursor.skip;
		_validate_cursor();
	} else if (sc == KeyBind::get_keybind_code(KB_CURSOR_MOVE_UP_1_ROW)) {
		cursor.row -= 1;
		_validate_cursor();
	} else if (sc == KeyBind::get_keybind_code(KB_CURSOR_MOVE_DOWN_1_ROW)) {
		cursor.row += 1;
		_validate_cursor();
	} else if (sc == KeyBind::get_keybind_code(KB_CURSOR_PAGE_UP)) {
		cursor.row -=
				song->pattern_get_beats_per_bar(current_pattern) * rows_per_beat;
		_validate_cursor();
	} else if (sc == KeyBind::get_keybind_code(KB_CURSOR_PAGE_DOWN)) {
		cursor.row +=
				song->pattern_get_beats_per_bar(current_pattern) * rows_per_beat;
		_validate_cursor();
	} else if (sc == KeyBind::get_keybind_code(KB_CURSOR_MOVE_LEFT)) {

		if (cursor.field == 0) {
			if (cursor.column > 0) {
				cursor.column--;

				Track *track;
				int automation;
				int column;
				get_cursor_column_data(&track, automation, column);
				ERR_FAIL_COND_V(!track, false);

				if (automation >= 0) {
					if (track->get_automation(automation)->get_display_mode() !=
							Automation::DISPLAY_ROWS) {
						cursor.field = 0;
					} else {
						cursor.field = 1;
					}
				} else {
					cursor.field = 3;
				}
			}
		} else {
			cursor.field--;
		}
		_validate_cursor();
	} else if (sc == KeyBind::get_keybind_code(KB_CURSOR_MOVE_RIGHT)) {

		Track *track;
		int automation;
		int column;
		get_cursor_column_data(&track, automation, column);
		ERR_FAIL_COND_V(!track, false);

		int max_field = 1;

		if (automation >= 0) {
			if (track->get_automation(automation)->get_display_mode() !=
					Automation::DISPLAY_ROWS) {
				max_field = 0;
			} else {
				max_field = 1;
			}
		} else {
			max_field = 3;
		}

		if (cursor.field == max_field) {
			if (cursor.column < song->get_event_column_count() - 1) {
				cursor.column++;
				cursor.field = 0;
			}
		} else {
			cursor.field++;
		}
		_validate_cursor();
	} else if (sc == KeyBind::get_keybind_code(KB_CURSOR_TAB)) {

		if (cursor.column < song->get_event_column_count() - 1) {
			cursor.column++;
			cursor.field = 0;
		}
		_validate_cursor();

	} else if (sc == KeyBind::get_keybind_code(KB_CURSOR_BACKTAB)) {

		if (cursor.field > 0)
			cursor.field = 0;
		else if (cursor.column > 0)
			cursor.column--;
		_validate_cursor();
	} else if (sc == KeyBind::get_keybind_code(KB_CURSOR_HOME)) {

		cursor.column = 0;
		cursor.field = 0;
		_validate_cursor();

	} else if (sc == KeyBind::get_keybind_code(KB_CURSOR_END)) {

		cursor.column = song->get_event_column_count() - 1;
		cursor.field = 0;
		_validate_cursor();
	} else if (sc == KeyBind::get_keybind_code(KB_PATTERN_PAN_WINDOW_UP)) {

		if (v_offset > 0) {
			if (v_offset + visible_rows - 1 == cursor.row)
				cursor.row--;
			v_offset--;
			update();
		}
	} else if (sc == KeyBind::get_keybind_code(KB_PATTERN_PAN_WINDOW_DOWN)) {

		if (v_offset + visible_rows < get_total_rows()) {
			if (cursor.row <= v_offset) {
				cursor.row++;
			}
			v_offset++;
			update();
		}
	} else if (sc == KeyBind::get_keybind_code(KB_CURSOR_INSERT)) {

		List<Track::PosEvent> events;

		Track::Pos from;
		from.tick = cursor.row * TICKS_PER_BEAT / rows_per_beat;
		from.column = cursor.column;

		Track::Pos to;
		to.tick = song->pattern_get_beats(current_pattern) * TICKS_PER_BEAT;
		to.column = cursor.column;

		song->get_events_in_range(current_pattern, from, to, &events);

		if (events.size()) {

			undo_redo->begin_action("Insert", true);

			for (List<Track::PosEvent>::Element *E = events.front(); E; E = E->next()) {

				Track::Event ev = E->get().event;
				ev.a = 0xFF;
				ev.b = 0xFF;

				undo_redo->do_method(song, &Song::set_event, current_pattern, cursor.column, E->get().pos.tick, ev);
			}

			for (List<Track::PosEvent>::Element *E = events.front(); E; E = E->next()) {

				Track::Event ev = E->get().event;
				undo_redo->do_method(song, &Song::set_event, current_pattern, cursor.column, E->get().pos.tick + TICKS_PER_BEAT / rows_per_beat, ev);
				ev.a = 0xFF;
				ev.b = 0xFF;
				undo_redo->undo_method(song, &Song::set_event, current_pattern, cursor.column, E->get().pos.tick + TICKS_PER_BEAT / rows_per_beat, ev);
			}

			for (List<Track::PosEvent>::Element *E = events.front(); E; E = E->next()) {

				Track::Event ev = E->get().event;
				undo_redo->undo_method(song, &Song::set_event, current_pattern, cursor.column, E->get().pos.tick, ev);
			}

			undo_redo->do_method(this, &PatternEditor::updatef);
			undo_redo->undo_method(this, &PatternEditor::updatef);
			undo_redo->commit_action();
		}
	} else if (sc == KeyBind::get_keybind_code(KB_CURSOR_DELETE)) {

		List<Track::PosEvent> events;

		Track::Pos from;
		from.tick = cursor.row * TICKS_PER_BEAT / rows_per_beat;
		from.column = cursor.column;

		Track::Pos to;
		to.tick = song->pattern_get_beats(current_pattern) * TICKS_PER_BEAT;
		to.column = cursor.column;

		song->get_events_in_range(current_pattern, from, to, &events);

		if (events.size()) {

			undo_redo->begin_action("Delete", true);

			Tick limit = from.tick;

			for (List<Track::PosEvent>::Element *E = events.front(); E; E = E->next()) {

				Track::Event ev = E->get().event;
				ev.a = 0xFF;
				ev.b = 0xFF;

				undo_redo->do_method(song, &Song::set_event, current_pattern, cursor.column, E->get().pos.tick, ev);
			}

			for (List<Track::PosEvent>::Element *E = events.front(); E; E = E->next()) {

				Tick new_ofs = E->get().pos.tick - TICKS_PER_BEAT / rows_per_beat;

				if (new_ofs < limit)
					continue;

				Track::Event ev = E->get().event;
				undo_redo->do_method(song, &Song::set_event, current_pattern, cursor.column, new_ofs, ev);
				ev.a = 0xFF;
				ev.b = 0xFF;
				undo_redo->undo_method(song, &Song::set_event, current_pattern, cursor.column, new_ofs, ev);
			}

			for (List<Track::PosEvent>::Element *E = events.front(); E; E = E->next()) {

				Track::Event ev = E->get().event;
				undo_redo->undo_method(song, &Song::set_event, current_pattern, cursor.column, E->get().pos.tick, ev);
			}

			undo_redo->do_method(this, &PatternEditor::updatef);
			undo_redo->undo_method(this, &PatternEditor::updatef);
			undo_redo->commit_action();
		}

	} else {

		// check field

		Track *track;
		int automation;
		int column;
		get_cursor_column_data(&track, automation, column);
		ERR_FAIL_COND_V(!track, false);

		if (column >= 0) {

			if (cursor.field == 0) {
				// put a note
				for (int i = KB_PIANO_C0; i <= KB_PIANO_E2; i++) {
					if (sc == KeyBind::get_keybind_code(KeyBindList(i))) {
						int note = i - KB_PIANO_C0;

						Track::Event ev =
								song->get_event(current_pattern, cursor.column,
										cursor.row * TICKS_PER_BEAT / rows_per_beat);
						Track::Event old_ev = ev;
						ev.a = current_octave * 12 + note;
						undo_redo->begin_action("Add Note");
						undo_redo->do_method(
								song, &Song::set_event, current_pattern, cursor.column,
								Tick(cursor.row * TICKS_PER_BEAT / rows_per_beat), ev);
						undo_redo->undo_method(
								song, &Song::set_event, current_pattern, cursor.column,
								Tick(cursor.row * TICKS_PER_BEAT / rows_per_beat), old_ev);
						undo_redo->do_method(this, &PatternEditor::updatef);
						undo_redo->undo_method(this, &PatternEditor::updatef);
						undo_redo->commit_action();
						_cursor_advance();

						return true;
					}
				}

				if (sc == KeyBind::get_keybind_code(KB_PATTERN_CURSOR_NOTE_OFF)) {

					Track::Event ev =
							song->get_event(current_pattern, cursor.column,
									cursor.row * TICKS_PER_BEAT / rows_per_beat);
					Track::Event old_ev = ev;
					ev.a = Track::Note::OFF;
					undo_redo->begin_action("Add Note Off");
					undo_redo->do_method(
							song, &Song::set_event, current_pattern, cursor.column,
							Tick(cursor.row * TICKS_PER_BEAT / rows_per_beat), ev);
					undo_redo->undo_method(
							song, &Song::set_event, current_pattern, cursor.column,
							Tick(cursor.row * TICKS_PER_BEAT / rows_per_beat), old_ev);
					undo_redo->do_method(this, &PatternEditor::updatef);
					undo_redo->undo_method(this, &PatternEditor::updatef);
					undo_redo->commit_action();
					_cursor_advance();
				}

				if (sc == KeyBind::get_keybind_code(KB_CURSOR_FIELD_CLEAR)) {
					_field_clear();
				}

			} else if (cursor.field == 1) {
				// put octave
				if (p_scan_code >= KEY_0 && p_scan_code <= KEY_9) {

					int octave = p_scan_code - KEY_0;

					Track::Event ev =
							song->get_event(current_pattern, cursor.column,
									cursor.row * TICKS_PER_BEAT / rows_per_beat);
					Track::Event old_ev = ev;

					if (old_ev.a == Track::Note::EMPTY) {
						return true; // no add octave on empty
					}

					ev.a = (ev.a % 12) + octave * 12;

					undo_redo->begin_action("Set Octave");
					undo_redo->do_method(
							song, &Song::set_event, current_pattern, cursor.column,
							Tick(cursor.row * TICKS_PER_BEAT / rows_per_beat), ev);
					undo_redo->undo_method(
							song, &Song::set_event, current_pattern, cursor.column,
							Tick(cursor.row * TICKS_PER_BEAT / rows_per_beat), old_ev);
					undo_redo->do_method(this, &PatternEditor::updatef);
					undo_redo->undo_method(this, &PatternEditor::updatef);
					undo_redo->commit_action();
					_cursor_advance();

					return true;
				}

				if (sc == KeyBind::get_keybind_code(KB_CURSOR_FIELD_CLEAR)) {
					_field_clear();
				}

			} else if (cursor.field == 2) {
				// put volume 1
				if (p_scan_code >= KEY_0 && p_scan_code <= KEY_9) {

					int vol_g = p_scan_code - KEY_0;

					Track::Event ev =
							song->get_event(current_pattern, cursor.column,
									cursor.row * TICKS_PER_BEAT / rows_per_beat);
					Track::Event old_ev = ev;

					if (old_ev.b == Track::Note::EMPTY) {
						ev.b = 0;
					}

					ev.b = (ev.b % 10) + vol_g * 10;

					undo_redo->begin_action("Set Volume");
					undo_redo->do_method(
							song, &Song::set_event, current_pattern, cursor.column,
							Tick(cursor.row * TICKS_PER_BEAT / rows_per_beat), ev);
					undo_redo->undo_method(
							song, &Song::set_event, current_pattern, cursor.column,
							Tick(cursor.row * TICKS_PER_BEAT / rows_per_beat), old_ev);
					undo_redo->do_method(this, &PatternEditor::updatef);
					undo_redo->undo_method(this, &PatternEditor::updatef);
					undo_redo->commit_action();
					cursor.field = 3;

					return true;
				}

				if (sc == KeyBind::get_keybind_code(KB_CURSOR_FIELD_CLEAR)) {
					_field_clear();
				}

			} else if (cursor.field == 3) {
				// put volume 2
				if (p_scan_code >= KEY_0 && p_scan_code <= KEY_9) {

					int vol_l = p_scan_code - KEY_0;

					Track::Event ev =
							song->get_event(current_pattern, cursor.column,
									cursor.row * TICKS_PER_BEAT / rows_per_beat);

					Track::Event old_ev = ev;

					if (old_ev.b == Track::Note::EMPTY) {
						ev.b = 0;
					}

					ev.b -= (ev.b % 10);
					ev.b += vol_l;

					undo_redo->begin_action("Set Volume");
					undo_redo->do_method(
							song, &Song::set_event, current_pattern, cursor.column,
							Tick(cursor.row * TICKS_PER_BEAT / rows_per_beat), ev);
					undo_redo->undo_method(
							song, &Song::set_event, current_pattern, cursor.column,
							Tick(cursor.row * TICKS_PER_BEAT / rows_per_beat), old_ev);
					undo_redo->do_method(this, &PatternEditor::updatef);
					undo_redo->undo_method(this, &PatternEditor::updatef);
					undo_redo->commit_action();
					cursor.field = 2;
					_cursor_advance();

					return true;
				}

				if (sc == KeyBind::get_keybind_code(KB_CURSOR_FIELD_CLEAR)) {
					_field_clear();
				}
			}
		} else if (automation >= 0) {

			Automation *a = track->get_automation(automation);
			if (a->get_display_mode() == Automation::DISPLAY_ROWS) {

				if (p_scan_code >= KEY_0 && p_scan_code <= KEY_9) {
					Track::Event ev =
							song->get_event(current_pattern, cursor.column,
									cursor.row * TICKS_PER_BEAT / rows_per_beat);

					Track::Event old_ev = ev;

					if (ev.a == Automation::EMPTY) {
						ev.a = 0;
					}

					if (cursor.field == 0) {
						ev.a = (p_scan_code - KEY_0) * 10 + ev.a % 10;
					} else {
						ev.a = (p_scan_code - KEY_0) + (ev.a / 10) * 10;
					}

					undo_redo->begin_action("Set Automation Point");
					undo_redo->do_method(
							song, &Song::set_event, current_pattern, cursor.column,
							Tick(cursor.row * TICKS_PER_BEAT / rows_per_beat), ev);
					undo_redo->undo_method(
							song, &Song::set_event, current_pattern, cursor.column,
							Tick(cursor.row * TICKS_PER_BEAT / rows_per_beat), old_ev);
					undo_redo->do_method(this, &PatternEditor::updatef);
					undo_redo->undo_method(this, &PatternEditor::updatef);
					undo_redo->commit_action();
					if (cursor.field == 0) {
						cursor.field = 1;
					} else {
						cursor.field = 0;
						_cursor_advance();
					}
				}
			}

			if (sc == KeyBind::get_keybind_code(KB_CURSOR_FIELD_CLEAR)) {
				_field_clear();
			}
		}

		return false;
	}

	return true;
}

void PatternEditor::_validate_cursor() {

	if (cursor.row < 0)
		cursor.row = 0;
	else if (cursor.row >= get_total_rows())
		cursor.row = get_total_rows() - 1;

	if (cursor.row < v_offset)
		v_offset = cursor.row;
	if (cursor.row >= v_offset + visible_rows)
		v_offset = cursor.row - visible_rows + 1;

	if (cursor.column < h_offset) {
		h_offset = cursor.column;
	}

	update();
}

int PatternEditor::get_total_rows() const {

	return song->pattern_get_beats(current_pattern) * rows_per_beat;
}

void PatternEditor::mouse_button(const Point &p_pos, int p_button, bool p_press,
		int p_modifier_mask) {

	if (p_press && p_button == 1) {

		// test trackbuttons
		for (List<TrackButton>::Element *E = track_buttons.front(); E;
				E = E->next()) {

			if (E->get().r.has_point(p_pos)) {
				track_menu->set_pos(get_global_pos() + E->get().r.pos +
									Point(0, E->get().r.size.y));
				track_menu->clear();

				if (song->get_track(E->get().track)->get_column_count() < 16)
					track_menu->add_item("Add Column", TRACK_MENU_ADD_COLUMN);
				if (song->get_track(E->get().track)->get_column_count() > 1)
					track_menu->add_item("Remove Column", TRACK_MENU_REMOVE_COLUMN);
				track_menu->add_separator();
				track_menu->add_item(
						"Solo", TRACK_MENU_SOLO, NULL,
						KeyBind::get_keybind_code(KB_PATTERN_SOLO_TOGGLE_CHANNEL), false);
				track_menu->add_check_itemsc(
						"Muted", TRACK_MENU_MUTE,
						song->get_track(E->get().track)->is_muted(),
						KeyBind::get_keybind_code(KB_PATTERN_MUTE_TOGGLE_CHANNEL));
				track_menu->add_separator();
				track_menu->add_item("Automations..", TRACK_MENU_EDIT_AUTOMATIONS);
				track_menu->add_separator();
				track_menu->add_item("Rename..", TRACK_MENU_RENAME);
				if (song->get_track_count() > 1) {
					track_menu->add_separator();
					if (E->get().track > 0)
						track_menu->add_item("Move Left", TRACK_MENU_MOVE_LEFT);
					if (E->get().track < song->get_track_count() - 1)
						track_menu->add_item("Move Right", TRACK_MENU_MOVE_RIGHT);
				}
				track_menu->add_separator();
				track_menu->add_item("Remove", TRACK_MENU_REMOVE);
				track_menu->popup();
				current_menu_track = E->get().track;
				return;
			}
		}

		for (List<AutomationButton>::Element *E = automation_buttons.front(); E;
				E = E->next()) {

			if (E->get().r.has_point(p_pos)) {
				auto_menu->set_pos(get_global_pos() + E->get().r.pos +
								   Point(0, E->get().r.size.y));
				auto_menu->clear();
				auto_menu->add_check_item("Visible", AUTOMATION_MENU_VISIBLE,
						song->get_track(E->get().track)
								->get_automation(E->get().automation)
								->is_visible());
				auto_menu->add_separator();
				auto_menu->add_check_item("Numbers", AUTOMATION_MENU_MODE_ROWS,
						song->get_track(E->get().track)
										->get_automation(E->get().automation)
										->get_display_mode() ==
								Automation::DISPLAY_ROWS);
				auto_menu->add_check_item("Small Envelope", AUTOMATION_MENU_MODE_SMALL,
						song->get_track(E->get().track)
										->get_automation(E->get().automation)
										->get_display_mode() ==
								Automation::DISPLAY_SMALL);
				auto_menu->add_check_item("Large Envelope", AUTOMATION_MENU_MODE_LARGE,
						song->get_track(E->get().track)
										->get_automation(E->get().automation)
										->get_display_mode() ==
								Automation::DISPLAY_LARGE);
				if (song->get_track(E->get().track)->get_automation_count() > 1) {
					auto_menu->add_separator();
					if (E->get().automation > 0)
						auto_menu->add_item("Move Left", AUTOMATION_MENU_MOVE_LEFT);
					if (E->get().automation <
							song->get_track(E->get().track)->get_automation_count() - 1)
						auto_menu->add_item("Move Right", AUTOMATION_MENU_MOVE_RIGHT);
				}
				auto_menu->add_separator();
				auto_menu->add_item("Remove", AUTOMATION_MENU_REMOVE);
				auto_menu->popup();
				current_menu_track = E->get().track;
				current_menu_automation = E->get().automation;
				return;
			}
		}

		for (List<ClickArea>::Element *E = click_areas.front(); E; E = E->next()) {

			int point_index = -1;
			float point_d = 1e20;

			Point pos;
			for (List<ClickArea::AutomationPoint>::Element *F = E->get().automation_points.front(); F; F = F->next()) {

				Point p = Point(F->get().x, F->get().y);

				float d = sqrt((p.x - p_pos.x) * (p.x - p_pos.x) + (p.y - p_pos.y) * (p.y - p_pos.y));

				if (d < 4) {
					if (point_index < 0 || d < point_d) {
						point_index = F->get().index;
						point_d = d;
						pos = p;
					}
				}
			}

			if (point_index >= 0) {
				grabbing_point = point_index;
				grabbing_point_tick_from = E->get().automation->get_point_tick_by_index(current_pattern, grabbing_point);
				grabbing_point_value_from = E->get().automation->get_point_by_index(current_pattern, grabbing_point);
				grabbing_point_tick = grabbing_point_tick_from;
				grabbing_point_value = grabbing_point_value_from;
				grabbing_automation = E->get().automation;
				grabbing_x = E->get().fields[0].x;
				grabbing_width = E->get().fields[0].width;
				grabbing_mouse_pos = pos;

			} else if (p_modifier_mask & KEY_MASK_CTRL && p_pos.x >= E->get().fields[0].x && p_pos.x < E->get().fields[0].x + E->get().fields[0].width) {
				//add it
				int x = p_pos.x - E->get().fields[0].x;
				int y = p_pos.y;
				int w = E->get().fields[0].width;

				Tick tick = MAX(0, (y - row_top_ofs + v_offset * row_height_cache)) * TICKS_PER_BEAT / (row_height_cache * rows_per_beat);

				uint8_t value = CLAMP((x)*Automation::VALUE_MAX / w, 0, Automation::VALUE_MAX);

				grabbing_automation = E->get().automation;
				grabbing_automation->set_point(current_pattern, tick, value);
				grabbing_point = 1; //useless, can be anything here
				grabbing_point_tick_from = tick;
				grabbing_point_value_from = Automation::EMPTY;
				grabbing_point_tick = tick;
				grabbing_point_value = value;
				grabbing_x = E->get().fields[0].x;
				grabbing_width = E->get().fields[0].width;
				grabbing_mouse_pos = p_pos;
				update();
			} else {
				for (int i = 0; i < E->get().fields.size(); i++) {
					int localx = p_pos.x - E->get().fields[i].x;
					if (localx >= 0 && localx < E->get().fields[i].width) {
						cursor.field = i;
						cursor.column = E->get().column;
						update();
						cursor.row = (p_pos.y / row_height_cache) + v_offset;

						return;
					}
				}
			}
		}
	}

	if (p_press && p_button == 3 && grabbing_point == -1) {
		//remove
		for (List<ClickArea>::Element *E = click_areas.front(); E; E = E->next()) {

			int point_index = -1;
			float point_d = 1e20;
			for (List<ClickArea::AutomationPoint>::Element *F = E->get().automation_points.front(); F; F = F->next()) {

				Point p = Point(F->get().x, F->get().y);

				float d = sqrt((p.x - p_pos.x) * (p.x - p_pos.x) + (p.y - p_pos.y) * (p.y - p_pos.y));

				if (d < 4) {
					if (point_index < 0 || d < point_d) {
						point_index = F->get().index;
						point_d = d;
					}
				}
			}

			if (point_index >= 0) {
				undo_redo->begin_action("Remove Point");
				Tick tick = E->get().automation->get_point_tick_by_index(current_pattern, point_index);
				uint8_t value = E->get().automation->get_point_by_index(current_pattern, point_index);

				undo_redo->do_method(E->get().automation, &Automation::remove_point, current_pattern, tick);
				undo_redo->undo_method(E->get().automation, &Automation::set_point, current_pattern, tick, value);
				undo_redo->do_method(this, &PatternEditor::_redraw);
				undo_redo->undo_method(this, &PatternEditor::_redraw);
				undo_redo->commit_action();
			}
		}
	}

	if (!p_press && p_button == 1) {

		if (grabbing_point >= 0) {
			grabbing_point = -1;

			undo_redo->begin_action("Move Point");
			undo_redo->do_method(grabbing_automation, &Automation::remove_point, current_pattern, grabbing_point_tick_from);
			undo_redo->do_method(grabbing_automation, &Automation::set_point, current_pattern, grabbing_point_tick, grabbing_point_value);
			undo_redo->undo_method(grabbing_automation, &Automation::remove_point, current_pattern, grabbing_point_tick);
			undo_redo->undo_method(grabbing_automation, &Automation::set_point, current_pattern, grabbing_point_tick_from, grabbing_point_value_from);
			undo_redo->do_method(this, &PatternEditor::_redraw);
			undo_redo->undo_method(this, &PatternEditor::_redraw);
			undo_redo->commit_action();
		}
	}
}

void PatternEditor::mouse_motion(const Point &p_pos, const Point &p_rel, int p_button_mask) {

	if (grabbing_point >= 0) {

		grabbing_mouse_pos += p_rel;

		int y = grabbing_mouse_pos.y;
		int x = grabbing_mouse_pos.x;
		Tick tick = MAX(0, (y - row_top_ofs + v_offset * row_height_cache)) * TICKS_PER_BEAT / (row_height_cache * rows_per_beat);

		grabbing_automation->remove_point(current_pattern, grabbing_point_tick);
		while (grabbing_automation->get_point(current_pattern, tick) != Automation::EMPTY) {
			tick++;
		}
		uint8_t value = CLAMP((x - grabbing_x) * Automation::VALUE_MAX / grabbing_width, 0, Automation::VALUE_MAX);

		grabbing_point_tick = tick;
		grabbing_point_value = value;

		grabbing_automation->set_point(current_pattern, tick, value);
		update();
	}
}

int PatternEditor::get_visible_rows() const {
	return visible_rows;
}

void PatternEditor::get_cursor_column_data(Track **r_track, int &r_automation,
		int &r_track_column) {

	int cc = cursor.column;

	r_automation = -1;
	r_track_column = -1;
	*r_track = NULL;

	for (int i = 0; i < song->get_track_count(); i++) {

		Track *t = song->get_track(i);
		*r_track = t;
		r_automation = -1;
		r_track_column = -1;

		for (int j = 0; j < t->get_column_count(); j++) {

			r_track_column = j;
			if (cc == 0) {
				return;
			}
			cc--;
		}

		r_track_column = -1;

		for (int j = 0; j < t->get_automation_count(); j++) {

			if (!t->get_automation(i)->is_visible())
				continue;

			r_automation = j;

			if (cc == 0) {
				return;
			}

			cc--;
		}
	}
}

void PatternEditor::draw(const Point &p_global, const Size &p_size,
		const Rect &p_exposed) {

	printf("field: %i\n", cursor.field);
	Painter *p = get_painter();

	FontID f = font(FONT_PATTERN_EDITOR_ROWS);
	FontID fcol = font(FONT_PATTERN_EDITOR_COLUMN_NAME);
	int fw = p->get_font_char_width(f, 'X');
	int fh = p->get_font_height(f);
	int fa = p->get_font_ascent(f);
	int sep = 1;
	fh += sep;

	row_height_cache = fh;
	int top_ofs = 4;

	row_top_ofs = top_ofs;

	Color track_sep_color = color(COLOR_PATTERN_EDITOR_TRACK_SEPARATOR);
	Color cursorcol = color(COLOR_PATTERN_EDITOR_CURSOR);
	int track_sep_w = constant(C_PATTERN_EDITOR_TRACK_SEPARATION);

	track_buttons.clear();
	automation_buttons.clear();
	click_areas.clear();

	visible_rows = (p_size.height - top_ofs) / fh;

	int beats_per_bar = song->pattern_get_beats_per_bar(current_pattern);

	for (int i = 0; i < visible_rows; i++) {

		int row = v_offset + i;
		int beat = row / rows_per_beat;
		int subbeat = row % rows_per_beat;

		if (subbeat == 0 || i == 0) {
			if (beat % beats_per_bar == 0)
				p->draw_text(f, Point(0, top_ofs + i * fh + fa), String::num(beat),
						color(COLOR_PATTERN_EDITOR_ROW_BAR));
			else
				p->draw_text(f, Point(0, top_ofs + i * fh + fa), String::num(beat),
						color(COLOR_PATTERN_EDITOR_ROW_BEAT));

		} else {
			char text[3] = { '0' + (subbeat / 10), '0' + (subbeat % 10), 0 };
			p->draw_text(f, Point(fw, top_ofs + i * fh + fa), text,
					color(COLOR_PATTERN_EDITOR_ROW_SUB_BEAT));
		}
	}

	int ofs = fw * 4;
	Color bgcol = color(COLOR_PATTERN_EDITOR_BG);
	p->draw_fill_rect(Point(ofs, 0), Point(p_size.x - ofs, p_size.y), bgcol);

	int idx = 0;

	for (int i = 0; i < song->get_track_count(); i++) {

		Track *t = song->get_track(i);

		bool drawn = false;

		for (int j = 0; j < t->get_column_count(); j++) {

			if (idx < h_offset) {
				idx++;
				continue;
			}
			if (j == 0) {

				int as = p->get_font_height(fcol);
				p->draw_arrow(Point(ofs, top_ofs), Size(as, as), DOWN,
						color(COLOR_PATTERN_EDITOR_TRACK_NAME));
				p->draw_text(
						fcol,
						Point(ofs + p->get_font_height(fcol) - p->get_font_ascent(fcol),
								top_ofs + as),
						t->get_name(), DOWN, color(COLOR_PATTERN_EDITOR_TRACK_NAME));
				TrackButton tb;
				tb.track = i;

				tb.r = Rect(Point(ofs, top_ofs), Point(as, as));
				track_buttons.push_back(tb);
				ofs += p->get_font_height(fcol);
			}

			{
				//fill fields for click areas
				ClickArea ca;
				ca.column = idx;
				ClickArea::Field f;
				f.width = fw;
				f.x = ofs;
				ca.fields.push_back(f);
				f.x += fw * 2;
				ca.fields.push_back(f);
				f.x += fw * 2;
				ca.fields.push_back(f);
				f.x += fw;
				ca.fields.push_back(f);

				click_areas.push_back(ca);
			}

			drawn = true;
			int extrahl = (j < t->get_column_count() - 1) ? fw : 0;

			for (int k = 0; k < visible_rows; k++) {

				char rowstr[7] = { '.', '.', '.', ' ', '.', '.', 0 };

				int row = v_offset + k;
				int beat = row / rows_per_beat;
				int subbeat = row % rows_per_beat;

				Color c = color(COLOR_PATTERN_EDITOR_NOTE);
				Color bgc = bgcol;

				if (subbeat == 0 || k == 0) {
					if ((beat % beats_per_bar) == 0)
						p->draw_fill_rect(Point(ofs, top_ofs + k * fh - sep),
								Size(fw * 6 + extrahl, fh),
								color(COLOR_PATTERN_EDITOR_HL_BAR));
					else
						p->draw_fill_rect(Point(ofs, top_ofs + k * fh - sep),
								Size(fw * 6 + extrahl, fh),
								color(COLOR_PATTERN_EDITOR_HL_BEAT));
				}

				Track::Pos from, to;
				from.tick = row * TICKS_PER_BEAT / rows_per_beat;
				from.column = j;
				Tick to_tick = (row + 1) * TICKS_PER_BEAT / rows_per_beat;
				to.tick = to_tick;
				to.column = j;

				List<Track::PosNote> pn;

				t->get_notes_in_range(current_pattern, from, to, &pn);
				Vector<Track::PosNote> valid;
				for (List<Track::PosNote>::Element *E = pn.front(); E; E = E->next()) {

					if (E->get().pos.column != j || E->get().pos.tick >= to_tick)
						continue;
					valid.push_back(E->get());
				}

				if (valid.size() == 0) {

					p->draw_text(f, Point(ofs, top_ofs + k * fh + fa), rowstr, c);
				} else if (valid.size() == 1) {

					Track::PosNote n = pn.front()->get();
					if (n.pos.tick != from.tick)
						c = color(COLOR_PATTERN_EDITOR_NOTE_NOFIT);
					if (n.note.note == Track::Note::OFF) {
						rowstr[0] = '=';
						rowstr[1] = '=';
						rowstr[2] = '=';
					} else if (n.note.note < 120) {
						static const char *note[12] = { "C-", "C#", "D-", "D#", "E-", "F-",
							"F#", "G-", "G#", "A-", "A#", "B-" };
						rowstr[0] = note[n.note.note % 12][0];
						rowstr[1] = note[n.note.note % 12][1];
						rowstr[2] = '0' + n.note.note / 12; // octave
					}

					if (n.note.volume < 100) {
						rowstr[4] = '0' + n.note.volume / 10;
						rowstr[5] = '0' + n.note.volume % 10;
					}

					p->draw_text(f, Point(ofs, top_ofs + k * fh + fa), rowstr, c);

				} else {

					Point base = Point(ofs, top_ofs + k * fh);

					for (int l = 0; l < valid.size(); l++) {

						int h = (fh - 2) / valid.size();
						int w = fw * 3 - 2;
						int vw = fw * 2 - 2;

						if (valid[l].note.note < 120) {
							p->draw_fill_rect(base + Point(0, h * l), Size(fw * 3, h - 1), c);
							p->draw_rect(base +
												 Point(valid[l].note.note * w / 120, 1 + h * l),
									Size(2, h - 2), bgc);
						} else if (valid[l].note.note == Track::Note::OFF) {
							p->draw_rect(base + Point(0, 1 + h * l), Size(fw * 3, 1), c);
							p->draw_rect(base + Point(0, 1 + h * l + h - 2), Size(fw * 3, 1),
									c);
						}

						if (valid[l].note.volume < 100) {
							p->draw_fill_rect(base + Point(fw * 4, h * l),
									Size(fw * 2, h - 1), c);
							p->draw_rect(base +
												 Point(fw * 4 + valid[l].note.volume * vw / 100,
														 1 + h * l),
									Size(2, h - 2), bgc);
						}
					}
				}

				if (has_focus() && idx == cursor.column && cursor.row == row) {

					int field_ofs[4] = { 0, 2, 4, 5 };
					Point cursor_pos(ofs + field_ofs[cursor.field] * fw,
							top_ofs + k * fh - sep);
					p->draw_rect(cursor_pos - Point(1, 1), Size(fw + 1, fh + 1),
							cursorcol);
				}
			}

			if (j < t->get_column_count() - 1)
				ofs += constant(C_PATTERN_EDITOR_COLUMN_SEPARATION);
			ofs += fw * 6;
			idx++;
		}

		for (int j = 0; j < t->get_automation_count(); j++) {

			Automation *a = t->get_automation(j);

			if (idx < h_offset) {
				if (a->is_visible())
					idx++;
				continue;
			}

			{
				int as = p->get_font_height(fcol);
				p->draw_arrow(Point(ofs, top_ofs), Size(as, as), DOWN,
						color(COLOR_PATTERN_EDITOR_AUTOMATION_NAME));
				p->draw_text(fcol, Point(ofs + p->get_font_height(fcol) -
												   p->get_font_ascent(fcol),
										   top_ofs + as),
						a->get_control_port()->get_name(), DOWN,
						color(COLOR_PATTERN_EDITOR_AUTOMATION_NAME));

				AutomationButton ab;
				ab.track = i;
				ab.automation = j;
				ab.r = Rect(Point(ofs, top_ofs), Point(as, as));
				automation_buttons.push_back(ab);
			}
			ofs += p->get_font_height(fcol);

			if (!a->is_visible()) {
				continue;
			}

			switch (a->get_display_mode()) {
				case Automation::DISPLAY_ROWS: {

					{
						//fill fields for click areas
						ClickArea ca;
						ca.column = idx;
						ClickArea::Field f;
						f.width = fw;
						f.x = ofs;
						ca.fields.push_back(f);
						f.x += fw;
						ca.fields.push_back(f);
						click_areas.push_back(ca);
					}

					for (int k = 0; k < visible_rows; k++) {

						char rowstr[3] = { '.', '.', 0 };

						int row = v_offset + k;
						int beat = row / rows_per_beat;
						int subbeat = row % rows_per_beat;

						Color c = color(COLOR_PATTERN_EDITOR_AUTOMATION_VALUE);
						Color bgc = bgcol;

						if (subbeat == 0 || k == 0) {
							if ((beat % beats_per_bar) == 0)
								p->draw_fill_rect(Point(ofs, top_ofs + k * fh - sep),
										Size(fw * 2, fh),
										color(COLOR_PATTERN_EDITOR_AUTOMATION_HL_BAR));
							else
								p->draw_fill_rect(Point(ofs, top_ofs + k * fh - sep),
										Size(fw * 2, fh),
										color(COLOR_PATTERN_EDITOR_AUTOMATION_HL_BEAT));
						}

						Tick from = row * TICKS_PER_BEAT / rows_per_beat;
						Tick to = (row + 1) * TICKS_PER_BEAT / rows_per_beat;

						int first;
						int count;

						a->get_points_in_range(current_pattern, from, to, first, count);

						if (count == 0) {

							p->draw_text(f, Point(ofs, top_ofs + k * fh + fa), rowstr, c);
						} else if (count == 1) {

							int val = a->get_point_by_index(current_pattern, first);

							if (a->get_point_tick_by_index(current_pattern, first) != from)
								c = color(COLOR_PATTERN_EDITOR_AUTOMATION_VALUE_NOFIT);

							rowstr[0] = '0' + val / 10;
							rowstr[1] = '0' + val % 10;

							p->draw_text(f, Point(ofs, top_ofs + k * fh + fa), rowstr, c);

						} else {

							Point base = Point(ofs, top_ofs + k * fh);

							for (int l = 0; l < count; l++) {

								int h = (fh - 2) / count;
								int w = fw * 2 - 2;
								int val = a->get_point_by_index(current_pattern, first + l);

								p->draw_fill_rect(base + Point(0, h * l), Size(fw * 2, h - 1), c);
								p->draw_rect(base + Point(val * w / Automation::VALUE_MAX, 1 + h * l),
										Size(2, h - 2), bgc);
							}
						}

						if (has_focus() && idx == cursor.column && cursor.row == row) {

							Point cursor_pos(ofs + cursor.field * fw, top_ofs + k * fh - sep);
							p->draw_rect(cursor_pos - Point(1, 1), Size(fw + 1, fh + 1),
									cursorcol);
						}
					}

					ofs += fw * 2;

				} break;
				case Automation::DISPLAY_SMALL:
				case Automation::DISPLAY_LARGE: {

					int w = a->get_display_mode() == Automation::DISPLAY_SMALL ? 4 : 8;
					w *= fw;

					Color c = color(COLOR_PATTERN_EDITOR_AUTOMATION_VALUE);
					Color cpoint = color(COLOR_PATTERN_EDITOR_AUTOMATION_POINT);

					//fill fields for click areas
					ClickArea ca;
					ca.column = idx;
					ClickArea::Field f;
					f.width = w;
					f.x = ofs;
					ca.fields.push_back(f);

					for (int k = 0; k < visible_rows; k++) {

						int row = v_offset + k;
						int beat = row / rows_per_beat;
						int subbeat = row % rows_per_beat;

						if (subbeat == 0 || k == 0) {
							if ((beat % beats_per_bar) == 0)
								p->draw_fill_rect(Point(ofs, top_ofs + k * fh - sep), Size(w, fh),
										color(COLOR_PATTERN_EDITOR_AUTOMATION_HL_BAR));
							else
								p->draw_fill_rect(Point(ofs, top_ofs + k * fh - sep), Size(w, fh),
										color(COLOR_PATTERN_EDITOR_AUTOMATION_HL_BEAT));

						} else {

							p->draw_fill_rect(Point(ofs, top_ofs + k * fh - sep + fh),
									Size(w, 1),
									color(COLOR_PATTERN_EDITOR_AUTOMATION_HL_BEAT));
							p->draw_fill_rect(Point(ofs, top_ofs + k * fh - sep), Size(1, fh),
									color(COLOR_PATTERN_EDITOR_AUTOMATION_HL_BEAT));
							p->draw_fill_rect(Point(ofs + w - 1, top_ofs + k * fh - sep),
									Size(1, fh),
									color(COLOR_PATTERN_EDITOR_AUTOMATION_HL_BEAT));
						}

						float prev = -1;
						for (int l = 0; l < fh; l++) {

							Tick at = (fh * row + l) * (TICKS_PER_BEAT / rows_per_beat) / fh;

							float tofs = a->interpolate_offset(current_pattern, at);
							if (prev == -1)
								prev = tofs;

							if (tofs >= 0)
								p->draw_fill_rect(
										Point(ofs + tofs * w, top_ofs + k * fh - sep + l),
										Size(2, 1), c);

							prev = tofs;
						}

						if (has_focus() && idx == cursor.column && cursor.row == row) {

							Point cursor_pos(ofs, top_ofs + k * fh - sep);
							p->draw_rect(cursor_pos - Point(1, 1), Size(w + 1, fh + 1),
									cursorcol);
						}
					}

					Tick pfrom = (v_offset) * (TICKS_PER_BEAT / rows_per_beat);
					Tick pto = (v_offset + visible_rows) * (TICKS_PER_BEAT / rows_per_beat);
					int first;
					int count;
					a->get_points_in_range(current_pattern, pfrom, pto, first, count);
					ca.automation = a;

					for (int l = first; l < first + count; l++) {
						int x = (a->get_point_by_index(current_pattern, l) * w / Automation::VALUE_MAX);
						int y = a->get_point_tick_by_index(current_pattern, l) * fh / (TICKS_PER_BEAT / rows_per_beat) - v_offset * fh;
						p->draw_fill_rect(Point(ofs + x - 2, top_ofs + y - 2 - sep),
								Size(5, 5), cpoint);

						ClickArea::AutomationPoint ap;
						ap.tick = a->get_point_tick_by_index(current_pattern, l);
						ap.x = ofs + x;
						ap.y = top_ofs + y /*- sep*/;
						ap.index = l;
						ca.automation_points.push_back(ap);
					}

					click_areas.push_back(ca);

					ofs += w;
				} break;
			}

			idx++;
		}

		if (drawn) {

			p->draw_fill_rect(Point(ofs, 0), Point(track_sep_w, p_size.y),
					track_sep_color);
			ofs += track_sep_w;
		}
	}

	if (has_focus())
		p->draw_rect(Point(), p_size, color(COLOR_PATTERN_EDITOR_CURSOR));
}

void PatternEditor::set_current_pattern(int p_pattern) {

	current_pattern = p_pattern;
	update();
}

int PatternEditor::get_current_pattern() const {
	return current_pattern;
}

void PatternEditor::set_in_window() {

	track_menu = new PopUpMenu(get_window());
	track_menu->selected_id_signal.connect(this, &PatternEditor::_menu_option);
	track_menu->checked_id_signal.connect(this, &PatternEditor::_menu_optionc);

	auto_menu = new PopUpMenu(get_window());
	auto_menu->selected_id_signal.connect(this, &PatternEditor::_menu_option);
	auto_menu->checked_id_signal.connect(this, &PatternEditor::_menu_optionc);
}

void PatternEditor::_redraw() {
	update();
}
void PatternEditor::_menu_optionc(int p_option, bool) {

	_menu_option(p_option);
}

void PatternEditor::_menu_option(int p_option) {

	switch (p_option) {

		case TRACK_MENU_ADD_COLUMN: {

			undo_redo->begin_action("Add Column");
			undo_redo->do_method(
					song->get_track(current_menu_track), &Track::set_columns,
					song->get_track(current_menu_track)->get_column_count() + 1);
			undo_redo->undo_method(
					song->get_track(current_menu_track), &Track::set_columns,
					song->get_track(current_menu_track)->get_column_count());
			undo_redo->do_method(this, &PatternEditor::_redraw);
			undo_redo->undo_method(this, &PatternEditor::_redraw);
			undo_redo->commit_action();
		} break;
		case TRACK_MENU_REMOVE_COLUMN: {

			undo_redo->begin_action("Remove Column");
			undo_redo->do_method(
					song->get_track(current_menu_track), &Track::set_columns,
					song->get_track(current_menu_track)->get_column_count() - 1);
			undo_redo->undo_method(
					song->get_track(current_menu_track), &Track::set_columns,
					song->get_track(current_menu_track)->get_column_count());
			undo_redo->do_method(this, &PatternEditor::_redraw);
			undo_redo->undo_method(this, &PatternEditor::_redraw);
			undo_redo->commit_action();

		} break;
		case TRACK_MENU_SOLO: {

			undo_redo->begin_action("Solo");
			for (int i = 0; i < song->get_track_count(); i++) {

				undo_redo->do_method(song->get_track(i), &Track::set_muted,
						i != current_menu_track);
				undo_redo->undo_method(song->get_track(i), &Track::set_muted,
						song->get_track(i)->is_muted());
			}
			undo_redo->do_method(this, &PatternEditor::_redraw);
			undo_redo->undo_method(this, &PatternEditor::_redraw);
			undo_redo->commit_action();

		} break;
		case TRACK_MENU_MUTE: {

			undo_redo->begin_action("Mute");
			undo_redo->do_method(song->get_track(current_menu_track), &Track::set_muted,
					!song->get_track(current_menu_track)->is_muted());
			undo_redo->undo_method(song->get_track(current_menu_track),
					&Track::set_muted,
					song->get_track(current_menu_track)->is_muted());
			undo_redo->do_method(this, &PatternEditor::_redraw);
			undo_redo->undo_method(this, &PatternEditor::_redraw);
			undo_redo->commit_action();

		} break;
		case TRACK_MENU_EDIT_AUTOMATIONS: {

		} break;
		case TRACK_MENU_RENAME: {

		} break;
		case TRACK_MENU_MOVE_LEFT: {

			undo_redo->begin_action("Track Move Left");
			undo_redo->do_method(song, &Song::swap_tracks, current_menu_track,
					current_menu_track - 1);
			undo_redo->undo_method(song, &Song::swap_tracks, current_menu_track,
					current_menu_track - 1);
			undo_redo->do_method(this, &PatternEditor::_redraw);
			undo_redo->undo_method(this, &PatternEditor::_redraw);
			undo_redo->commit_action();

		} break;
		case TRACK_MENU_MOVE_RIGHT: {

			undo_redo->begin_action("Track Move Right");
			undo_redo->do_method(song, &Song::swap_tracks, current_menu_track,
					current_menu_track + 1);
			undo_redo->undo_method(song, &Song::swap_tracks, current_menu_track,
					current_menu_track + 1);
			undo_redo->do_method(this, &PatternEditor::_redraw);
			undo_redo->undo_method(this, &PatternEditor::_redraw);
			undo_redo->commit_action();

		} break;
		case TRACK_MENU_REMOVE: {

			undo_redo->begin_action("Remove");
			undo_redo->do_method(song, &Song::remove_track, current_menu_track);
			undo_redo->undo_method(song, &Song::add_track_at_pos,
					song->get_track(current_menu_track),
					current_menu_track);
			undo_redo->do_method(this, &PatternEditor::_redraw);
			undo_redo->undo_method(this, &PatternEditor::_redraw);
			undo_redo->undo_data(song->get_track(current_menu_track));
			undo_redo->commit_action();

		} break;
		case AUTOMATION_MENU_VISIBLE: {

			Automation *a = song->get_track(current_menu_track)
									->get_automation(current_menu_automation);
			undo_redo->begin_action("Toggle Automation Visibility");
			undo_redo->do_method(a, &Automation::set_visible, !a->is_visible());
			undo_redo->undo_method(a, &Automation::set_visible, a->is_visible());
			undo_redo->do_method(this, &PatternEditor::_redraw);
			undo_redo->undo_method(this, &PatternEditor::_redraw);
			undo_redo->commit_action();

		} break;
		case AUTOMATION_MENU_MODE_ROWS: {

			Automation *a = song->get_track(current_menu_track)
									->get_automation(current_menu_automation);
			undo_redo->begin_action("Automation Display Numbers");
			undo_redo->do_method(a, &Automation::set_display_mode,
					Automation::DISPLAY_ROWS);
			undo_redo->undo_method(a, &Automation::set_display_mode,
					a->get_display_mode());
			undo_redo->do_method(this, &PatternEditor::_redraw);
			undo_redo->undo_method(this, &PatternEditor::_redraw);
			undo_redo->commit_action();

		} break;
		case AUTOMATION_MENU_MODE_SMALL: {

			Automation *a = song->get_track(current_menu_track)
									->get_automation(current_menu_automation);
			undo_redo->begin_action("Automation Display Small");
			undo_redo->do_method(a, &Automation::set_display_mode,
					Automation::DISPLAY_SMALL);
			undo_redo->undo_method(a, &Automation::set_display_mode,
					a->get_display_mode());
			undo_redo->do_method(this, &PatternEditor::_redraw);
			undo_redo->undo_method(this, &PatternEditor::_redraw);
			undo_redo->commit_action();

		} break;
		case AUTOMATION_MENU_MODE_LARGE: {

			Automation *a = song->get_track(current_menu_track)
									->get_automation(current_menu_automation);
			undo_redo->begin_action("Automation Display Large");
			undo_redo->do_method(a, &Automation::set_display_mode,
					Automation::DISPLAY_LARGE);
			undo_redo->undo_method(a, &Automation::set_display_mode,
					a->get_display_mode());
			undo_redo->do_method(this, &PatternEditor::_redraw);
			undo_redo->undo_method(this, &PatternEditor::_redraw);
			undo_redo->commit_action();

		} break;
		case AUTOMATION_MENU_MOVE_LEFT: {

			undo_redo->begin_action("Automation Move Left");
			undo_redo->do_method(song->get_track(current_menu_track),
					&Track::swap_automations, current_menu_automation,
					current_menu_automation - 1);
			undo_redo->undo_method(song->get_track(current_menu_track),
					&Track::swap_automations, current_menu_automation,
					current_menu_automation - 1);
			undo_redo->do_method(this, &PatternEditor::_redraw);
			undo_redo->undo_method(this, &PatternEditor::_redraw);
			undo_redo->commit_action();

		} break;
		case AUTOMATION_MENU_MOVE_RIGHT: {

			undo_redo->begin_action("Automation Move Right");
			undo_redo->do_method(song->get_track(current_menu_track),
					&Track::swap_automations, current_menu_automation,
					current_menu_automation + 1);
			undo_redo->undo_method(song->get_track(current_menu_track),
					&Track::swap_automations, current_menu_automation,
					current_menu_automation + 1);
			undo_redo->do_method(this, &PatternEditor::_redraw);
			undo_redo->undo_method(this, &PatternEditor::_redraw);
			undo_redo->commit_action();

		} break;
		case AUTOMATION_MENU_REMOVE: {

			Automation *a = song->get_track(current_menu_track)
									->get_automation(current_menu_automation);
			undo_redo->begin_action("Remove Automation");
			undo_redo->do_method(song->get_track(current_menu_track),
					&Track::remove_automation, current_menu_automation);
			undo_redo->undo_method(song->get_track(current_menu_track),
					&Track::add_automation, a, current_menu_automation);
			undo_redo->do_method(this, &PatternEditor::_redraw);
			undo_redo->undo_method(this, &PatternEditor::_redraw);
			undo_redo->commit_action();

		} break;
	}
}

PatternEditor::PatternEditor(Song *p_song, UndoRedo *p_undo_redo) {

	undo_redo = p_undo_redo;

	v_offset = 0;
	h_offset = 0;

	song = p_song;
	current_pattern = 0;
	rows_per_beat = 4;

	Track *t = new Track;
	t->set_columns(2);
	song->add_track(t);
	visible_rows = 4;
	current_octave = 5;
	cursor_advance = 1;

	cursor.row = 0;
	cursor.field = 0;
	cursor.column = 0;
	cursor.skip = 1;

	t->set_note(0, Track::Pos(0, 0), Track::Note(60, 99));
	t->set_note(0, Track::Pos(TICKS_PER_BEAT, 1),
			Track::Note(88, Track::Note::EMPTY));
	t->set_note(0, Track::Pos(TICKS_PER_BEAT * 4, 1),
			Track::Note(Track::Note::OFF));
	t->set_note(0, Track::Pos(66, 1), Track::Note(63, 33));
	t->set_note(0, Track::Pos(TICKS_PER_BEAT * 3, 0), Track::Note(28, 80));
	t->set_note(0, Track::Pos(TICKS_PER_BEAT * 3 + 1, 0),
			Track::Note(Track::Note::OFF, 30));
	t->add_automation(new Automation(t->get_volume_port()));
	t->add_automation(new Automation(t->get_swing_port()));
	t->add_automation(new Automation(t->get_pan_port()));
	t->get_automation(0)->set_display_mode(Automation::DISPLAY_ROWS);
	t->get_automation(0)->set_point(0, 0, 160);
	t->get_automation(0)->set_point(0, TICKS_PER_BEAT * 3 + 2, 22);
	t->get_automation(0)->set_point(0, TICKS_PER_BEAT * 5, 22);
	t->get_automation(0)->set_point(0, TICKS_PER_BEAT * 5 + 2, 80);
	t->get_automation(0)->set_point(0, TICKS_PER_BEAT * 5 + 4, 44);
	t->get_automation(1)->set_display_mode(Automation::DISPLAY_SMALL);
	t->get_automation(2)->set_display_mode(Automation::DISPLAY_LARGE);
	t->get_automation(2)->set_point(0, 0, 22);
	t->get_automation(2)->set_point(0, TICKS_PER_BEAT * 2, 22);
	t->get_automation(2)->set_point(0, TICKS_PER_BEAT * 3, 44);
	set_focus_mode(FOCUS_ALL);
	grabbing_point = -1;
}
