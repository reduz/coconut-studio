#ifndef EDITOR_H
#define EDITOR_H

#include "base/window.h"
#include "engine/song.h"
#include "cskin.h"
#include "containers/box_container.h"
#include "bundles/menu_box.h"
#include "gui/widgets/label.h"
#include "bundles/spin_box.h"
#include "pattern_editor.h"
#include "engine/undo_redo.h"
#include "keyboard_dialog.h"
#include "editor/key_bindings.h"
using namespace GUI;

class Editor : public Window {

	enum {
		SONG_NEW,
		SONG_LOAD,
		SONG_SAVE,
		SONG_SAVE_AS,
		SONG_UNDO,
		SONG_REDO,
		SONG_QUIT
	};

	KeyBind keybind;
	Song song;
	bool quit;
	MenuBox *file_menu;
	SpinBox *pattern_index;
	SpinBox *pattern_length;
	PatternEditor *pattern_editor;

	void _update_pattern_data();
	UndoRedo undo_redo;

	void _menu_option(int p_option);
	void _add_track();
	KeyboardDialog *key_dialog;
	ConfigApi *config;

	void _unhandled_key(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask);
	void _file_menu_press();

public:

	void quit_request();
	bool must_quit();
	Editor(Painter *p_painter,Timer *p_timer,Skin *p_skin,ConfigApi *p_config);
};

#endif // EDITOR_H
