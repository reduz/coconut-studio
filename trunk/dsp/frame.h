#ifndef FRAME_H
#define FRAME_H

#include "typedefs.h"
typedef float Frame;

struct Frame2 {

	Frame l,r;


	_FORCE_INLINE_ const Frame& operator[](int idx) const { return idx==0?l:r; }
	_FORCE_INLINE_ Frame& operator[](int idx) { return idx==0?l:r; }
	_FORCE_INLINE_ Frame2 operator+(const Frame2& p_frame2) const { return Frame2(l+p_frame2.l,r+p_frame2.r); }
	_FORCE_INLINE_ Frame2 operator-(const Frame2& p_frame2) const { return Frame2(l-p_frame2.l,r-p_frame2.r); }
	_FORCE_INLINE_ Frame2 operator*(const Frame2& p_frame2) const { return Frame2(l*p_frame2.l,r*p_frame2.r); }
	_FORCE_INLINE_ Frame2 operator/(const Frame2& p_frame2) const { return Frame2(l/p_frame2.l,r/p_frame2.r); }

	_FORCE_INLINE_ void operator+=(const Frame2& p_frame2) { l+=p_frame2.l;r+=p_frame2.r; }
	_FORCE_INLINE_ void operator-=(const Frame2& p_frame2) { l-=p_frame2.l;r-=p_frame2.r; }
	_FORCE_INLINE_ void operator*=(const Frame2& p_frame2) { l*=p_frame2.l;r*=p_frame2.r; }
	_FORCE_INLINE_ void operator/=(const Frame2& p_frame2) { l/=p_frame2.l;r/=p_frame2.r; }

	_FORCE_INLINE_ Frame2 operator+(const Frame& p_frame) const { return Frame2(l+p_frame,r+p_frame); }
	_FORCE_INLINE_ Frame2 operator-(const Frame& p_frame) const { return Frame2(l-p_frame,r-p_frame); }
	_FORCE_INLINE_ Frame2 operator*(const Frame& p_frame) const { return Frame2(l*p_frame,r*p_frame); }
	_FORCE_INLINE_ Frame2 operator/(const Frame& p_frame) const { return Frame2(l/p_frame,r/p_frame); }

	_FORCE_INLINE_ void operator+=(const Frame& p_frame) { l+=p_frame; r+=p_frame; }
	_FORCE_INLINE_ void operator-=(const Frame& p_frame) { l-=p_frame; r-=p_frame; }
	_FORCE_INLINE_ void operator*=(const Frame& p_frame) { l*=p_frame; r*=p_frame; }
	_FORCE_INLINE_ void operator/=(const Frame& p_frame) { l/=p_frame; r/=p_frame; }

	_FORCE_INLINE_ Frame2(Frame p_l, Frame p_r) {l=p_l; r=p_r;}
	_FORCE_INLINE_ Frame2(Frame p_frame) {l=p_frame; r=p_frame;}
	_FORCE_INLINE_ Frame2() {}

} _FORCE_ALIGN_;

typedef Frame2 StereoFrame;

enum FrameType {

	FRAME_MONO,
	FRAME_STEREO
};




#endif // FRAME_H
