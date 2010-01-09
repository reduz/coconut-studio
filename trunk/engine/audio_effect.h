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


class AudioEffect {
public:
	//process
	virtual bool process(const Frame2 *p_in,const Frame2 *p_chain, Frame2* p_out, const Event* p_events, int p_frames, bool p_prev_active)=0;

	//info
	virtual bool has_sidechain() const=0;
	virtual bool has_synth() const=0;

	virtual const AudioEffectInfo * get_info() const=0;

	virtual int get_port_count() const=0;
	virtual String get_port_name(int p_port) const=0;
	virtual String get_port_caption(int p_port) const=0;
	virtual PortRangeHint get_port_range_hint(int p_port) const=0;

	//port (realtime)
	virtual void set_port_value(int p_port, float p_value)=0;
	virtual float get_port_value(int p_port) const=0;

	virtual String get_port_value_as_string(int p_port) const; //not realtime

	virtual void set_mix_rate(float p_hz)=0;
	virtual void reset()=0;

	/* Load/Save */

	virtual Error save( TreeSaver * p_tree )=0;
	virtual Error load( TreeLoader * p_tree )=0;


	AudioEffect();
	virtual ~AudioEffect();
};

#endif // AUDIO_EFFECT_H
