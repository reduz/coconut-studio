#include "audio_effect.h"


ControlPort* AudioEffect::get_control_port(int p_idx) const {

	return const_cast<AudioEfect*>(this)->get_control_port();
}
AudioEffect::AudioEffect()
{
}

AudioEffect::~AudioEffect()
{
}
