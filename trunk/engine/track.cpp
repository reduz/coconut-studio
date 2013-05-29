#include "track.h"


void Automation::set_point(int p_pattern, Tick p_offset, uint8_t p_value) {

	_AUDIO_LOCK_

	if (!data.has(p_pattern)) {

		data[p_pattern]=ValueStream<Tick,uint8_t>();
	}

	data[p_pattern].insert(p_offset,p_value);
}

bool Automation::has_point(int p_pattern, Tick p_offset) const {

	if (!data.has(p_pattern))
		return false;
	return data[p_pattern].find_exact(p_offset)>=0;
}

uint8_t Automation::get_point(int p_pattern, Tick p_offset) const {

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

	const Map<int,ValueStream<Tick,uint8_t> >::Element *E = data.find(p_pattern);
	ERR_FAIL_COND_V(!E,0);


	ERR_FAIL_INDEX_V( p_index, E->get().size(), 0 );
	return E->get().get_pos(p_index);

}

uint8_t Automation::get_point_by_index(int p_pattern,int p_index) const {

	// this is used super often when playing, so it should be more optimized

	const Map<int,ValueStream<Tick,uint8_t> >::Element *E = data.find(p_pattern);
	ERR_FAIL_COND_V(!E,0);
	ERR_FAIL_INDEX_V( p_index, E->get().size(), 0 );
	return E->get()[p_index];

}

int Automation::get_point_count(int p_pattern) const {

	const Map<int,ValueStream<Tick,uint8_t> >::Element *E = data.find(p_pattern);
	if (!E)
		return 0;

	return E->get().size();

}

void Automation::get_points_in_range(int p_pattern, Tick p_from, Tick p_to, int &r_from_idx, int& r_count) const {


	const Map<int,ValueStream<Tick,uint8_t> >::Element *E=data.find(p_pattern);
	if (!E) {
		r_count=0;
		return;
	}

	const ValueStream<Tick,uint8_t>& vs = E->get();

	if (vs.size()==0) {
		r_count=0;
		return;
	}

	int pos_beg = vs.find(p_from);
	int pos_end = vs.find(p_to);

	if (pos_end<0) {
		r_count=0;
		return;
	}

	if (pos_beg<0 || p_from<vs.get_pos(pos_beg))
		pos_beg++;

	r_from_idx=pos_beg;
	r_count=pos_end-pos_beg+1;

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
Automation *Track::get_automation(int p_pos) const{

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

void Track::get_notes_in_range(int p_pattern,const Pos& p_from,const Pos& p_to,int &r_first, int& r_count ) const {


	const Map<int, ValueStream<Pos, Note > >::Element *E=note_data.find(p_pattern);
	if (!E) {
		r_count=0;
		return;
	}

	const ValueStream<Pos,Note>& vs = E->get();

	if (vs.size()==0) {
		r_count=0;
		return;
	}

	int pos_beg = vs.find(p_from);
	int pos_end = vs.find(p_to);

	if (pos_end<0) {
		r_count=0;
		return;
	}

	if (pos_beg<0 || p_from<vs.get_pos(pos_beg))
		pos_beg++;

	r_first=pos_beg;
	r_count=pos_end-pos_beg+1;

}

int Track::get_note_count(int p_pattern) const {

	const Map<int, ValueStream<Pos, Note > >::Element *E=note_data.find(p_pattern);
	if (!E)
		return 0;

	return E->get().size();
}

Track::Note Track::get_note_by_index(int p_pattern,int p_index) const{

	const Map<int, ValueStream<Pos, Note > >::Element *E=note_data.find(p_pattern);
	if (!E)
		return Note();

	const ValueStream<Pos,Note>& vs = E->get();
	ERR_FAIL_INDEX_V(p_index,vs.size(),Note());
	return vs[p_index];

}
Track::Pos Track::get_note_pos_by_index(int p_pattern,int p_index) const{

	const Map<int, ValueStream<Pos, Note > >::Element *E=note_data.find(p_pattern);
	if (!E)
		return Pos();

	const ValueStream<Pos,Note>& vs = E->get();
	ERR_FAIL_INDEX_V(p_index,vs.size(),Pos());
	return vs.get_pos(p_index);

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

	int fromidx;
	int count;
	get_notes_in_range(p_pattern,from,to,fromidx,count);

	for(int i=0;i<count;i++) {
		PosNote pn;
		pn.pos=get_note_pos_by_index(p_pattern,i+fromidx);
		if (pn.pos.column<from.column || pn.pos.column>to.column)
			continue;
		pn.note=get_note_by_index(p_pattern,i+fromidx);
		r_notes->push_back(pn);
	}
}

/////

void Track::set_command_columns(int p_columns) {

	_AUDIO_LOCK_

	ERR_FAIL_COND(p_columns<1);

	command_columns=p_columns;
}

int Track::get_command_columns() const {

	return command_columns;
}

void Track::set_command(int p_pattern, Pos p_pos, Command p_command) {

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
Track::Command Track::get_command(int p_pattern,Pos p_pos) const {

	const Map<int, ValueStream<Pos, Command > >::Element *E = command_data.find(p_pattern);

	if (!E)
		return Command();


	int idx = E->get().find_exact(p_pos);
	if (idx<0)
		return Command();
	else
		return E->get()[idx];

}

void Track::get_commands_in_range(int p_pattern,const Pos& p_from,const Pos& p_to,int &r_first, int& r_count ) const {


	const Map<int, ValueStream<Pos, Command > >::Element *E=command_data.find(p_pattern);
	if (!E) {
		r_count=0;
		return;
	}

	const ValueStream<Pos,Command>& vs = E->get();

	if (vs.size()==0) {
		r_count=0;
		return;
	}

	int pos_beg = vs.find(p_from);
	int pos_end = vs.find(p_to);

	if (pos_end<0) {
		r_count=0;
		return;
	}

	if (pos_beg<0 || p_from<vs.get_pos(pos_beg))
		pos_beg++;

	r_first=pos_beg;
	r_count=pos_end-pos_beg+1;

}

int Track::get_command_count(int p_pattern) const {

	const Map<int, ValueStream<Pos, Command > >::Element *E=command_data.find(p_pattern);
	if (!E)
		return 0;

	return E->get().size();
}

Track::Command Track::get_command_by_index(int p_pattern,int p_index) const{

	const Map<int, ValueStream<Pos, Command > >::Element *E=command_data.find(p_pattern);
	if (!E)
		return Command();

	const ValueStream<Pos,Command>& vs = E->get();
	ERR_FAIL_INDEX_V(p_index,vs.size(),Command());
	return vs[p_index];

}
Track::Pos Track::get_command_pos_by_index(int p_pattern,int p_index) const{

	const Map<int, ValueStream<Pos, Command > >::Element *E=command_data.find(p_pattern);
	if (!E)
		return Pos();

	const ValueStream<Pos,Command>& vs = E->get();
	ERR_FAIL_INDEX_V(p_index,vs.size(),Pos());
	return vs.get_pos(p_index);

}


void Track::get_commands_in_range(int p_pattern,const Pos& p_from,const Pos& p_to,List<PosCommand> *r_commands ) const {

	Pos from = p_from;
	Pos to = p_to;
	if (from.column>to.column) {
		SWAP(from.column,to.column);
	}
	if (from.tick>to.tick) {
		SWAP(from.tick,to.tick);
	}

	int fromidx;
	int count;
	get_commands_in_range(p_pattern,from,to,fromidx,count);

	for(int i=0;i<count;i++) {
		PosCommand pn;
		pn.pos=get_command_pos_by_index(p_pattern,i+fromidx);
		if (pn.pos.column<from.column || pn.pos.column>to.column)
			continue;
		pn.command=get_command_by_index(p_pattern,i+fromidx);
		r_commands->push_back(pn);
	}
}


///
int Track::get_event_column_count() const {

	return note_columns+command_columns+automations.size();
}

void Track::set_event(int p_pattern, int p_column, Tick p_pos, const Event& p_event) {

	ERR_FAIL_INDEX(p_column,get_event_column_count());

	if (p_column<note_columns) {
		//note
		Pos p;
		p.column=p_column;
		p.tick=p_pos;
		set_note(p_pattern,p,p_event);
	} else if (p_column<note_columns+command_columns) {
		//note
		Pos p;
		p.column=p_column-note_columns;
		p.tick=p_pos;
		set_command(p_pattern,p,p_event);

	} else {

		int auto_idx=p_column-note_columns-command_columns;
		get_automation(auto_idx)->set_point(p_pattern,p_pos,p_event);
	}

}

Track::Event Track::get_event(int p_pattern,int p_column, Tick p_pos) const {

	ERR_FAIL_INDEX_V(p_column,get_event_column_count(),Event());

	if (p_column<note_columns) {
		//note
		Pos p;
		p.column=p_column;
		p.tick=p_pos;
		return get_note(p_pattern,p);
	} else if (p_column<note_columns+command_columns) {
		//note
		Pos p;
		p.column=p_column-note_columns;
		p.tick=p_pos;
		return get_command(p_pattern,p);

	} else {

		int auto_idx=p_column-note_columns-command_columns;
		return get_automation(auto_idx)->get_point(p_pattern,p_pos);
	}

}

void Track::get_events_in_range(int p_pattern,const Pos& p_from,const Pos& p_to,List<PosEvent> *r_events ) const {

	Map<Pos,Event> events;

	if (p_from.column<note_columns) {
		//has notes
		List<PosNote> pn;
		Pos end=p_to;
		if (end.column>=note_columns)
			end.column=note_columns-1;

		get_notes_in_range(p_pattern,p_from,end,&pn);

		for(const List<PosNote>::Element *E=pn.front();E;E=E->next()) {

			events.insert(E->get().pos,E->get().note);
		}

	}

	if (p_from.column<note_columns+command_columns && p_to.column>=note_columns) {
		//has commands
		List<PosCommand> pc;
		Pos begin=p_from;
		begin.column-=note_columns;
		Pos end=p_to;
		end.column-=note_columns;
		if (end.column>=command_columns)
			end.column=command_columns-1;

		get_commands_in_range(p_pattern,begin,end,&pc);

		for(const List<PosCommand>::Element *E=pc.front();E;E=E->next()) {

			Pos p=E->get().pos;
			p.column+=note_columns;

			events.insert(p,E->get().command);
		}

	}

	if (p_to.column>=note_columns+command_columns) {
		//has commands

		int begin = p_from.column-(note_columns+command_columns);
		int end = p_to.column-(note_columns+command_columns);


		if (begin<0)
			begin=0;

		if (end>=automations.size())
			end=automations.size()-1;

		for(int i=begin;i<=end;i++) {

			int f,c;
			automations[i]->get_points_in_range(p_pattern,p_from.tick,p_to.tick,f,c);
			for(int j=0;j<c;j++) {

				uint8_t v = automations[i]->get_point_by_index(p_pattern,j+f);
				Tick t = automations[i]->get_point_tick_by_index(p_pattern,j+f);
				Pos p;
				p.column=i+note_columns+command_columns;
				p.tick=t;
				events.insert(p,v);

			}
		}

	}

	//add everything beautifully ordered
	for (Map<Pos,Event>::Element *E=events.front();E;E=E->next()) {

		PosEvent pe;
		pe.pos=E->key();
		pe.event=E->get();
		r_events->push_back(pe);
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
	command_columns=0;

}

