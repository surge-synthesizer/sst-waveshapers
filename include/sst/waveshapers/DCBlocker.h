#ifndef SST_WAVESHAPERS_DCBLOCKER_H
#define SST_WAVESHAPERS_DCBLOCKER_H

#include "QuadWaveshaper.h"

namespace sst::waveshapers
{
template <int R1, int R2> inline SIMD_M128 dcBlock(QuadWaveshaperState *__restrict s, SIMD_M128 x)
{
    // https://www.dsprelated.com/freebooks/filters/DC_Blocker.html
    // y_n = x_n - x_n-1 + R y_n-1
    const auto fac = SIMD_MM(set1_ps)(0.9999);
    auto dx = SIMD_MM(sub_ps)(x, s->R[R1]);
    auto filtval = SIMD_MM(add_ps)(dx, SIMD_MM(mul_ps)(fac, s->R[R2]));
    s->R[R1] = x;
    s->R[R2] = filtval;
    s->init = SIMD_MM(setzero_ps)();
    return filtval;
}
} // namespace sst::waveshapers

#endif // SST_WAVESHAPERS_DCBLOCKER_H
