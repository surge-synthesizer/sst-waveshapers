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
#ifndef WS_INCLUDE_SST_WAVESHAPERS_WAVESHAPERLUT_H
#define WS_INCLUDE_SST_WAVESHAPERS_WAVESHAPERLUT_H

#include "QuadWaveshaper.h"
#include "DCBlocker.h"
#include "Saturators.h"

namespace sst::waveshapers
{
template <int xRes, int xCenter, int size>
SIMD_M128 WS_LUT(QuadWaveshaperState *__restrict, const float *table, SIMD_M128 in, SIMD_M128 drive)
{
    const auto one = SIMD_MM(set1_ps)(1.f);
    const auto m32 = SIMD_MM(set1_ps)(xRes);
    const auto m512 = SIMD_MM(set1_ps)(xCenter);
    const SIMD_M128I UB = SIMD_MM(set1_epi16)(size - 1);

    auto x = SIMD_MM(mul_ps)(in, drive);
    x = SIMD_MM(add_ps)(SIMD_MM(mul_ps)(x, m32), m512);

    SIMD_M128I e = SIMD_MM(cvtps_epi32)(x);
    auto a = SIMD_MM(sub_ps)(x, SIMD_MM(cvtepi32_ps)(e));
    e = SIMD_MM(packs_epi32)(e, e);
    e = SIMD_MM(max_epi16)(SIMD_MM(min_epi16)(e, UB), SIMD_MM(setzero_si128)());

#if MAC
    // this should be very fast on C2D/C1D (and there are no macs with K8's)
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

    auto ws1 = SIMD_MM(load_ss)(&table[e4[0] & size]);
    auto ws2 = SIMD_MM(load_ss)(&table[e4[1] & size]);
    auto ws3 = SIMD_MM(load_ss)(&table[e4[2] & size]);
    auto ws4 = SIMD_MM(load_ss)(&table[e4[3] & size]);
    auto ws = SIMD_MM(movelh_ps)(SIMD_MM(unpacklo_ps)(ws1, ws2), SIMD_MM(unpacklo_ps)(ws3, ws4));
    ws1 = SIMD_MM(load_ss)(&table[(e4[0] + 1) & size]);
    ws2 = SIMD_MM(load_ss)(&table[(e4[1] + 1) & size]);
    ws3 = SIMD_MM(load_ss)(&table[(e4[2] + 1) & size]);
    ws4 = SIMD_MM(load_ss)(&table[(e4[3] + 1) & size]);
    auto wsn = SIMD_MM(movelh_ps)(SIMD_MM(unpacklo_ps)(ws1, ws2), SIMD_MM(unpacklo_ps)(ws3, ws4));

    x = SIMD_MM(add_ps)(SIMD_MM(mul_ps)(SIMD_MM(sub_ps)(one, a), ws), SIMD_MM(mul_ps)(a, wsn));

    return x;
}

// Given a table of size N+1, N a power of 2, representing data between -1 and 1, interp
template <int N> SIMD_M128 WS_PM1_LUT(const float *table, SIMD_M128 in)
{
    const auto one = SIMD_MM(set1_ps)(1.f);
    const auto dx = SIMD_MM(set1_ps)(N / 2.f);
    const auto ctr = SIMD_MM(set1_ps)(N / 2.f);
    const auto UB = SIMD_MM(set1_ps)(N - 1.f);
    const auto zero = SIMD_MM(setzero_ps)();

    auto x = SIMD_MM(add_ps)(SIMD_MM(mul_ps)(in, dx), ctr);
    auto e = SIMD_MM(cvtps_epi32)(SIMD_MM(max_ps)(SIMD_MM(min_ps)(x, UB), zero));
    auto a = SIMD_MM(cvtepi32_ps)(e);
    auto frac = SIMD_MM(sub_ps)(x, a);
    e = SIMD_MM(packs_epi32)(e, e);

    // on PC write to memory & back as XMM -> GPR is slow on K8
    short e4 alignas(16)[8];
    SIMD_MM(store_si128)((SIMD_M128I *)&e4, e);

    auto ws1 = SIMD_MM(load_ss)(&table[e4[0]]);
    auto ws2 = SIMD_MM(load_ss)(&table[e4[1]]);
    auto ws3 = SIMD_MM(load_ss)(&table[e4[2]]);
    auto ws4 = SIMD_MM(load_ss)(&table[e4[3]]);
    auto ws = SIMD_MM(movelh_ps)(SIMD_MM(unpacklo_ps)(ws1, ws2), SIMD_MM(unpacklo_ps)(ws3, ws4));
    ws1 = SIMD_MM(load_ss)(&table[(e4[0] + 1)]);
    ws2 = SIMD_MM(load_ss)(&table[(e4[1] + 1)]);
    ws3 = SIMD_MM(load_ss)(&table[(e4[2] + 1)]);
    ws4 = SIMD_MM(load_ss)(&table[(e4[3] + 1)]);
    auto wsn = SIMD_MM(movelh_ps)(SIMD_MM(unpacklo_ps)(ws1, ws2), SIMD_MM(unpacklo_ps)(ws3, ws4));

    auto res = SIMD_MM(add_ps)(SIMD_MM(mul_ps)(SIMD_MM(sub_ps)(one, frac), ws),
                               SIMD_MM(mul_ps)(frac, wsn));

    return res;
}

template <int NP, float F(const float)> struct LUTBase
{
    static constexpr int N = NP;
    static constexpr float dx = 2.0 / N;
    float data[N + 1];

    LUTBase()
    {
        for (int i = 0; i < N + 1; ++i)
        {
            float x = i * dx - 1.0;
            data[i] = F(x);
        }
    }
};

template <float F(float), int N, SIMD_M128 C(QuadWaveshaperState *__restrict, SIMD_M128, SIMD_M128),
          bool block = true>
SIMD_M128 TableEval(QuadWaveshaperState *__restrict s, SIMD_M128 x, SIMD_M128 drive)
{
    static LUTBase<N, F> table;
    if (block)
        return dcBlock<0, 1>(s, WS_PM1_LUT<N>(table.data, C(s, x, drive)));
    else
        return WS_PM1_LUT<N>(table.data, C(s, x, drive));
}
} // namespace sst::waveshapers

#endif // SST_WAVESHAPERS_WAVESHAPERLUT_H
