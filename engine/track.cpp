#include "track.h"


void Automation::set_point(int p_pattern, Tick p_offset, float p_value) {

	_AUDIO_LOCK_

	if (!data.has(p_pattern)) {

		data[p_pattern]=ValueStream<Tick,float>();
	}

	data[p_pattern].insert(p_offset,p_value);
}

bool Automation::has_point(int p_pattern, Tick p_offset) const {

	if (!data.has(p_pattern))
		return false;
	return data[p_pattern].find_exact(p_offset)>=0;
}

float Automation::get_point(int p_pattern, Tick p_offset) const {

	ERR_FAIL_COND_V( !data.has(p_pattern), 0 );

	int idx = data[p_pattern].find_exact(p_offset);
	ERR_FAIL_COND_V( idx < 0, 0);
	return data[p_pattern][idx];
}

void Automation::remove_point(int p_pattern, Tick p_offset) {

	if ( !data.has(p_pattern) )
		return;

	_AUDIO_LOCK_

	int idx = data[p_pattern].find_exact(p_offset);
	if (idx < 0 )
		return;
	data[p_pattern].erase(idx);
	if (data[p_pattern].size()==0)
		data.erase(p_pattern);

}


Tick Automation::get_point_tick_by_index(int p_pattern,int p_index) const {

	// this is used super often when playing, so it should be more optimized

	const Map<int,ValueStream<Tick,float> >::Element *E = data.find(p_pattern);
	ERR_FAIL_COND_V(!E,0);


	ERR_FAIL_INDEX_V( p_index, E->get().size(), 0 );
	return E->get().get_pos(p_index);

}

float Automation::get_point_by_index(int p_pattern,int p_index) const {

	// this is used super often when playing, so it should be more optimized

	const Map<int,ValueStream<Tick,float> >::Element *E = data.find(p_pattern);
	ERR_FAIL_COND_V(!E,0);
	ERR_FAIL_INDEX_V( p_index, E->get().size(), 0 );
	return E->get()[p_index];

}

int Automation::get_point_count(int p_pattern) const {

	const Map<int,ValueStream<Tick,float> >::Element *E = data.find(p_pattern);
	if (!E)
		return 0;

	return E->get().size();

}

void Automation::get_points_in_range(int p_pattern, Tick p_from, Tick p_to, int &r_from_idx, int& r_to_idx) const {


}


ControlPort *Automation::get_control_port() {

	return port;
}

AudioEffect *Automation::get_owner() {

	return owner;
}


bool Automation::is_visible() const {

	return visible;
}

Automation::DisplayMode Automation::get_display_mode() const {

	return display_mode;
}

Automation::Automation(ControlPort *p_port, AudioEffect *p_owner) {

	port=p_port;
	owner=p_owner;
}


/* audio effects */

int Track::get_audio_effect_count() const {

	return effects.size();
}

void Track::add_audio_effect(AudioEffect *p_effect,int p_pos) {

	_AUDIO_LOCK_

	if (p_pos<0)
		p_pos=effects.size();
	ERR_FAIL_COND(p_pos > effects.size());
	effects.insert(p_pos,p_effect);
}

void Track::remove_audio_effect(int p_pos) {

	_AUDIO_LOCK_
	ERR_FAIL_INDEX(p_pos,effects.size());
	AudioEffect *fx = effects[p_pos];

	for(int i=0;i<automations.size();i++) {

		if (automations[i]->get_owner()==fx) {

			automations.remove(i);
			i--;
		}
	}

	effects.remove(p_pos);
}

AudioEffect *Track::get_audio_effect(int p_pos) {

	ERR_FAIL_INDEX_V(p_pos,effects.size(),NULL);
	return effects[p_pos];
}

/* automations */

int Track::get_automation_count() const {

	return automations.size();
}
void Track::add_automation(Automation *p_automation,int p_pos) {

	_AUDIO_LOCK_
	if (p_pos<0)
		p_pos=automations.size();
	ERR_FAIL_COND(p_pos > automations.size());
	automations.insert(p_pos,p_automation);

}
void Track::remove_automation(int p_pos) {

	_AUDIO_LOCK_
	ERR_FAIL_INDEX(p_pos,automations.size());
	automations.remove(p_pos);


}
Automation *Track::get_automation(int p_pos) {

	ERR_FAIL_INDEX_V(p_pos,automations.size(),NULL);
	return automations[p_pos];

}

void Track::set_note_columns(int p_columns) {

	_AUDIO_LOCK_

	ERR_FAIL_COND(p_columns<1);

	note_columns=p_columns;
}

int Track::get_note_columns() const {

	return note_columns;
}

void Track::set_note(int p_pattern, Pos p_pos, Note p_note) {

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
Track::Note Track::get_note(int p_pattern,Pos p_pos) const {

	const Map<int, ValueStream<Pos, Note > >::Element *E = note_data.find(p_pattern);

	if (!E)
		return Note();


	int idx = E->get().find_exact(p_pos);
	if (idx<0)
		return Note();
	else
		return E->get()[idx];

}

void Track::get_notes_in_range(int p_pattern,const Pos& p_from,const Pos& p_to,List<PosNote> *r_notes ) const {

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



void Track::set_swing(float p_swing) {

	_AUDIO_LOCK_
	swing=p_swing;
}

float Track::get_swing() const {

	return swing;
}

void Track::set_swing_step(int p_swing_step) {

	_AUDIO_LOCK_
	swing_step=p_swing_step;
}
int Track::get_swing_step() const {

	return swing_step;
}

Track::Track() {

	swing=0;
	swing_step=1;
	note_columns=1;

}

