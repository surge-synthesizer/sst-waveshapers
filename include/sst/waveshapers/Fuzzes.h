#ifndef SST_WAVESHAPERS_FUZZES_H
#define SST_WAVESHAPERS_FUZZES_H

#include <random>

#include "QuadWaveshaper.h"
#include "WaveshaperLUT.h"

namespace sst::waveshapers
{
template <int scale> float FuzzTable(const float x)
{
    static auto gen = std::minstd_rand(2112);
    static const float range = 0.1 * scale;
    static auto dist = std::uniform_real_distribution<float>(-range, range);

    auto xadj = x * (1 - range) + dist(gen);
    return xadj;
}

template <int scale, __m128 C(QuadWaveshaperState *__restrict, __m128, __m128)>
__m128 Fuzz(QuadWaveshaperState *__restrict s, __m128 x, __m128 drive)
{
    static LUTBase<1024, FuzzTable<scale>> table;
    return dcBlock<0, 1>(s, WS_PM1_LUT<1024>(table.data, C(s, x, drive)));
}

inline float FuzzCtrTable(const float x)
{
    static auto gen = std::minstd_rand(2112);
    static const float b = 20;

    static auto dist = std::uniform_real_distribution<float>(-1.0, 1.0);

    auto g = exp(-x * x * b);
    auto xadj = x + g * dist(gen);
    return xadj;
}

inline float FuzzEdgeTable(const float x)
{
    static auto gen = std::minstd_rand(2112);
    static auto dist = std::uniform_real_distribution<float>(-1.0, 1.0);

    auto g = x * x * x * x;
    auto xadj = 0.85 * x + 0.15 * g * dist(gen);
    return xadj;
}
} // namespace sst::waveshapers

#endif // SST_WAVESHAPERS_FUZZES_H
