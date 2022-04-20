#ifndef SST_WAVESHAPERS_DCBLOCKER_H
#define SST_WAVESHAPERS_DCBLOCKER_H

#include "QuadWaveshaper.h"

namespace sst::waveshapers
{
template <int R1, int R2> inline __m128 dcBlock(QuadWaveshaperState *__restrict s, __m128 x)
{
    // https://www.dsprelated.com/freebooks/filters/DC_Blocker.html
    // y_n = x_n - x_n-1 + R y_n-1
    const auto fac = _mm_set1_ps(0.9999);
    auto dx = _mm_sub_ps(x, s->R[R1]);
    auto filtval = _mm_add_ps(dx, _mm_mul_ps(fac, s->R[R2]));
    s->R[R1] = x;
    s->R[R2] = filtval;
    s->init = _mm_setzero_ps();
    return filtval;
}
} // namespace sst::waveshapers

#endif // SST_WAVESHAPERS_DCBLOCKER_H
