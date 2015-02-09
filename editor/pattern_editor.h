#ifndef PATTERN_EDITOR_H
#define PATTERN_EDITOR_H

#include "gui/base/widget.h"
#include "engine/song.h"
#include "bundles/popup_menu.h"
#include "engine/undo_redo.h"

using namespace GUI;

class PatternEditor : public Widget {


	enum {
		TRACK_MENU_ADD_COLUMN,
		TRACK_MENU_REMOVE_COLUMN,
		TRACK_MENU_SOLO,
		TRACK_MENU_MUTE,
		TRACK_MENU_EDIT_AUTOMATIONS,
		TRACK_MENU_RENAME,
		TRACK_MENU_MOVE_LEFT,
		TRACK_MENU_MOVE_RIGHT,
		TRACK_MENU_REMOVE,
		AUTOMATION_MENU_VISIBLE,
		AUTOMATION_MENU_MODE_ROWS,
		AUTOMATION_MENU_MODE_SMALL,
		AUTOMATION_MENU_MODE_LARGE,
		AUTOMATION_MENU_MOVE_LEFT,
		AUTOMATION_MENU_MOVE_RIGHT,
		AUTOMATION_MENU_REMOVE
	};

	Song *song;
	int current_pattern;
	int v_offset;
	int rows_per_beat;
	int h_offset;
	int visible_rows;

	struct Cursor {
		int row;
		int column;
		int field;
		int skip;
	} cursor;



	PopUpMenu *track_menu;
	PopUpMenu *auto_menu;

	struct TrackButton {
		int track;
		Rect r;
	};

	List<TrackButton> track_buttons;

	struct AutomationButton {
		int track;
		int automation;
		Rect r;
	};

	int current_menu_track;
	int current_menu_automation;
	void _menu_option(int p_option);
	void _menu_optionc(int p_option,bool);

	UndoRedo *undo_redo;

	List<AutomationButton> automation_buttons;

	int get_total_rows() const;
	int get_visible_rows() const;
	virtual bool key(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask);
	virtual void mouse_button(const Point& p_pos, int p_button,bool p_press,int p_modifier_mask);
	virtual void draw(const Point& p_global,const Size& p_size,const Rect& p_exposed);
	virtual void set_in_window();

	void _validate_cursor();
	void _redraw();

public:

	void set_current_pattern(int p_pattern);
	int get_current_pattern() const;
	PatternEditor(Song * p_song,UndoRedo* p_undo_redo);
};

#endif // PATTERN_EDITOR_H
