#ifndef AUDIO_EFFECT_H
#define AUDIO_EFFECT_H


struct PortRangeHint {
	float min,max,def;
	String max_str,min_str;
};


class AudioEffect {
public:
	//process
	bool process(const Frame2 *p_in,const Frame2 *p_chain, Frame2* p_out, const Event* p_events, int p_frames, bool p_prev_active)=0;

	//info
	bool has_sidechain() const=0;
	bool has_synth() const=0;

	int get_port_count() const=0;
	String get_port_name(int p_port) const=0;
	String get_port_caption(int p_port) const=0;
	PortRangeHint get_port_range_hint(int p_port) const=0;

	//port (realtime)
	void set_port_value(int p_port, float p_value)=0;
	float get_port_value(int p_port) const=0;

	String get_port_value_as_string(int p_port) const; //not realtime

	void set_mix_rate(float p_hz);


	AudioEffect();
};

#endif // AUDIO_EFFECT_H
