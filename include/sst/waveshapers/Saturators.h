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
#ifndef WS_INCLUDE_SST_WAVESHAPERS_SATURATORS_H
#define WS_INCLUDE_SST_WAVESHAPERS_SATURATORS_H

#include "QuadWaveshaper.h"
#include "WaveshaperTables.h"
#include "WaveshaperLUT.h"

namespace sst::waveshapers
{
inline SIMD_M128 CLIP(QuadWaveshaperState *__restrict, SIMD_M128 in, SIMD_M128 drive)
{
    const auto x_min = SIMD_MM(set1_ps)(-1.0f);
    const auto x_max = SIMD_MM(set1_ps)(1.0f);
    return SIMD_MM(max_ps)(SIMD_MM(min_ps)(SIMD_MM(mul_ps)(in, drive), x_max), x_min);
}

inline SIMD_M128 TANH(QuadWaveshaperState *__restrict, SIMD_M128 in, SIMD_M128 drive)
{
    // Closer to ideal than TANH0
    // y = x * ( 27 + x * x ) / ( 27 + 9 * x * x );
    // y = clip(y)
    const auto m9 = SIMD_MM(set1_ps)(9.f);
    const auto m27 = SIMD_MM(set1_ps)(27.f);

    auto x = SIMD_MM(mul_ps)(in, drive);
    auto xx = SIMD_MM(mul_ps)(x, x);
    auto denom = SIMD_MM(add_ps)(m27, SIMD_MM(mul_ps)(m9, xx));
    auto y = SIMD_MM(mul_ps)(x, SIMD_MM(add_ps)(m27, xx));
    y = SIMD_MM(mul_ps)(y, SIMD_MM(rcp_ps)(denom));

    const auto y_min = SIMD_MM(set1_ps)(-1.0f);
    const auto y_max = SIMD_MM(set1_ps)(1.0f);
    return SIMD_MM(max_ps)(SIMD_MM(min_ps)(y, y_max), y_min);
}

inline SIMD_M128 ZAMSAT(QuadWaveshaperState *__restrict s, SIMD_M128 x, SIMD_M128 drive)
{
    x = CLIP(s, x, drive);

    // 2x * ( 1 - |x| )
    const auto p1 = SIMD_MM(set1_ps)(1.f);
    const auto p2 = SIMD_MM(set1_ps)(2.f);

    auto gz = SIMD_MM(cmpge_ps)(x, SIMD_MM(setzero_ps)());
    auto sgn = SIMD_MM(sub_ps)(SIMD_MM(and_ps)(gz, p1), SIMD_MM(andnot_ps)(gz, p1));

    auto twox = SIMD_MM(mul_ps)(p2, x);
    auto F = SIMD_MM(sub_ps)(twox, SIMD_MM(mul_ps)(sgn, SIMD_MM(mul_ps)(x, x)));
    return F;
}

inline SIMD_M128 ASYM_SSE2(QuadWaveshaperState *__restrict s, SIMD_M128 in, SIMD_M128 drive)
{
    return WS_LUT<32, 512, 0x3ff>(
        s, globalWaveshaperTables.waveshapers[static_cast<int>(WaveshaperType::wst_asym)], in,
        drive);
}

inline SIMD_M128 OJD(QuadWaveshaperState *__restrict s, SIMD_M128 x, SIMD_M128 drive)
{
    x = SIMD_MM(mul_ps)(x, drive);

    const auto pm17 = SIMD_MM(set1_ps)(-1.7f);
    const auto p11 = SIMD_MM(set1_ps)(1.1f);
    const auto pm03 = SIMD_MM(set1_ps)(-0.3f);
    const auto p09 = SIMD_MM(set1_ps)(0.9f);

    const auto denLow = SIMD_MM(set1_ps)(1.f / (4 * (1 - 0.3f)));
    const auto denHigh = SIMD_MM(set1_ps)(1.f / (4 * (1 - 0.9f)));

    auto maskNeg = SIMD_MM(cmple_ps)(x, pm17); // in <= -1.7f
    auto maskPos = SIMD_MM(cmpge_ps)(x, p11);  // in > 1.1f
    auto maskLow =
        SIMD_MM(andnot_ps)(maskNeg, SIMD_MM(cmplt_ps)(x, pm03)); // in > -1.7 && in < =0.3
    auto maskHigh = SIMD_MM(andnot_ps)(maskPos, SIMD_MM(cmpgt_ps)(x, p09)); // in > 0.9 && in < 1.1
    auto maskMid =
        SIMD_MM(and_ps)(SIMD_MM(cmpge_ps)(x, pm03), SIMD_MM(cmple_ps)(x, p09)); // the middle

    const auto vNeg = SIMD_MM(set1_ps)(-1.0);
    const auto vPos = SIMD_MM(set1_ps)(1.0);
    auto vMid = x;

    auto xlow = SIMD_MM(sub_ps)(x, pm03);
    auto vLow = SIMD_MM(add_ps)(xlow, SIMD_MM(mul_ps)(denLow, SIMD_MM(mul_ps)(xlow, xlow)));
    vLow = SIMD_MM(add_ps)(vLow, pm03);

    auto xhi = SIMD_MM(sub_ps)(x, p09);
    auto vHi = SIMD_MM(sub_ps)(xhi, SIMD_MM(mul_ps)(denHigh, SIMD_MM(mul_ps)(xhi, xhi)));
    vHi = SIMD_MM(add_ps)(vHi, p09);

    auto res = SIMD_MM(add_ps)(
        SIMD_MM(add_ps)(
            SIMD_MM(add_ps)(SIMD_MM(and_ps)(maskNeg, vNeg), SIMD_MM(and_ps)(maskLow, vLow)),
            SIMD_MM(add_ps)(SIMD_MM(and_ps)(maskHigh, vHi), SIMD_MM(and_ps)(maskPos, vPos))),
        SIMD_MM(and_ps)(maskMid, vMid));

    return res;
}
} // namespace sst::waveshapers

#endif // SST_WAVESHAPERS_SATURATORS_H
