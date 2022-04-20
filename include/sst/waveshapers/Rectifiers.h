#ifndef SST_WAVESHAPERS_RECTIFIERS_H
#define SST_WAVESHAPERS_RECTIFIERS_H

#include "QuadWaveshaper.h"
#include "Saturators.h"
#include "ADAA.h"
#include "DCBlocker.h"

namespace sst::waveshapers
{
inline void posrect_kernel(__m128 x, __m128 &f, __m128 &adF)
{
    /*
     * F   : x > 0 ? x : 0
     * adF : x > 0 ? x^2/2 : 0
     *
     * observe that adF = F^2/2 in all cases
     */
    static const auto p5 = _mm_set1_ps(0.5f);
    auto gz = _mm_cmpge_ps(x, _mm_setzero_ps());
    f = _mm_and_ps(gz, x);
    adF = _mm_mul_ps(p5, _mm_mul_ps(f, f));
}

template <int R1, int R2>
__m128 ADAA_POS_WAVE(QuadWaveshaperState *__restrict s, __m128 x, __m128 drive)
{
    x = CLIP(s, x, drive);
    return ADAA<posrect_kernel, R1, R2>(s, x);
}

inline void negrect_kernel(__m128 x, __m128 &f, __m128 &adF)
{
    /*
     * F   : x < 0 ? x : 0
     * adF : x < 0 ? x^2/2 : 0
     *
     * observe that adF = F^2/2 in all cases
     */
    static const auto p5 = _mm_set1_ps(0.5f);
    auto gz = _mm_cmple_ps(x, _mm_setzero_ps());
    f = _mm_and_ps(gz, x);
    adF = _mm_mul_ps(p5, _mm_mul_ps(f, f));
}

template <int R1, int R2>
__m128 ADAA_NEG_WAVE(QuadWaveshaperState *__restrict s, __m128 x, __m128 drive)
{
    x = CLIP(s, x, drive);
    return ADAA<negrect_kernel, R1, R2>(s, x);
}

inline void fwrect_kernel(__m128 x, __m128 &F, __m128 &adF)
{
    /*
     * F   : x > 0 ? x : -x  = sgn(x) * x
     * adF : x > 0 ? x^2 / 2 : -x^2 / 2 = sgn(x) * x^2 / 2
     */
    static const auto p1 = _mm_set1_ps(1.f);
    static const auto p05 = _mm_set1_ps(0.5f);
    auto gz = _mm_cmpge_ps(x, _mm_setzero_ps());
    auto sgn = _mm_sub_ps(_mm_and_ps(gz, p1), _mm_andnot_ps(gz, p1));

    F = _mm_mul_ps(sgn, x);
    adF = _mm_mul_ps(F, _mm_mul_ps(x, p05)); // sgn * x * x * 0.5
}

inline __m128 ADAA_FULL_WAVE(QuadWaveshaperState *__restrict s, __m128 x, __m128 drive)
{
    x = CLIP(s, x, drive);
    return ADAA<fwrect_kernel, 0, 1>(s, x);
}

inline void softrect_kernel(__m128 x, __m128 &F, __m128 &adF)
{
    /*
     * F   : x > 0 ? 2x-1 : -2x - 1   = 2 * ( sgn(x) * x ) - 1
     * adF : x > 0 ? x^2-x : -x^2 - x = sgn(x) * x^2 - x
     */
    static const auto p2 = _mm_set1_ps(2.f);
    static const auto p1 = _mm_set1_ps(1.f);
    auto gz = _mm_cmpge_ps(x, _mm_setzero_ps());
    auto sgn = _mm_sub_ps(_mm_and_ps(gz, p1), _mm_andnot_ps(gz, p1));

    F = _mm_sub_ps(_mm_mul_ps(p2, _mm_mul_ps(sgn, x)), p1);
    adF = _mm_sub_ps(_mm_mul_ps(sgn, _mm_mul_ps(x, x)), x);
}

inline __m128 ADAA_SOFTRECT_WAVE(QuadWaveshaperState *__restrict s, __m128 x, __m128 drive)
{
    return TANH(s, dcBlock<2, 3>(s, ADAA<softrect_kernel, 0, 1, false>(s, x)), drive);
}
} // namespace sst::waveshapers

#endif // SST_WAVESHAPERS_RECTIFIERS_H
