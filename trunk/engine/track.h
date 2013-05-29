#ifndef TRACK_H
#define TRACK_H


#include "vector.h"
#include "map.h"
#include "list.h"
#include "audio_effect.h"
#include "value_stream.h"
#include "audio_lock.h"


#define TICKS_PER_BEAT 192
typedef uint64_t Tick;


class Automation {
public:

	enum DisplayMode {
		DISPLAY_ROWS,
		DISPLAY_SMALL,
		DISPLAY_LARGE
	};

	enum {

		EMPTY=255
	};

private:
	AudioEffect *owner;
	ControlPort *port;
	DisplayMode display_mode;
	bool visible;

	Map<int,ValueStream<Tick,uint8_t> > data;
public:

	void set_point(int p_pattern, Tick p_offset, uint8_t p_value);
	bool has_point(int p_pattern, Tick p_offset) const;
	uint8_t get_point(int p_pattern, Tick p_offset) const;
	void remove_point(int p_pattern, Tick p_offset);

	Tick get_point_tick_by_index(int p_pattern,int p_index) const;
	uint8_t get_point_by_index(int p_pattern,int p_index) const;
	int get_point_count(int p_pattern) const;
	void get_points_in_range(int p_pattern, Tick p_from, Tick p_to, int &r_from_idx, int& r_count) const;

	ControlPort *get_control_port();
	AudioEffect *get_owner();


	bool is_visible() const;
	DisplayMode get_display_mode() const;

	Automation(ControlPort *p_port, AudioEffect *p_owner=NULL);

};

class Track : public AudioEffect {
public:
	struct Note {

		enum {
			EMPTY=0xFF,
			OFF=0xFE,
			MAX_VOLUME=99
		};

		uint8_t note;
		uint8_t volume;
		inline bool is_empty() const { return (note==EMPTY && volume==EMPTY); }
		bool operator==(Note p_note) const { return note==p_note.note && volume==p_note.volume; }

		Note() { note=EMPTY; volume=EMPTY; }
	};

	struct Command {

		enum {
			EMPTY=0xFF,
			OFF=0xFE,
			MAX_PARAMETER=99
		};

		uint8_t command;
		uint8_t parameter;
		inline bool is_empty() const { return (command==EMPTY && parameter==EMPTY); }
		bool operator==(Command p_command) const { return command==p_command.command && parameter==p_command.parameter; }

		Command() { command=EMPTY; parameter=EMPTY; }
	};

	struct Pos {

		Tick tick;
		int column;

		bool operator<(const Pos& p_rval) const { return (tick==p_rval.tick)?(column<p_rval.column):(tick<p_rval.tick); }
		bool operator>(const Pos& p_rval) const { return (tick==p_rval.tick)?(column>p_rval.column):(tick>p_rval.tick); }
		bool operator==(const Pos& p_rval) const { return (tick==p_rval.tick) && (column==p_rval.column); }

		Pos() { tick=0; column=0; }
	};

	//generic event?
	struct Event {

		enum Type {
			TYPE_NOTE,
			TYPE_COMMAND,
			TYPE_AUTOMATION
		};

		Type type;
		uint8_t a;
		uint8_t b;

		operator uint8_t() const {
			// to automation
			if (type!=TYPE_AUTOMATION)
				return Automation::EMPTY;
			else
				return a;
		}

		operator Note() const {

			if (type!=TYPE_NOTE)
				return Note();
			else {

				Note n;
				n.note=a;
				n.volume=b;
				return n;
			}

		}

		operator Command() const {

			if (type!=TYPE_COMMAND)
				return Command();
			else {

				Command c;
				c.command=a;
				c.parameter=b;
				return c;
			}

		}

		Event(const Note& p_note) {
			type=TYPE_NOTE;
			a=p_note.note;
			b=p_note.volume;
		}

		Event(const Command& p_command) {
			type=TYPE_COMMAND;
			a=p_command.command;
			b=p_command.parameter;
		}

		Event(const uint8_t p_autoval) {

			type=TYPE_AUTOMATION;
			a=p_autoval;
			b=0;
		}

		Event() { type=TYPE_NOTE; a=Note::EMPTY; b=Note::EMPTY; }
	};

	struct PosNote {
		Pos pos;
		Note note;
	};

	struct PosCommand {
		Pos pos;
		Command command;
	};

	struct PosEvent {
		Pos pos;
		Event event;
	};

private:

	Map<int, ValueStream<Pos, Note > > note_data;
	int note_columns;

	Map<int, ValueStream<Pos, Command > > command_data;
	int command_columns;

	float swing;
	int swing_step;

	Vector<AudioEffect*> effects;
	Vector<Automation*> automations;

public:

	/* audio effects */

	int get_audio_effect_count() const;
	void add_audio_effect(AudioEffect *p_effect,int p_pos=-1);
	void remove_audio_effect(int p_pos);
	AudioEffect *get_audio_effect(int p_pos);

	/* automations */

	int get_automation_count() const;
	void add_automation(Automation *p_automation,int p_pos=-1);
	void remove_automation(int p_pos);
	Automation *get_automation(int p_pos) const;

	virtual int get_internal_automation_count() const=0;
	virtual Automation *internal_automation_get(int p_pos)=0;

	/* notes */

	void set_note_columns(int p_columns);
	int get_note_columns() const;

	void set_note(int p_pattern, Pos p_pos, Note p_note);
	Note get_note(int p_pattern,Pos p_pos) const;

	int get_note_count(int p_pattern) const;
	Note get_note_by_index(int p_pattern,int p_index) const;
	Pos get_note_pos_by_index(int p_pattern,int p_index) const;

	void get_notes_in_range(int p_pattern,const Pos& p_from,const Pos& p_to,int &r_first, int& r_count ) const;
	void get_notes_in_range(int p_pattern,const Pos& p_from,const Pos& p_to,List<PosNote> *r_notes ) const;

	/* commands */

	void set_command_columns(int p_columns);
	int get_command_columns() const;

	void set_command(int p_pattern, Pos p_pos, Command p_command);
	Command get_command(int p_pattern,Pos p_pos) const;

	int get_command_count(int p_pattern) const;
	Command get_command_by_index(int p_pattern,int p_index) const;
	Pos get_command_pos_by_index(int p_pattern,int p_index) const;

	void get_commands_in_range(int p_pattern,const Pos& p_from,const Pos& p_to,int &r_first, int& r_count ) const;
	void get_commands_in_range(int p_pattern,const Pos& p_from,const Pos& p_to,List<PosCommand> *r_commands ) const;


	/* swingie */

	void set_swing(float p_swing);
	float get_swing() const;

	void set_swing_step(int p_swing_step);
	int get_swing_step() const;


	int get_event_column_count() const;
	void set_event(int p_pattern, int p_column, Tick p_pos, const Event& p_event);
	Event get_event(int p_pattern,int p_column, Tick p_pos) const;
	void get_events_in_range(int p_pattern,const Pos& p_from,const Pos& p_to,List<PosEvent> *r_events ) const;

	Track();
};

#endif // TRACK_H
