#ifndef SONG_H
#define SONG_H

#include "track.h"

class Song {
public:

	enum {
		DEFAULT_SIGNATURE_BEATS=4,
		DEFAULT_SIGNATURE_MEASURE=4,
		DEFAULT_PATTERN_BARS=4,
		ORDER_MAX=(1<<16),
		ORDER_EMPTY=0xFFFFF,
		ORDER_SKIP=0xFFFFE,
	};

private:

	struct PatternConfig {

		int beats;
		int measure;
		int bars;
		PatternConfig() { beats=DEFAULT_PATTERN_BARS; measure=DEFAULT_SIGNATURE_MEASURE; bars=DEFAULT_PATTERN_BARS; }
	};

	Map<int,PatternConfig> pattern_config;
	Map<int,int> order_list;

	Vector<Track*> tracks;

	void _check_delete_pattern_config(int p_pattern);

public:

	void pattern_set_beats(int p_pattern, int p_beats);
	int pattern_get_beats(int p_pattern) const;
	void pattern_set_measure(int p_pattern, int p_measure);
	int pattern_get_measure(int p_pattern) const;
	void pattern_set_bars(int p_pattern, int p_bars);
	int pattern_get_bars(int p_pattern) const;

	void order_set(int p_order, int p_pattern);
	int order_get(int p_order) const;

	int get_track_count() const;
	void add_track(Track *p_track,int p_pos=-1);
	Track *get_track(int p_pos);
	void remove_track(int p_pos);

	~Song();
	Song();
};

#endif // SONG_H
