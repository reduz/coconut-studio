#ifndef TRACK_H
#define TRACK_H

#include "vector.h"
#include "map.h"
#include "audio_effect.h"
#include "value_stream.h"
#include "audio_lock.h"

#define TICKS_PER_BEAT 192
typedef uint64_t Tick;

class Automation {

	AudioEffect *owner;
	ControlPort *port;

	Map<int,ValueStream<Tick,float> > data;
public:

	void set_point(int p_pattern, Tick p_offset, float p_value);
	bool has_point(int p_pattern, Tick p_offset) const;
	float get_point(int p_pattern, Tick p_offset) const;
	void remove_point(int p_pattern, Tick p_offset);

	Tick get_point_tick_by_index(int p_pattern,int p_index) const;
	float get_point_by_index(int p_pattern,int p_index) const;
	int get_point_count(int p_pattern) const;
	void get_points_in_range(int p_pattern, Tick p_from, Tick p_to, int &r_from_idx, int& r_to_idx) const;

	ControlPort *get_control_port();
	AudioEffect *get_owner();

	Automation(ControlPort *p_port, AudioEffect *p_owner=NULL);

};

class Track : public AudioEffect {

	Vector<AudioEffect*> effects;
	Vector<Automation*> automations;

public:

	enum Type {
		TYPE_GLOBAL,
		TYPE_PATTERN,
		TYPE_AUDIO
	};

	virtual Type get_type() const=0;

	/* audio effects */

	int get_audio_effect_count() const;
	void add_audio_effect(AudioEffect *p_effect,int p_pos=-1);
	void remove_audio_effect(int p_pos);
	AudioEffect *get_audio_effect(int p_pos);

	/* automations */

	int get_automation_count() const;
	void add_automation(Automation *p_automation,int p_pos=-1);
	void remove_automation(int p_pos);
	Automation *get_automation(int p_pos);

	virtual int get_internal_automation_count() const=0;
	virtual Automation *internal_automation_get(int p_pos)=0;

	Track();
};

#endif // TRACK_H
