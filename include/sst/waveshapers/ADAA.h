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
#ifndef WS_INCLUDE_SST_WAVESHAPERS_ADAA_H
#define WS_INCLUDE_SST_WAVESHAPERS_ADAA_H

#include "QuadWaveshaper.h"

namespace sst::waveshapers
{
/*
 * Given a function which is from x -> (F, adF) and two registers, do the ADAA
 * Set updateInit to false if you are going to wrap this in a dcBlocker (which
 * resets init itself) or other init adjusting function.
 */
template <void FandADF(SIMD_M128, SIMD_M128 &, SIMD_M128 &), int xR, int aR, bool updateInit = true>
SIMD_M128 ADAA(QuadWaveshaperState *__restrict s, SIMD_M128 x)
{
    auto xPrior = s->R[xR];
    auto adPrior = s->R[aR];

    SIMD_M128 f, ad;
    FandADF(x, f, ad);

    auto dx = SIMD_MM(sub_ps)(x, xPrior);
    auto dad = SIMD_MM(sub_ps)(ad, adPrior);

    const static auto tolF = 0.0001;
    const static auto tol = SIMD_MM(set1_ps)(tolF), ntol = SIMD_MM(set1_ps)(-tolF);
    auto ltt = SIMD_MM(and_ps)(SIMD_MM(cmplt_ps)(dx, tol),
                               SIMD_MM(cmpgt_ps)(dx, ntol)); // dx < tol && dx > -tol
    ltt = SIMD_MM(or_ps)(ltt, s->init);
    auto dxDiv =
        SIMD_MM(rcp_ps)(SIMD_MM(add_ps)(SIMD_MM(and_ps)(ltt, tol), SIMD_MM(andnot_ps)(ltt, dx)));

    auto fFromAD = SIMD_MM(mul_ps)(dad, dxDiv);
    auto r = SIMD_MM(add_ps)(SIMD_MM(and_ps)(ltt, f), SIMD_MM(andnot_ps)(ltt, fFromAD));

    s->R[xR] = x;
    s->R[aR] = ad;
    if (updateInit)
    {
        s->init = SIMD_MM(setzero_ps)();
    }

    return r;
}
} // namespace sst::waveshapers

#endif // SST_WAVESHAPERS_ADAA_H
