#include "edit_commands.h"


void EditCommands::song_pattern_set_beats(Song *p_song, int p_pattern, int p_beats) {


	String action_name = "Pattern "+String::num(p_pattern)+" Set Beats";
	int old_beats = p_song->pattern_get_beats(p_pattern);
	add_action(
		action_name,
		command(p_song,&Song::pattern_set_beats,p_pattern,p_beats),
		command(p_song,&Song::pattern_set_beats,p_pattern,old_beats)
	);

}

void EditCommands::song_pattern_set_measure(Song *p_song, int p_pattern, int p_measure) {

	String action_name = "Pattern "+String::num(p_pattern)+" Set Measure";
	int old_measure = p_song->pattern_get_measure(p_pattern);
	add_action(
		action_name,
		command(p_song,&Song::pattern_set_measure,p_pattern,p_measure),
		command(p_song,&Song::pattern_set_measure,p_pattern,old_measure)
	);

}
void EditCommands::song_pattern_set_bars(Song *p_song, int p_pattern, int p_bars) {

	String action_name = "Pattern "+String::num(p_pattern)+" Set Bars";
	int old_bars = p_song->pattern_get_bars(p_pattern);
	add_action(
		action_name,
		command(p_song,&Song::pattern_set_bars,p_pattern,p_bars),
		command(p_song,&Song::pattern_set_bars,p_pattern,old_bars)
	);

}

void EditCommands::song_order_set(Song *p_song,int p_order, int p_pattern) {

	String action_name = "Set Order "+String::num(p_order);
	int old_pattern = p_song->order_get(p_order);
	add_action(
		action_name,
		command(p_song,&Song::order_set,p_order,p_pattern),
		command(p_song,&Song::order_set,p_order,old_pattern)
	);

}
void EditCommands::song_track_add(Song *p_song,Track *p_track,int p_pos) {

	String type;
	switch(p_track->get_type()) {
		case Track::TYPE_AUDIO: type="Audio"; break;
		case Track::TYPE_PATTERN: type="Pattern"; break;
		case Track::TYPE_GLOBAL: type="Global"; break;
	}

	String action_name = "Add "+type+" Track";
	// validate p_pos
	if (p_pos<0)
		p_pos=p_song->get_track_count(); //by default add last
	else if (p_pos>p_song->get_track_count())
		p_pos=p_song->get_track_count();

	add_action(
		action_name,
		command(p_song,&Song::add_track,p_track,p_pos)->with_data(p_track),
		command(p_song,&Song::remove_track,p_pos)
	);

}
void EditCommands::song_track_remove(Song *p_song,int p_pos) {

	String action_name = "Remove Track "+String::num(p_pos);
	// validate p_pos
	if (p_pos<0 || p_pos>=p_song->get_track_count())
		return;

	Track *track = p_song->get_track(p_pos);
	ERR_FAIL_COND(!track);

	add_action(
		action_name,
		command(p_song,&Song::remove_track,p_pos),
		command(p_song,&Song::add_track,track,p_pos)->with_data(track)
	);

}

