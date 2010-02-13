#ifndef TRACK_H
#define TRACK_H

#include <vector>
#include "audio_effect.h"
#include "track.h"
#include "value_stream.h"

class Automation {

	ControlPort *port;

	std::map<int,ValueStream<Tick,float> > data;
public:

	void set_point(int p_pattern, Tick p_offset, float p_value);
	bool has_point(int p_pattern, Tick p_offset) const;
	float get_point(int p_pattern, Trick p_offset) const;
	void remove_point(int p_pattern, Tick p_offset, float p_value);

	int get_point_tick_by_index(int p_pattern,int p_index) const;
	float get_point_by_index(int p_pattern,int p_index) const;
	int get_point_count(int p_pattern) const;
	void get_points_in_range(int p_pattern, Tick p_from, Tick p_to, int &r_from, int& r_to) const;

	ControlPort *get_control_port();

	Automation(ControlPort *p_port);

};

class Track : public AudioEffect {

	std::vector<AudioEffect*> effects;


	struct Automation {

	};

public:

	enum Type {
		TYPE_GLOBAL,
		TYPE_SEQUENCE,
		TYPE_AUDIO
	};

	virtual Type get_type() const=0;

	/* audio effects */
	int get_audio_effect_count() const;
	void insert_audio_effect(int p_pos, AudioEffect *p_effect);
	void remove_audio_effect(int p_pos);
	AudioEffect *get_audio_effect(int p_pos);

	Track(Song *p_song);
};

#endif // TRACK_H
