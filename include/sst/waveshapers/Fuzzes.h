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
#ifndef WS_INCLUDE_SST_WAVESHAPERS_FUZZES_H
#define WS_INCLUDE_SST_WAVESHAPERS_FUZZES_H

#include <random>

#include "QuadWaveshaper.h"
#include "WaveshaperLUT.h"

namespace sst::waveshapers
{

// What the heck is this? Well this is the macOS and Linux definitision
// of std::minstd_rand, and we de-typedef it here to make the fuzzes the
// same on all platforms and compiler choices, since the spec gives
// leeway as to what to choose
using portable_minstd_rand = std::linear_congruential_engine<uint_fast32_t, 48271, 0, 2147483647>;

template <int scale> float FuzzTable(const float x)
{
    static auto gen = portable_minstd_rand(2112);
    const float range = 0.1 * scale;
    static auto dist = std::uniform_real_distribution<float>(-range, range);

    auto xadj = x * (1 - range) + dist(gen);
    return xadj;
}

template <int scale, SIMD_M128 C(QuadWaveshaperState *__restrict, SIMD_M128, SIMD_M128)>
SIMD_M128 Fuzz(QuadWaveshaperState *__restrict s, SIMD_M128 x, SIMD_M128 drive)
{
    static LUTBase<1024, FuzzTable<scale>> table;
    return dcBlock<0, 1>(s, WS_PM1_LUT<1024>(table.data, C(s, x, drive)));
}

inline float FuzzCtrTable(const float x)
{
    static auto gen = portable_minstd_rand(2112);
    const float b = 20;

    static auto dist = std::uniform_real_distribution<float>(-1.0, 1.0);

    auto g = exp(-x * x * b);
    auto xadj = x + g * dist(gen);
    return xadj;
}

inline float FuzzEdgeTable(const float x)
{
    static auto gen = portable_minstd_rand(2112);
    static auto dist = std::uniform_real_distribution<float>(-1.0, 1.0);

    auto g = x * x * x * x;
    auto xadj = 0.85 * x + 0.15 * g * dist(gen);
    return xadj;
}
} // namespace sst::waveshapers

#endif // SST_WAVESHAPERS_FUZZES_H
