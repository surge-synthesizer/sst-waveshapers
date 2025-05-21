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
#ifndef WS_INCLUDE_SST_WAVESHAPERS_EFFECTS_H
#define WS_INCLUDE_SST_WAVESHAPERS_EFFECTS_H

#include "QuadWaveshaper.h"
#include "WaveshaperTables.h"

namespace sst::waveshapers
{
inline SIMD_M128 DIGI_SSE2(QuadWaveshaperState *__restrict, SIMD_M128 in, SIMD_M128 drive)
{
    // v1.2: return (double)((int)((double)(x*p0inv*16.f+1.0)))*p0*0.0625f;
    const auto m16 = SIMD_MM(set1_ps)(16.f);
    const auto m16inv = SIMD_MM(set1_ps)(0.0625f);
    const auto mofs = SIMD_MM(set1_ps)(0.5f);

    auto invdrive = SIMD_MM(rcp_ps)(drive);
    SIMD_M128I a = SIMD_MM(cvtps_epi32)(
        SIMD_MM(add_ps)(mofs, SIMD_MM(mul_ps)(invdrive, SIMD_MM(mul_ps)(m16, in))));

    return SIMD_MM(mul_ps)(drive,
                           SIMD_MM(mul_ps)(m16inv, SIMD_MM(sub_ps)(SIMD_MM(cvtepi32_ps)(a), mofs)));
}

template <bool DO_FOLD = false>
inline SIMD_M128 SINUS_SSE2(QuadWaveshaperState *__restrict s, SIMD_M128 in, SIMD_M128 drive)
{
    const auto one = SIMD_MM(set1_ps)(1.f);
    const auto m256 = SIMD_MM(set1_ps)(256.f);
    const auto m512 = SIMD_MM(set1_ps)(512.f);

    // Scale so that -1.0 - 1.0 goes to 256 - 768
    // +6 dB gets you the full sine wave
    auto x = SIMD_MM(mul_ps)(in, drive);
    x = SIMD_MM(add_ps)(SIMD_MM(mul_ps)(x, m256), m512);

    // Convert to 32 bit ints
    SIMD_M128I e = SIMD_MM(cvtps_epi32)(x);
    // Calculate the remainder due to truncation. This is used for later interpolation
    auto a = SIMD_MM(sub_ps)(x, SIMD_MM(cvtepi32_ps)(e));

    // Template arg -- Compiler will optimize this out
    if (DO_FOLD)
    {
        // Now, make sure the fold pattern repeats
        // Fortunately, we're dealing with a power-of-two LUT so we can do a modulus by bitwise and
        // like so:
        e = SIMD_MM(and_si128)(e, SIMD_MM(set1_epi32)(0x3ff));
        // Now pack into 16 bit ints. Should already be truncated
        // If not, whoops, segfault
        e = SIMD_MM(packs_epi32)(e, e);
    }
    else
    {
        // Don't repeat; Instead, clip to zero at the boundaries
        e = SIMD_MM(packs_epi32)(e, e);
        const SIMD_M128I UB = SIMD_MM(set1_epi16)(0x3fe);
        e = SIMD_MM(max_epi16)(SIMD_MM(min_epi16)(e, UB), SIMD_MM(setzero_si128)());
    }

#if MAC
    // this should be very fast on C2D/C1D (and there are no macs with K8's)
    // GCC seems to optimize around the XMM -> int transfers so this is needed here
    int e4 alignas(16)[4];
    e4[0] = SIMD_MM(cvtsi128_si32)(e);
    e4[1] = SIMD_MM(cvtsi128_si32)(SIMD_MM(shufflelo_epi16)(e, SIMD_MM_SHUFFLE(1, 1, 1, 1)));
    e4[2] = SIMD_MM(cvtsi128_si32)(SIMD_MM(shufflelo_epi16)(e, SIMD_MM_SHUFFLE(2, 2, 2, 2)));
    e4[3] = SIMD_MM(cvtsi128_si32)(SIMD_MM(shufflelo_epi16)(e, SIMD_MM_SHUFFLE(3, 3, 3, 3)));
#else
    // on PC write to memory & back as XMM -> GPR is slow on K8
    short e4 alignas(16)[8];
    SIMD_MM(store_si128)((SIMD_M128I *)&e4, e);
#endif

    const auto &table =
        globalWaveshaperTables.waveshapers[static_cast<int>(WaveshaperType::wst_sine)];
    auto ws1 = SIMD_MM(load_ss)(&table[e4[0] & 0x3ff]);
    auto ws2 = SIMD_MM(load_ss)(&table[e4[1] & 0x3ff]);
    auto ws3 = SIMD_MM(load_ss)(&table[e4[2] & 0x3ff]);
    auto ws4 = SIMD_MM(load_ss)(&table[e4[3] & 0x3ff]);
    auto ws = SIMD_MM(movelh_ps)(SIMD_MM(unpacklo_ps)(ws1, ws2), SIMD_MM(unpacklo_ps)(ws3, ws4));
    ws1 = SIMD_MM(load_ss)(&table[(e4[0] + 1) & 0x3ff]);
    ws2 = SIMD_MM(load_ss)(&table[(e4[1] + 1) & 0x3ff]);
    ws3 = SIMD_MM(load_ss)(&table[(e4[2] + 1) & 0x3ff]);
    ws4 = SIMD_MM(load_ss)(&table[(e4[3] + 1) & 0x3ff]);
    auto wsn = SIMD_MM(movelh_ps)(SIMD_MM(unpacklo_ps)(ws1, ws2), SIMD_MM(unpacklo_ps)(ws3, ws4));

    x = SIMD_MM(add_ps)(SIMD_MM(mul_ps)(SIMD_MM(sub_ps)(one, a), ws), SIMD_MM(mul_ps)(a, wsn));

    return x;
}
} // namespace sst::waveshapers

#endif // SST_WAVESHAPERS_EFFECTS_H
