#include "pattern_track.h"


void PatternTrack::set_note_columns(int p_columns) {

	_AUDIO_LOCK_

	ERR_FAIL_COND(p_columns<1);

	note_columns=p_columns;
}

int PatternTrack::get_note_columns() const {

	return note_columns;
}

void PatternTrack::set_note(int p_pattern, Pos p_pos, Note p_note) {

	_AUDIO_LOCK_

	if (!note_data.has(p_pattern))
		note_data[p_pattern]=ValueStream<Pos, Note >();

	if (p_note.is_empty()) {
		int idx = note_data[p_pattern].find_exact(p_pos);
		if (idx<0)
			return;

		note_data[p_pattern].erase(idx);
	} else {
		note_data[p_pattern].insert(p_pos,p_note);
	}

}
PatternTrack::Note PatternTrack::get_note(int p_pattern,Pos p_pos) const {

	const Map<int, ValueStream<Pos, Note > >::Element *E = note_data.find(p_pattern);

	if (!E)
		return Note();


	int idx = E->get().find_exact(p_pos);
	if (idx<0)
		return Note();
	else
		return E->get()[idx];

}


void PatternTrack::set_command(int p_pattern, Pos p_pos, Command p_command) {

	_AUDIO_LOCK_

	if (!command_data.has(p_pattern))
		command_data[p_pattern]=ValueStream<Pos, Command >();

	if (p_command.is_empty()) {
		int idx = command_data[p_pattern].find_exact(p_pos);
		if (idx<0)
			return;

		command_data[p_pattern].erase(idx);
	} else {
		command_data[p_pattern].insert(p_pos,p_command);
	}

}

PatternTrack::Command PatternTrack::get_command(int p_pattern,Pos p_pos) const {

	const Map<int, ValueStream<Pos, Command > >::Element *E = command_data.find(p_pattern);

	if (!E)
		return Command();


	int idx = E->get().find_exact(p_pos);
	if (idx<0)
		return Command();
	else
		return E->get()[idx];

}

void PatternTrack::set_command_columns(int p_columns) {

	_AUDIO_LOCK_
	ERR_FAIL_COND(p_columns<0);
}
int PatternTrack::get_command_columns() const {

	return command_columns;
}

void PatternTrack::set_swing(float p_swing) {

	_AUDIO_LOCK_
	swing=p_swing;
}

float PatternTrack::get_swing() const {

	return swing;
}

void PatternTrack::set_swing_step(int p_swing_step) {

	_AUDIO_LOCK_
	swing_step=p_swing_step;
}
int PatternTrack::get_swing_step() const {

	return swing_step;
}

PatternTrack::PatternTrack() {

	swing=0;
	swing_step=1;
	note_columns=1;
	command_columns=0;
}
