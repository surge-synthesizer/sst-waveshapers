#ifndef SST_WAVESHAPERS_SATURATORS_H
#define SST_WAVESHAPERS_SATURATORS_H

#include "QuadWaveshaper.h"
#include "WaveshaperTables.h"
#include "WaveshaperLUT.h"

namespace sst::waveshapers
{
inline __m128 CLIP(QuadWaveshaperState *__restrict, __m128 in, __m128 drive)
{
    const __m128 x_min = _mm_set1_ps(-1.0f);
    const __m128 x_max = _mm_set1_ps(1.0f);
    return _mm_max_ps(_mm_min_ps(_mm_mul_ps(in, drive), x_max), x_min);
}

inline __m128 TANH(QuadWaveshaperState *__restrict, __m128 in, __m128 drive)
{
    // Closer to ideal than TANH0
    // y = x * ( 27 + x * x ) / ( 27 + 9 * x * x );
    // y = clip(y)
    const __m128 m9 = _mm_set1_ps(9.f);
    const __m128 m27 = _mm_set1_ps(27.f);

    __m128 x = _mm_mul_ps(in, drive);
    __m128 xx = _mm_mul_ps(x, x);
    __m128 denom = _mm_add_ps(m27, _mm_mul_ps(m9, xx));
    __m128 y = _mm_mul_ps(x, _mm_add_ps(m27, xx));
    y = _mm_mul_ps(y, _mm_rcp_ps(denom));

    const __m128 y_min = _mm_set1_ps(-1.0f);
    const __m128 y_max = _mm_set1_ps(1.0f);
    return _mm_max_ps(_mm_min_ps(y, y_max), y_min);
}

inline __m128 ZAMSAT(QuadWaveshaperState *__restrict s, __m128 x, __m128 drive)
{
    x = CLIP(s, x, drive);

    // 2x * ( 1 - |x| )
    static const auto p1 = _mm_set1_ps(1.f);
    static const auto p2 = _mm_set1_ps(2.f);

    auto gz = _mm_cmpge_ps(x, _mm_setzero_ps());
    auto sgn = _mm_sub_ps(_mm_and_ps(gz, p1), _mm_andnot_ps(gz, p1));

    auto twox = _mm_mul_ps(p2, x);
    auto F = _mm_sub_ps(twox, _mm_mul_ps(sgn, _mm_mul_ps(x, x)));
    return F;
}

inline __m128 ASYM_SSE2(QuadWaveshaperState *__restrict s, __m128 in, __m128 drive)
{
    return WS_LUT<32, 512, 0x3ff>(
        s, globalWaveshaperTables.waveshapers[static_cast<int>(WaveshaperType::wst_asym)], in,
        drive);
}

inline __m128 OJD(QuadWaveshaperState *__restrict s, __m128 x, __m128 drive)
{
    x = _mm_mul_ps(x, drive);

    static const auto pm17 = _mm_set1_ps(-1.7f);
    static const auto p11 = _mm_set1_ps(1.1f);
    static const auto pm03 = _mm_set1_ps(-0.3f);
    static const auto p09 = _mm_set1_ps(0.9f);

    static const auto denLow = _mm_set1_ps(1.f / (4 * (1 - 0.3f)));
    static const auto denHigh = _mm_set1_ps(1.f / (4 * (1 - 0.9f)));

    auto maskNeg = _mm_cmple_ps(x, pm17);                         // in <= -1.7f
    auto maskPos = _mm_cmpge_ps(x, p11);                          // in > 1.1f
    auto maskLow = _mm_andnot_ps(maskNeg, _mm_cmplt_ps(x, pm03)); // in > -1.7 && in < =0.3
    auto maskHigh = _mm_andnot_ps(maskPos, _mm_cmpgt_ps(x, p09)); // in > 0.9 && in < 1.1
    auto maskMid = _mm_and_ps(_mm_cmpge_ps(x, pm03), _mm_cmple_ps(x, p09)); // the middle

    static const auto vNeg = _mm_set1_ps(-1.0);
    static const auto vPos = _mm_set1_ps(1.0);
    auto vMid = x;

    auto xlow = _mm_sub_ps(x, pm03);
    auto vLow = _mm_add_ps(xlow, _mm_mul_ps(denLow, _mm_mul_ps(xlow, xlow)));
    vLow = _mm_add_ps(vLow, pm03);

    auto xhi = _mm_sub_ps(x, p09);
    auto vHi = _mm_sub_ps(xhi, _mm_mul_ps(denHigh, _mm_mul_ps(xhi, xhi)));
    vHi = _mm_add_ps(vHi, p09);

    auto res =
        _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_and_ps(maskNeg, vNeg), _mm_and_ps(maskLow, vLow)),
                              _mm_add_ps(_mm_and_ps(maskHigh, vHi), _mm_and_ps(maskPos, vPos))),
                   _mm_and_ps(maskMid, vMid));

    return res;
}
} // namespace sst::waveshapers

#endif // SST_WAVESHAPERS_SATURATORS_H
