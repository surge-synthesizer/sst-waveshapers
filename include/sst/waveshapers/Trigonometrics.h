#ifndef SST_WAVESHAPERS_TRIGONOMETRICS_H
#define SST_WAVESHAPERS_TRIGONOMETRICS_H

#include <cmath>

#include "QuadWaveshaper.h"

namespace sst::waveshapers
{
inline float SinPlusX(const float x) { return x - std::sin(x * M_PI); }

template <int T> float SinNXPlusXBound(const float x)
{
    auto z = 1 - fabs(x);
    auto r = z * std::sin(x * M_PI * T);
    return r + x;
}

template <int T> float SinNX(const float x) { return std::sin(x * M_PI * T); }

template <int T> float SinNXBound(const float x)
{
    auto z = 1 - fabs(x);
    auto r = z * std::sin(x * M_PI * T);
    return r;
}
} // namespace sst::waveshapers

#endif // SST_WAVESHAPERS_TRIGONOMETRICS_H
