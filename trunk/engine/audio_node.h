#ifndef AUDIO_NODE_H
#define AUDIO_NODE_H

#include "rstring.h"
#include <vector>
#include "dsp/frame.h"
#include "dsp/event.h"
#include "tree_loader.h"
#include "tree_saver.h"


class AudioNode;

/**
 * AudioNodeInfo holds information about a node type, necesary for cataloging and instancing.
 */

struct AudioNodeInfo {


	String caption; ///< Caption of the Node (for node browser menu)
	String short_caption; ///< Short Caption of the Node (for audio graph)
	String description; ///< Short description of the node (for node browser / node info )
	String long_description; ///< Long description of the node (for node browser / node info )
	String unique_ID; ///< Unique String ID of node (so it is reconizable when saving)
	String category; ///< String to categorize this node (for node browser)
	String icon_string; ///< icon string (to look up for an bundled icon - internal nodes)
	int version; ///< node_version, any scheme the node likes as long as it increases.
	AudioNode* (*creation_func)(const AudioNodeInfo *);  ///< creation function for instancing this node
};


/**
 * ControlPort is an abstract base class for exporting node control ports. It holds a little extra information
 * to allow GUIs to edit them.
 */

class ControlPortInfo {

	enum Hint {

		HINT_RANGE, ///< just a range (Default)
		HINT_TOGGLE, ///< valid values 0.0f , 1.0f
		HINT_ENUM, ///< asking integer values to get_Value_as_text will return them
	};

	String name;
	String short_name;
	String prefix;
	String min_text, max_text, zero_text;

	Hint hint;

	float min,max,step,initial;
	std::vector<String> enum_values;

	bool realtime_safe;

	ControlPortInfo() { hint=HINT_RANGE; min=0; max=1; step=0; initial=0; realtime_safe=true; }
};

/**
 * AudioNodeUIData is an optional base class to store User Interface data for a Node UI, in case it has to
 * to persist between every time it is edited.
 */

class AudioNodeUIData {

public:

	virtual ~AudioNodeUIData() {}
};


class AudioNode {

	int _x,_y; // visual coordinate hints in graph
	String _name;
	AudioNodeUIData *_ui_data; // to be used by the UI for persisting stuff
protected:
friend class AudioGraph;

	// these functions are called when entering and exiting from the audio graph.
	virtual void graph_enter() {}
	virtual void graph_exit() {}
public:
	/* Audio Port API */

	virtual int get_in_audio_port_count() const=0;
	virtual int get_out_audio_port_count() const=0;

	//stereo or mono
	virtual FrameType get_in_audio_port_frame_type(int p_port) const=0;
	virtual FrameType get_out_audio_port_frame_type(int p_port) const=0;

	virtual void set_in_audio_port_buffer(int p_port, const void *p_buffer)=0;
	virtual void set_out_audio_port_buffer(int p_port, void *p_buffer)=0;

	/* Event Port API */

	virtual int get_in_event_port_count() const=0;
	virtual int get_out_event_port_count() const=0;

	virtual void set_in_event_port_buffer(int p_port, const Event *p_buffer)=0;
	virtual void set_out_event_port_buffer(int p_port, Event *p_buffer)=0;

	/* Control Port API */


	virtual int get_in_control_port_count() const=0;
	virtual int get_out_control_port_count() const=0;

	virtual void set_in_control_port(int p_port, float p_value)=0;
	virtual void get_in_control_port(int p_port) const=0;
	virtual float get_out_control_port(int p_port) const=0;

	// hint/info

	virtual void set_in_control_port_normalized(int p_port, float p_value);
	virtual float get_in_control_port_normalized(int p_port) const;
	virtual float get_out_control_port_normalized(int p_port) const;

	virtual ControlPortInfo get_in_control_port_info(int p_port) const=0;
	virtual ControlPortInfo get_out_control_port_info(int p_port) const=0;

	virtual String get_in_control_port_value_as_text(int p_port) const=0;
	virtual String get_out_control_port_value_as_text(int p_port) const=0;

	/* AudioNode Info */

	virtual const AudioNodeInfo *get_info() const=0;

	// naming for audio graph
	virtual String get_name() const;
	void set_name(String p_name);

	/* Process */

	struct ProcessInfo {

		int audio_buffer_size;
		int event_buffer_size;
		float mix_rate;
	};

	virtual void process(const ProcessInfo& p_info)=0;
	virtual void set_mix_rate(float p_hz)=0;
	virtual void reset()=0;

	/* Load/Save */

	virtual Error save( TreeSaver * p_tree )=0;
	virtual Error load( TreeLoader * p_tree )=0;

	/* Graph Positioning */

	void set_x(int p_x);
	int get_x() const;
	void set_y(int p_y);
	int get_y() const;


	Error save_file( String p_filename );
	Error load_file( String p_filename );


	// to be used by the UI, DONT TOUCH IT
	void set_ui_data(AudioNodeUIData *p_ui_data);
	AudioNodeUIData *get_ui_data();

	AudioNode();
	virtual ~AudioNode();

};


#endif // AUDIO_NODE_H
