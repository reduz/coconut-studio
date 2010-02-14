#ifndef EDIT_COMMANDS_H
#define EDIT_COMMANDS_H

#include "undo_redo.h"
#include "song.h"
class EditCommands : public UndoRedo {


public:

	void song_pattern_set_beats(Song *p_song, int p_pattern, int p_beats);
	void song_pattern_set_measure(Song *p_song, int p_pattern, int p_measure);
	void song_pattern_set_bars(Song *p_song, int p_pattern, int p_bars);

	void song_order_set(Song *p_song,int p_order, int p_pattern);
	void song_track_add(Song *p_song,Track *p_track,int p_pos=-1);
	void song_track_remove(Song *p_song,int p_pos);


};

#endif // EDIT_COMMANDS_H
