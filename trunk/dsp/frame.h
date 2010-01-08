#ifndef FRAME_H
#define FRAME_H

#include "typedefs.h"
typedef float Frame;

struct Frame2 {

	Frame l,r;


	SIMD_INLINE const Frame& operator[](int idx) const { return idx==0?l:r; }
	SIMD_INLINE Frame& operator[](int idx) { return idx==0?l:r; }
	SIMD_INLINE Frame2 operator+(const Frame2& p_frame2) const { return Frame2(l+p_frame2.l,r+p_frame2.r); }
	SIMD_INLINE Frame2 operator-(const Frame2& p_frame2) const { return Frame2(l-p_frame2.l,r-p_frame2.r); }
	SIMD_INLINE Frame2 operator*(const Frame2& p_frame2) const { return Frame2(l*p_frame2.l,r*p_frame2.r); }
	SIMD_INLINE Frame2 operator/(const Frame2& p_frame2) const { return Frame2(l/p_frame2.l,r/p_frame2.r); }

	SIMD_INLINE void operator+=(const Frame2& p_frame2) { l+=p_frame2.l;r+=p_frame2.r; }
	SIMD_INLINE void operator-=(const Frame2& p_frame2) { l-=p_frame2.l;r-=p_frame2.r; }
	SIMD_INLINE void operator*=(const Frame2& p_frame2) { l*=p_frame2.l;r*=p_frame2.r; }
	SIMD_INLINE void operator/=(const Frame2& p_frame2) { l/=p_frame2.l;r/=p_frame2.r; }

	SIMD_INLINE Frame2 operator+(const Frame& p_frame) const { return Frame2(l+p_frame,r+p_frame); }
	SIMD_INLINE Frame2 operator-(const Frame& p_frame) const { return Frame2(l-p_frame,r-p_frame); }
	SIMD_INLINE Frame2 operator*(const Frame& p_frame) const { return Frame2(l*p_frame,r*p_frame); }
	SIMD_INLINE Frame2 operator/(const Frame& p_frame) const { return Frame2(l/p_frame,r/p_frame); }

	SIMD_INLINE void operator+=(const Frame& p_frame) { l+=p_frame; r+=p_frame; }
	SIMD_INLINE void operator-=(const Frame& p_frame) { l-=p_frame; r-=p_frame; }
	SIMD_INLINE void operator*=(const Frame& p_frame) { l*=p_frame; r*=p_frame; }
	SIMD_INLINE void operator/=(const Frame& p_frame) { l/=p_frame; r/=p_frame; }

	SIMD_INLINE Frame2(Frame p_l, Frame p_r) {l=p_l; r=p_r;}
	SIMD_INLINE Frame2(Frame p_frame) {l=p_frame; r=p_frame;}
	SIMD_INLINE Frame2() {}

} SIMD_ALIGN;

typedef Frame2 StereoFrame;

enum FrameType {

	FRAME_MONO,
	FRAME_STEREO
};




#endif // FRAME_H
