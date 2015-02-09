#include "editor.h"


void Editor::quit_request() {

	quit=true;
}

bool Editor::must_quit() {

	return quit;
}

void Editor::_update_pattern_data() {
	pattern_editor->update();
}

void Editor::_add_track() {

	Track * t = new Track;
	undo_redo.begin_action("Add Track");
	undo_redo.do_method(&song,&Song::add_track,t);
	undo_redo.undo_method(&song,&Song::remove_track,song.get_track_count());
	undo_redo.do_method(this,&Editor::_update_pattern_data);
	undo_redo.undo_method(this,&Editor::_update_pattern_data);
	undo_redo.commit_action();

}

void Editor::_menu_option(int p_option) {

	switch( p_option) {
		case SONG_NEW: {} break;
		case SONG_LOAD: {} break;
		case SONG_SAVE: {} break;
		case SONG_SAVE_AS: {} break;
		case SONG_UNDO: {

			undo_redo.undo();
		} break;
		case SONG_REDO: {

			undo_redo.redo();

		} break;
		case SONG_QUIT:{} break;
	}

}

void Editor::_unhandled_key(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask) {

	if (!p_press || p_repeat)
		return;
	unsigned long sc=p_scan_code|p_modifier_mask;
	if (KeyBind::get_bind(sc)==KB_UNDO)
		_menu_option(SONG_UNDO);
	else if (KeyBind::get_bind(sc)==KB_REDO)
		_menu_option(SONG_REDO);
	else if (KeyBind::get_bind(sc)==KB_FILE_OPEN)
		_menu_option(SONG_LOAD);
	else if (KeyBind::get_bind(sc)==KB_FILE_SAVE)
		_menu_option(SONG_SAVE);
	else if (KeyBind::get_bind(sc)==KB_FILE_SAVE_AS)
		_menu_option(SONG_SAVE_AS);
	else if (KeyBind::get_bind(sc)==KB_QUIT)
		_menu_option(SONG_QUIT);
}


void Editor::_file_menu_press() {

	file_menu->clear();
	file_menu->add_item("New",SONG_NEW);
	file_menu->add_item("Open..",SONG_LOAD,KeyBind::get_keybind_code(KB_FILE_OPEN));
	file_menu->add_item("Save",SONG_NEW,KeyBind::get_keybind_code(KB_FILE_SAVE));
	file_menu->add_item("Save As..",SONG_SAVE_AS,KeyBind::get_keybind_code(KB_FILE_SAVE_AS));
	file_menu->add_separator();
	file_menu->add_item("Undo",SONG_UNDO,KeyBind::get_keybind_code(KB_UNDO));
	file_menu->add_item("Redo",SONG_REDO,KeyBind::get_keybind_code(KB_REDO));
	file_menu->add_separator();
	file_menu->add_item("Quit",SONG_QUIT,KeyBind::get_keybind_code(KB_QUIT));

}


Editor::Editor(Painter *p_painter,Timer *p_timer,Skin *p_skin,ConfigApi* p_config) : Window(p_painter,p_timer,p_skin)
{

	config=p_config;
	quit=false;
	VBoxContainer *vbc = new VBoxContainer;
	set_root_frame(vbc);
	HBoxContainer *top_hbc = vbc->add( new HBoxContainer );

	file_menu=top_hbc->add( new MenuBox );
	file_menu->set_text("Song..");
	file_menu->item_selected_signal.connect(this,&Editor::_menu_option);
	file_menu->pre_show_signal.connect(this,&Editor::_file_menu_press);

	HBoxContainer *pattern_hbc = vbc->add( new HBoxContainer,true );
	VBoxContainer *pattern_vbc = pattern_hbc->add( new VBoxContainer,true );
	HBoxContainer *pattern_menu_hbc = pattern_vbc->add( new HBoxContainer );

	Button * add_track = pattern_menu_hbc->add( new Button("Add Track") );
	add_track->pressed_signal.connect( this, &Editor::_add_track );
	pattern_menu_hbc->add( new Label("  Pattern: "));
	pattern_index = pattern_menu_hbc->add( new SpinBox );
	pattern_menu_hbc->add( new Label("  Length: "));
	pattern_length = pattern_menu_hbc->add( new SpinBox );
	pattern_editor = pattern_vbc->add( new PatternEditor(&song,&undo_redo),1 );

	key_dialog = new KeyboardDialog(this,config);
	key_dialog->set_default_it();
	key_unhandled_signal.connect(this,&Editor::_unhandled_key);
}
