//
// C++ Implementation: keyboard_dialog
//
// Description: 
//
//
// Author: Juan Linietsky <reduz@gmail.com>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "bundles/window_box.h"
#include "containers/center_container.h"
#include "containers/box_container.h"
#include "keyboard_dialog.h"
#include "editor/key_bindings.h"
#include "base/keyboard.h"
#include "widgets/separator.h"

#include <stdio.h>

void KeyboardDialog::keybind_selected( int p_which) {

	update_kb_text();

}

void KeyboardDialog::update_kb_text() {
	
	int idx=list->get_selected();
	if (idx<0)
		return;
	
	unsigned int bind=KeyBind::get_keybind_code( idx );
	if (bind==NO_BIND)
		bind_string->set_text("No KeyBind");
	else
		bind_string->set_text( Keyboard::get_code_name( bind ) );
	
}
void KeyboardDialog::update_keybind_list() {

	
	bind_string->clear();
	list->clear();
	for(int i=0;i<KeyBind::get_keybind_count();i++) {
		
		list->add_string( KeyBind::get_keybind_description(i));
	}
	
}

void KeyboardDialog::kb_change_ok() {
	
	int idx=list->get_selected();
	if (idx<0)
		return;
	
	kb_change_window->hide();
	
	KeyBindList bind_exists=KeyBind::get_bind(kb_change_last);
	
	if (bind_exists!=KB_MAX && bind_exists!=idx) {
		
		mb->show( Keyboard::get_code_name( kb_change_last ) + String(" Already assigned to: ")+KeyBind::get_keybind_description(bind_exists) );
		return;
	}
	
	KeyBind::set_keybind_code(idx,kb_change_last);
	update_kb_text();

}
void KeyboardDialog::key_handler(unsigned long p_unicode, unsigned long p_scan_code,bool p_press,bool p_repeat,int p_modifier_mask) {
	
	if (!p_press)
		return ;
	kb_change_last=p_scan_code|p_modifier_mask;
	kb_change_label->set_text( Keyboard::get_code_name( kb_change_last ) );
	
	return ;
}


void KeyboardDialog::change_cbk() {

	int idx=list->get_selected();
	if (idx<0)
		return;
	
	kb_change_last=KeyBind::get_keybind_code( idx );
	if (kb_change_last==NO_BIND)
		kb_change_label->set_text("No KeyBind");
	else
		kb_change_label->set_text(Keyboard::get_code_name( kb_change_last ));
	
	kb_change_window->show();
}
void KeyboardDialog::clear_cbk() {
	
	int idx=list->get_selected();
	if (idx<0)
		return;
	
	KeyBind::set_keybind_code(idx,NO_BIND);

	update_kb_text();
	
}

void KeyboardDialog::read_slot(String p_section,String p_entry,String p_value) {
	
	if (p_section!="KeyBindings")
		return;

	if (p_entry=="keybind_count") {
		
		allow_kb_load=( (int)p_value.to_double()==KB_MAX );
		
	} else if (p_entry=="keyrepeat_rate") {
		
		spin_rate->get_range()->set( p_value.to_double() );
		
	} else if (p_entry=="keyrepeat_delay") {
		
		spin_delay->get_range()->set( p_value.to_double() );
		
	} else {
	
		if (!allow_kb_load)
			return;
		
		int entrynum=(int)(p_entry.to_double());
		
		if (entrynum<=0 || entrynum>KB_MAX)
			return;
		
		entrynum--;
		
		int code=(int)(p_value.to_double());
		
		KeyBind::set_keybind_code( entrynum, code );
	}	
	
}
void KeyboardDialog::read_finished() {
	
	
	update_kb_text();
}


void KeyboardDialog::save_slot() {
	

	config->set_section( "KeyBindings" );
	config->add_entry( "keybind_count",String::num( KB_MAX ),"Please use UI to edit");
	config->add_entry( "keyrepeat_rate",String::num( spin_rate->get_range()->get() ),"ms");
	config->add_entry( "keyrepeat_delay",String::num( spin_delay->get_range()->get() ),"ms");
	
	for(int i=0;i<KeyBind::get_keybind_count();i++) {
		
		int code=KeyBind::get_keybind_code(i);
		config->add_entry( String::num(i+1) , String::num(code),String( KeyBind::get_keybind_description(i))+" - code is: "+ String(Keyboard::get_code_name( code ) ));
		list->add_string( KeyBind::get_keybind_description(i));
	}
	
}

void KeyboardDialog::keyrepeat_changed(double) {
	
	get_painter()->set_key_repeat( (int)spin_delay->get_range()->get(), (int)spin_rate->get_range()->get() );
}


void KeyboardDialog::set_default_it() {

	KeyBind::set(KB_CURSOR_MOVE_UP, KEY_UP);
	KeyBind::set(KB_CURSOR_MOVE_DOWN,KEY_DOWN );
	KeyBind::set(KB_CURSOR_MOVE_LEFT, KEY_LEFT );
	KeyBind::set(KB_CURSOR_MOVE_RIGHT,KEY_RIGHT );
	KeyBind::set(KB_CURSOR_PAGE_UP, KEY_PAGEUP );
	KeyBind::set(KB_CURSOR_PAGE_DOWN, KEY_PAGEDOWN );
	KeyBind::set(KB_CURSOR_MOVE_UP_1_ROW, KEY_MASK_CMD|KEY_HOME);
	KeyBind::set(KB_CURSOR_MOVE_DOWN_1_ROW, KEY_MASK_CMD|KEY_END);

	KeyBind::set(KB_CURSOR_MOVE_TRACK_LEFT,KEY_MASK_ALT|KEY_LEFT);
	KeyBind::set(KB_CURSOR_MOVE_TRACK_RIGHT,KEY_MASK_ALT|KEY_RIGHT);

	KeyBind::set(KB_CURSOR_HOME, KEY_HOME);
	KeyBind::set(KB_CURSOR_END, KEY_END );
	KeyBind::set(KB_CURSOR_INSERT, KEY_INSERT );
	KeyBind::set(KB_CURSOR_DELETE, KEY_DELETE );
	KeyBind::set(KB_CURSOR_TAB, KEY_TAB );
	KeyBind::set(KB_CURSOR_BACKTAB, KEY_MASK_SHIFT|KEY_TAB );
	KeyBind::set(KB_CURSOR_FIELD_CLEAR, KEY_PERIOD);

	KeyBind::set(KB_PATTERN_CURSOR_SPACING_0,KEY_MASK_ALT|KEY_0);
	KeyBind::set(KB_PATTERN_CURSOR_SPACING_1,KEY_MASK_ALT|KEY_1);
	KeyBind::set(KB_PATTERN_CURSOR_SPACING_2,KEY_MASK_ALT|KEY_2);
	KeyBind::set(KB_PATTERN_CURSOR_SPACING_3,KEY_MASK_ALT|KEY_3);
	KeyBind::set(KB_PATTERN_CURSOR_SPACING_4,KEY_MASK_ALT|KEY_4);
	KeyBind::set(KB_PATTERN_CURSOR_SPACING_5,KEY_MASK_ALT|KEY_5);
	KeyBind::set(KB_PATTERN_CURSOR_SPACING_6,KEY_MASK_ALT|KEY_6);
	KeyBind::set(KB_PATTERN_CURSOR_SPACING_7,KEY_MASK_ALT|KEY_7);
	KeyBind::set(KB_PATTERN_CURSOR_SPACING_8,KEY_MASK_ALT|KEY_8);
	KeyBind::set(KB_PATTERN_CURSOR_SPACING_9,KEY_MASK_ALT|KEY_9);

	KeyBind::set(KB_PATTERN_CURSOR_NOTE_OFF,KEY_BACKQUOTE);
	KeyBind::set(KB_PATTERN_CURSOR_PLAY_FIELD, KEY_4);
	KeyBind::set(KB_PATTERN_CURSOR_PLAY_ROW, KEY_8);

	KeyBind::set(KB_PATTERN_MUTE_TOGGLE_CHANNEL,KEY_F9);
	KeyBind::set(KB_PATTERN_SOLO_TOGGLE_CHANNEL,KEY_F10);

	KeyBind::set(KB_PATTERN_PAN_WINDOW_UP,KEY_MASK_ALT|KEY_UP);
	KeyBind::set(KB_PATTERN_PAN_WINDOW_DOWN,KEY_MASK_ALT|KEY_DOWN);

	KeyBind::set(KB_PATTERN_MARK_BLOCK_BEGIN, KEY_MASK_ALT|KEY_b );
	KeyBind::set(KB_PATTERN_MARK_BLOCK_END, KEY_MASK_ALT|KEY_e);
	KeyBind::set(KB_PATTERN_MARK_COLUMN_ALL, KEY_MASK_ALT|KEY_l );
	KeyBind::set(KB_PATTERN_BLOCK_COPY, KEY_MASK_ALT|KEY_c );
	KeyBind::set(KB_PATTERN_BLOCK_PASTE_INSERT, KEY_MASK_ALT|KEY_p);
	KeyBind::set(KB_PATTERN_BLOCK_PASTE_OVERWRITE, KEY_MASK_ALT|KEY_o);
	KeyBind::set(KB_PATTERN_BLOCK_PASTE_MIX, KEY_MASK_ALT|KEY_m);
	KeyBind::set(KB_PATTERN_BLOCK_UNMARK, KEY_MASK_ALT|KEY_u);
	KeyBind::set(KB_PATTERN_BLOCK_CUT, KEY_MASK_ALT|KEY_z);
	KeyBind::set(KB_PATTERN_BLOCK_SET_CURRENT_VOLUME, KEY_MASK_ALT|KEY_v);
	KeyBind::set(KB_PATTERN_BLOCK_WIPE_STRAY_VOLUMES, KEY_MASK_ALT|KEY_w);
	KeyBind::set(KB_PATTERN_BLOCK_RAMP_VALUES, KEY_MASK_ALT|KEY_k);
	KeyBind::set(KB_PATTERN_BLOCK_AMPLIFY_VOLUMES, KEY_MASK_ALT|KEY_j);
	KeyBind::set(KB_PATTERN_BLOCK_DOUBLE_BLOCK_LENGTH, KEY_MASK_ALT|KEY_f);
	KeyBind::set(KB_PATTERN_BLOCK_HALVE_BLOCK_LENGTH, KEY_MASK_ALT|KEY_g);
	KeyBind::set(KB_PATTERN_FIELD_TOGGLE_MASK, KEY_COMMA);
	KeyBind::set(KB_PATTERN_FIELD_COPY_CURRENT, KEY_RETURN);
	KeyBind::set(KB_PATTERN_FIELD_WRITE_CURRENT, KEY_SPACE );
	KeyBind::set(KB_PATTERN_RAISE_NOTES, KEY_MASK_ALT|KEY_q);
	KeyBind::set(KB_PATTERN_LOWER_NOTES, KEY_MASK_ALT|KEY_a);
	KeyBind::set(KB_PATTERN_RAISE_12_NOTES, KEY_MASK_SHIFT|KEY_MASK_ALT|KEY_q );
	KeyBind::set(KB_PATTERN_LOWER_12_NOTES, KEY_MASK_SHIFT|KEY_MASK_ALT|KEY_a);
	KeyBind::set(KB_PATTERN_NEXT, KEY_KP_PLUS);
	KeyBind::set(KB_PATTERN_PREV, KEY_KP_MINUS);
	KeyBind::set(KB_PATTERN_NEXT_ORDER, KEY_MASK_CMD|KEY_KP_PLUS);
	KeyBind::set(KB_PATTERN_PREV_ORDER, KEY_MASK_CMD|KEY_KP_MINUS);
	KeyBind::set(KB_PIANO_C0, KEY_z);
	KeyBind::set(KB_PIANO_Cs0, KEY_s );
	KeyBind::set(KB_PIANO_D0, KEY_x);
	KeyBind::set(KB_PIANO_Ds0, KEY_d);
	KeyBind::set(KB_PIANO_E0, KEY_c);
	KeyBind::set(KB_PIANO_F0, KEY_v);
	KeyBind::set(KB_PIANO_Fs0, KEY_g );
	KeyBind::set(KB_PIANO_G0, KEY_b);
	KeyBind::set(KB_PIANO_Gs0, KEY_h);
	KeyBind::set(KB_PIANO_A0, KEY_n);
	KeyBind::set(KB_PIANO_As0, KEY_j);
	KeyBind::set(KB_PIANO_B0, KEY_m);
	KeyBind::set(KB_PIANO_C1, KEY_q);
	KeyBind::set(KB_PIANO_Cs1, KEY_2);
	KeyBind::set(KB_PIANO_D1, KEY_w);
	KeyBind::set(KB_PIANO_Ds1, KEY_3);
	KeyBind::set(KB_PIANO_E1, KEY_e);
	KeyBind::set(KB_PIANO_F1, KEY_r);
	KeyBind::set(KB_PIANO_Fs1, KEY_5);
	KeyBind::set(KB_PIANO_G1, KEY_t);
	KeyBind::set(KB_PIANO_Gs1, KEY_6);
	KeyBind::set(KB_PIANO_A1, KEY_y);
	KeyBind::set(KB_PIANO_As1, KEY_7);
	KeyBind::set(KB_PIANO_B1, KEY_u);
	KeyBind::set(KB_PIANO_C2, KEY_i);
	KeyBind::set(KB_PIANO_Cs2, KEY_9);
	KeyBind::set(KB_PIANO_D2, KEY_o);
	KeyBind::set(KB_PIANO_Ds2, KEY_0);
	KeyBind::set(KB_PIANO_E2, KEY_p);
	KeyBind::set(KB_ORDERLIST_INSERT_SEPARATOR,KEY_EQUALS);

	KeyBind::set(KB_CURSOR_WRITE_MASK,KEY_SPACE);

	KeyBind::set(KB_GOTO_PATTERN_SCREEN,KEY_F2);
	KeyBind::set(KB_GOTO_PATTERN_SCREEN_ORDERLIST,KEY_F11);
	KeyBind::set(KB_OCTAVE_RAISE, KEY_KP_MULTIPLY);
	KeyBind::set(KB_OCTAVE_LOWER, KEY_KP_DIVIDE);
	KeyBind::set(KB_PATTERN_SCREEN_SONG_FOLLOW,KEY_MASK_CMD|KEY_f);

	KeyBind::set(KB_PATTERN_SCREEN_OPTIONS,KEY_MASK_SHIFT|KEY_F2);

	KeyBind::set(KB_PLAYER_PLAY_SONG,KEY_F5);
	KeyBind::set(KB_PLAYER_STOP_SONG,KEY_F8);
	KeyBind::set(KB_PLAYER_FF_SONG,KEY_MASK_ALT|KEY_KP_PLUS);
	KeyBind::set(KB_PLAYER_RW_SONG,KEY_MASK_ALT|KEY_KP_MINUS);
	KeyBind::set(KB_PLAYER_PLAY_PATTERN,KEY_F6);
	KeyBind::set(KB_PLAYER_PLAY_CURSOR,KEY_F7);
	KeyBind::set(KB_PLAYER_PLAY_ORDER,KEY_MASK_SHIFT|KEY_F6);

	KeyBind::set(KB_FILE_OPEN,KEY_MASK_CMD|KEY_o);
	KeyBind::set(KB_FILE_SAVE,KEY_MASK_CMD|KEY_s);
	KeyBind::set(KB_FILE_SAVE_AS,KEY_MASK_CMD|KEY_MASK_SHIFT|KEY_s);
	KeyBind::set(KB_DISPLAY_TOGGLE_FULLSCREEN,KEY_MASK_CMD|KEY_RETURN);
	KeyBind::set(KB_UNDO,KEY_MASK_CMD|KEY_z);
	KeyBind::set(KB_REDO,KEY_MASK_CMD|KEY_MASK_SHIFT|KEY_z);
	KeyBind::set(KB_QUIT,KEY_MASK_CMD|KEY_x);

}

KeyboardDialog::KeyboardDialog(Window *p_parent,ConfigApi *p_config) : Window(p_parent,MODE_POPUP,SIZE_TOPLEVEL_CENTER) {
	
	config=p_config;
	WindowBox *vb = new WindowBox("Keyboard Configuration");	
	set_root_frame( vb );
	
	MarginGroup *mg = vb->add( new MarginGroup("Key Binding List:"),1);
	HBoxContainer *hbc = mg->add( new HBoxContainer,1 );
	list=hbc->add( new List,1 );
	list->selected_signal.connect(this,&KeyboardDialog::keybind_selected);
	hbc->add( new VScrollBar )->set_range( list->get_range() );
	mg = vb->add( new MarginGroup("Selected Bind:"));
	bind_string=mg->add( new LineEdit );
	hbc=mg->add( new CenterContainer)->set(new HBoxContainer );
	hbc->add(new Button("Change"))->pressed_signal.connect(this,&KeyboardDialog::change_cbk );
	hbc->add(new Button("Clear"))->pressed_signal.connect(this,&KeyboardDialog::clear_cbk );
	
	update_keybind_list();
	
	kb_change_window = new Window(this,MODE_POPUP,SIZE_CENTER);
	mg = new MarginGroup("Press New KeyBind:        ");
	kb_change_window->set_root_frame( mg );
	kb_change_label=mg->add( new Label());
	kb_change_label->set_align( Label::ALIGN_CENTER );
	Button *b  = mg->add( new CenterContainer)->set(new Button("Apply") );
	b->set_focus_mode( FOCUS_NONE );
	b->pressed_signal.connect(this,&KeyboardDialog::kb_change_ok );
	kb_change_window->key_unhandled_signal.connect(this,&KeyboardDialog::key_handler);
	
	vb->add( new HSeparator );
	
	HBoxContainer *key_hb =vb->add( new HBoxContainer );
	
	spin_delay=key_hb->add( new MarginGroup("Key Repeat Delay (ms):"),1)->add(new SpinBox );
	spin_delay->get_range()->config( 1,1000, get_painter()->get_key_repeat_delay(), 1 );
	spin_delay->get_range()->value_changed_signal.connect(this,&KeyboardDialog::keyrepeat_changed);
	spin_rate=key_hb->add( new MarginGroup("Key Repeat Rate (ms):"),1)->add(new SpinBox );
	spin_rate->get_range()->config( 1,1000, get_painter()->get_key_repeat_rate(), 1 );
	spin_rate->get_range()->value_changed_signal.connect(this,&KeyboardDialog::keyrepeat_changed);
	vb->add( new MarginGroup("Test:"))->add(new LineEdit);
	mb= new MessageBox(this);
	
	config->write_entry_signal.connect( this, &KeyboardDialog::save_slot );	
	config->read_entry_signal.connect( this, &KeyboardDialog::read_slot );
	config->read_finished_signal.connect( this, &KeyboardDialog::read_finished );
	
	allow_kb_load=false;
}


KeyboardDialog::~KeyboardDialog()
{
}


