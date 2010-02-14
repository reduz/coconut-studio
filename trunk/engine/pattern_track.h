#ifndef PATTERN_TRACK_H
#define PATTERN_TRACK_H

#include "track.h"


struct Note {

	enum {
		EMPTY=0xFF,
		OFF=0xFE,
		MAX_VOLUME=99
	};

	uint8_t note;
	uint8_t volume;
	inline bool is_empty() const { return (note==EMPTY && volume==EMPTY); }

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

	Command() { command=EMPTY; param=EMPTY; }
};

struct EventPos {

	Tick tick;
	int column;

	bool operator<(const EventPos& p_rval) const { return (tick==p_rval.tick)?(column<p_rval.column):(tick<p_rval.tick); }
	bool operator>(const EventPos& p_rval) const { return (tick==p_rval.tick)?(column>p_rval.column):(tick>p_rval.tick); }
	bool operator==(const EventPos& p_rval) const { return (tick==p_rval.tick) && (column==p_rval.column); }

	EventPos() { tick=0; column=0; }
};

class PatternTrack : public Track{

	Map<int, ValueStream<EventPos, Note > > note_data;
	Map<int, ValueStream<EventPos, Command > > command_data;
	int note_columns;
	int command_columns;

	float swing;
	int swing_step;
public:

	/* notes */
	void set_note_columns(int p_columns);
	int get_note_columns() const;

	void note_set(int p_pattern, const EventPos& p_pos, const Note& p_note);
	Note note_get(int p_pattern,const EventPos& p_pos) const;

	/* commands */
	void command_set(int p_pattern, const EventPos& p_pos, const Command& p_command);
	Command command_get(int p_pattern,const EventPos& p_pos) const;

	void set_command_columns(int p_columns);
	int get_command_columns() const;

	void set_swing(float p_swing);
	float get_swing() const;

	void set_swing_step(int p_swing_step);
	int get_swing_step() const;

	PatternTrack();
};

#endif // PATTERN_TRACK_H
