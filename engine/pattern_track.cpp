#include "pattern_track.h"

#if 0
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

void PatternTrack::get_notes_in_range(int p_pattern,const Pos& p_from,const Pos& p_to,List<PosNote> *r_notes ) const {

	Pos from = p_from;
	Pos to = p_to;
	if (from.column>to.column) {
		SWAP(from.column,to.column);
	}
	if (from.tick>to.tick) {
		SWAP(from.tick,to.tick);
	}

	const Map<int, ValueStream<Pos, Note > >::Element *E = note_data.find(p_pattern);

	if (!E)
		return; //nothing! (no pattern i guess)

	const ValueStream<Pos, Note > &vs=E->get();

	int idx = vs.find(p_from);
	if (idx<0)
		idx++;
	while(idx<vs.size() && vs.get_pos(idx).tick<to.tick) {

		int c = vs.get_pos(idx).column;
		if (c>=from.column && c<=to.column) {

			PosNote n;
			n.pos=vs.get_pos(idx);
			n.note=vs[idx];
			r_notes->push_back(n);
		}

		idx++;
	}


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
#endif
