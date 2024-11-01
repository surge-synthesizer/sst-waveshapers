#ifndef SST_WAVESHAPERS_RECTIFIERS_H
#define SST_WAVESHAPERS_RECTIFIERS_H

#include "QuadWaveshaper.h"
#include "Saturators.h"
#include "ADAA.h"
#include "DCBlocker.h"

namespace sst::waveshapers
{
inline void posrect_kernel(SIMD_M128 x, SIMD_M128 &f, SIMD_M128 &adF)
{
    /*
     * F   : x > 0 ? x : 0
     * adF : x > 0 ? x^2/2 : 0
     *
     * observe that adF = F^2/2 in all cases
     */
    const auto p5 = SIMD_MM(set1_ps)(0.5f);
    auto gz = SIMD_MM(cmpge_ps)(x, SIMD_MM(setzero_ps)());
    f = SIMD_MM(and_ps)(gz, x);
    adF = SIMD_MM(mul_ps)(p5, SIMD_MM(mul_ps)(f, f));
}

template <int R1, int R2>
SIMD_M128 ADAA_POS_WAVE(QuadWaveshaperState *__restrict s, SIMD_M128 x, SIMD_M128 drive)
{
    x = CLIP(s, x, drive);
    return ADAA<posrect_kernel, R1, R2>(s, x);
}

inline void negrect_kernel(SIMD_M128 x, SIMD_M128 &f, SIMD_M128 &adF)
{
    /*
     * F   : x < 0 ? x : 0
     * adF : x < 0 ? x^2/2 : 0
     *
     * observe that adF = F^2/2 in all cases
     */
    const auto p5 = SIMD_MM(set1_ps)(0.5f);
    auto gz = SIMD_MM(cmple_ps)(x, SIMD_MM(setzero_ps)());
    f = SIMD_MM(and_ps)(gz, x);
    adF = SIMD_MM(mul_ps)(p5, SIMD_MM(mul_ps)(f, f));
}

template <int R1, int R2>
SIMD_M128 ADAA_NEG_WAVE(QuadWaveshaperState *__restrict s, SIMD_M128 x, SIMD_M128 drive)
{
    x = CLIP(s, x, drive);
    return ADAA<negrect_kernel, R1, R2>(s, x);
}

inline void fwrect_kernel(SIMD_M128 x, SIMD_M128 &F, SIMD_M128 &adF)
{
    /*
     * F   : x > 0 ? x : -x  = sgn(x) * x
     * adF : x > 0 ? x^2 / 2 : -x^2 / 2 = sgn(x) * x^2 / 2
     */
    const auto p1 = SIMD_MM(set1_ps)(1.f);
    const auto p05 = SIMD_MM(set1_ps)(0.5f);
    auto gz = SIMD_MM(cmpge_ps)(x, SIMD_MM(setzero_ps)());
    auto sgn = SIMD_MM(sub_ps)(SIMD_MM(and_ps)(gz, p1), SIMD_MM(andnot_ps)(gz, p1));

    F = SIMD_MM(mul_ps)(sgn, x);
    adF = SIMD_MM(mul_ps)(F, SIMD_MM(mul_ps)(x, p05)); // sgn * x * x * 0.5
}

inline SIMD_M128 ADAA_FULL_WAVE(QuadWaveshaperState *__restrict s, SIMD_M128 x, SIMD_M128 drive)
{
    x = CLIP(s, x, drive);
    return ADAA<fwrect_kernel, 0, 1>(s, x);
}

inline void softrect_kernel(SIMD_M128 x, SIMD_M128 &F, SIMD_M128 &adF)
{
    /*
     * F   : x > 0 ? 2x-1 : -2x - 1   = 2 * ( sgn(x) * x ) - 1
     * adF : x > 0 ? x^2-x : -x^2 - x = sgn(x) * x^2 - x
     */
    const auto p2 = SIMD_MM(set1_ps)(2.f);
    const auto p1 = SIMD_MM(set1_ps)(1.f);
    auto gz = SIMD_MM(cmpge_ps)(x, SIMD_MM(setzero_ps)());
    auto sgn = SIMD_MM(sub_ps)(SIMD_MM(and_ps)(gz, p1), SIMD_MM(andnot_ps)(gz, p1));

    F = SIMD_MM(sub_ps)(SIMD_MM(mul_ps)(p2, SIMD_MM(mul_ps)(sgn, x)), p1);
    adF = SIMD_MM(sub_ps)(SIMD_MM(mul_ps)(sgn, SIMD_MM(mul_ps)(x, x)), x);
}

inline SIMD_M128 ADAA_SOFTRECT_WAVE(QuadWaveshaperState *__restrict s, SIMD_M128 x, SIMD_M128 drive)
{
    return TANH(s, dcBlock<2, 3>(s, ADAA<softrect_kernel, 0, 1, false>(s, x)), drive);
}
} // namespace sst::waveshapers

#endif // SST_WAVESHAPERS_RECTIFIERS_H
