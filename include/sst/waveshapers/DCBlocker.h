/*
 * sst-waveshaper - an open source library of waveshaper algorithms
 * by the Surge Synth Team
 *
 * Copyright 2018-2025, various authors, as described in the GitHub
 * transaction log.
 *
 * sst-jucegui is released under the GNU General Public License 3 or later
 * as found in LICENSE.md in this repository.
 *
 * All source in sst-waveshapers available at
 * https://github.com/surge-synthesizer/sst-waveshapers
 */
#ifndef SST_WAVESHAPER_SST_WAVESHAPERS_DCBLOCKER_H
#define SST_WAVESHAPER_SST_WAVESHAPERS_DCBLOCKER_H

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
