#ifndef SST_WAVESHAPERS_ADAA_H
#define SST_WAVESHAPERS_ADAA_H

#include "QuadWaveshaper.h"

namespace sst::waveshapers
{
/*
 * Given a function which is from x -> (F, adF) and two registers, do the ADAA
 * Set updateInit to false if you are going to wrap this in a dcBlocker (which
 * resets init itself) or other init adjusting function.
 */
template <void FandADF(__m128, __m128 &, __m128 &), int xR, int aR, bool updateInit = true>
__m128 ADAA(QuadWaveshaperState *__restrict s, __m128 x)
{
    auto xPrior = s->R[xR];
    auto adPrior = s->R[aR];

    __m128 f, ad;
    FandADF(x, f, ad);

    auto dx = _mm_sub_ps(x, xPrior);
    auto dad = _mm_sub_ps(ad, adPrior);

    const static auto tolF = 0.0001;
    const static auto tol = _mm_set1_ps(tolF), ntol = _mm_set1_ps(-tolF);
    auto ltt = _mm_and_ps(_mm_cmplt_ps(dx, tol), _mm_cmpgt_ps(dx, ntol)); // dx < tol && dx > -tol
    ltt = _mm_or_ps(ltt, s->init);
    auto dxDiv = _mm_rcp_ps(_mm_add_ps(_mm_and_ps(ltt, tol), _mm_andnot_ps(ltt, dx)));

    auto fFromAD = _mm_mul_ps(dad, dxDiv);
    auto r = _mm_add_ps(_mm_and_ps(ltt, f), _mm_andnot_ps(ltt, fFromAD));

    s->R[xR] = x;
    s->R[aR] = ad;
    if (updateInit)
    {
        s->init = _mm_setzero_ps();
    }

    return r;
}
} // namespace sst::waveshapers

#endif // SST_WAVESHAPERS_ADAA_H
