#ifndef PATTERN_TRACK_H
#define PATTERN_TRACK_H

#include "track.h"


class PatternTrack : public Track{
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
			MAX_PARAM=99
		};

		inline bool is_empty() const { return (command==EMPTY && param==EMPTY); }
		uint8_t command;
		uint8_t param;
		bool operator==(Command p_command) const { return command==p_command.command && param==p_command.param; }

		Command() { command=EMPTY; param=EMPTY; }
	};

	struct Pos {

		Tick tick;
		int column;

		bool operator<(const Pos& p_rval) const { return (tick==p_rval.tick)?(column<p_rval.column):(tick<p_rval.tick); }
		bool operator>(const Pos& p_rval) const { return (tick==p_rval.tick)?(column>p_rval.column):(tick>p_rval.tick); }
		bool operator==(const Pos& p_rval) const { return (tick==p_rval.tick) && (column==p_rval.column); }

		Pos() { tick=0; column=0; }
	};

private:
	Map<int, ValueStream<Pos, Note > > note_data;
	Map<int, ValueStream<Pos, Command > > command_data;
	int note_columns;
	int command_columns;

	float swing;
	int swing_step;
public:

	/* notes */
	void set_note_columns(int p_columns);
	int get_note_columns() const;

	void set_note(int p_pattern, Pos p_pos, Note p_note);
	Note get_note(int p_pattern,Pos p_pos) const;

	/* commands */
	void set_command(int p_pattern, Pos p_pos, Command p_command);
	Command get_command(int p_pattern,Pos p_pos) const;

	void set_command_columns(int p_columns);
	int get_command_columns() const;

	void set_swing(float p_swing);
	float get_swing() const;

	void set_swing_step(int p_swing_step);
	int get_swing_step() const;

	PatternTrack();
};

typedef PatternTrack::Note PatternNote;
typedef PatternTrack::Command PatternCommand;
typedef PatternTrack::Pos PatternPos;

#endif // PATTERN_TRACK_H
