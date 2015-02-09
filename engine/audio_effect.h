#ifndef AUDIO_EFFECT_H
#define AUDIO_EFFECT_H

#include "rstring.h"
#include <vector>
#include "dsp/frame.h"
#include "dsp/event.h"
#include "tree_loader.h"
#include "tree_saver.h"

class AudioEffect;

struct AudioEffectInfo {

	String caption; ///< Caption of the Node (for node browser menu)
	String short_caption; ///< Short Caption of the Node (for audio graph)
	String description; ///< Short description of the node (for node browser / node info )
	String long_description; ///< Long description of the node (for node browser / node info )
	String unique_ID; ///< Unique String ID of node (so it is reconizable when saving)
	String category; ///< String to categorize this node (for node browser)
	String icon_string; ///< icon string (to look up for an bundled icon - internal nodes)
	int version; ///< node_version, any scheme the node likes as long as it increases.
	AudioEffect* (*creation_func)(const AudioEffectInfo *);  ///< creation function for instancing this node
};

struct PortRangeHint {
	float min,max,def;
	String max_str,min_str;
};



class ControlPort {
public:

	enum Hint {

		HINT_RANGE, ///< just a range (Default)
		HINT_TOGGLE, ///< valid values 0.0f , 1.0f
		HINT_ENUM, ///< asking integer values to get_Value_as_text will return them
	};

	virtual String get_name() const =0;

	virtual float get_min() const=0;
	virtual float get_max() const=0;
	virtual float get_step() const=0;
	virtual float get_initial() const=0;
	virtual float get() const=0;

	virtual void set(float p_val,bool p_make_initial=false)=0; //set, optionally make the value the default too
	virtual void set_normalized(float p_val,bool p_make_initial=false); // set in range 0-1, internally converted to range
	virtual float get_normalized() const;

	virtual String convert_value_to_text(float p_value) const;
	virtual String get_value_as_text() const;
	virtual Hint get_hint() const;

	ControlPort();
	virtual ~ControlPort();
};

class AudioEffect {
public:
	//process
	virtual bool process(const AudioFrame2 *p_in,const AudioFrame2 *p_chain, AudioFrame2* p_out, const Event* p_events, int p_frames, bool p_prev_active)=0;

	//info
	virtual bool has_sidechain() const=0;
	virtual bool has_synth() const=0;

	virtual const AudioEffectInfo * get_info() const=0;

	virtual int get_control_port_count() const=0;
	virtual ControlPort* get_control_port(int p_port)=0;
	const ControlPort* get_control_port(int p_port) const; //const

	virtual void set_mix_rate(float p_hz)=0;
	virtual void set_block_size(int p_size)=0;
	virtual void reset()=0;

	/* Load/Save */

	virtual Error save( TreeSaver * p_tree )=0;
	virtual Error load( TreeLoader * p_tree )=0;


	AudioEffect();
	virtual ~AudioEffect();
};

class ControlPortDefault : public ControlPort {
public:

	String name;
	float min,max,step,initial;
	float value;
	Hint hint;

	virtual String get_name() const { return name; }


	virtual float get_min() const { return min; }
	virtual float get_max() const { return max; }
	virtual float get_step() const { return step; }
	virtual float get_initial() const { return initial; }
	virtual float get() const { return value; }

	virtual void set(float p_val,bool p_make_initial=false) {
		value=p_val;
		if (p_make_initial)
			initial=value;
	}

	virtual Hint get_hint() const { return hint; }

	ControlPortDefault() { hint=HINT_RANGE; }
};


#endif // AUDIO_EFFECT_H
