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
#ifndef WS_INCLUDE_SST_WAVESHAPERS_TRIGONOMETRICS_H
#define WS_INCLUDE_SST_WAVESHAPERS_TRIGONOMETRICS_H

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
