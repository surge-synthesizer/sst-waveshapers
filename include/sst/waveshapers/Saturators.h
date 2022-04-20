#ifndef SST_WAVESHAPERS_SATURATORS_H
#define SST_WAVESHAPERS_SATURATORS_H

#include "QuadWaveshaper.h"
#include "WaveshaperTables.h"

namespace sst::waveshapers
{
__m128 CLIP(QuadWaveshaperState *__restrict, __m128 in, __m128 drive)
{
    const __m128 x_min = _mm_set1_ps(-1.0f);
    const __m128 x_max = _mm_set1_ps(1.0f);
    return _mm_max_ps(_mm_min_ps(_mm_mul_ps(in, drive), x_max), x_min);
}

__m128 TANH(QuadWaveshaperState *__restrict, __m128 in, __m128 drive)
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

__m128 ZAMSAT(QuadWaveshaperState *__restrict s, __m128 x, __m128 drive)
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

__m128 ASYM_SSE2(QuadWaveshaperState *__restrict, __m128 in, __m128 drive)
{
    const __m128 one = _mm_set1_ps(1.f);
    const __m128 m32 = _mm_set1_ps(32.f);
    const __m128 m512 = _mm_set1_ps(512.f);
    const __m128i UB = _mm_set1_epi16(0x3fe);

    __m128 x = _mm_mul_ps(in, drive);
    x = _mm_add_ps(_mm_mul_ps(x, m32), m512);

    __m128i e = _mm_cvtps_epi32(x);
    __m128 a = _mm_sub_ps(x, _mm_cvtepi32_ps(e));
    e = _mm_packs_epi32(e, e);
    e = _mm_max_epi16(_mm_min_epi16(e, UB), _mm_setzero_si128());

#if MAC
    // this should be very fast on C2D/C1D (and there are no macs with K8's)
    int e4 alignas(16)[4];
    e4[0] = _mm_cvtsi128_si32(e);
    e4[1] = _mm_cvtsi128_si32(_mm_shufflelo_epi16(e, _MM_SHUFFLE(1, 1, 1, 1)));
    e4[2] = _mm_cvtsi128_si32(_mm_shufflelo_epi16(e, _MM_SHUFFLE(2, 2, 2, 2)));
    e4[3] = _mm_cvtsi128_si32(_mm_shufflelo_epi16(e, _MM_SHUFFLE(3, 3, 3, 3)));

#else
    // on PC write to memory & back as XMM -> GPR is slow on K8
    short e4 alignas(16)[8];
    _mm_store_si128((__m128i *)&e4, e);
#endif

    const auto& table = globalWaveshaperTables.waveshapers[static_cast<int> (WaveshaperType::wst_asym)];
    __m128 ws1 = _mm_load_ss(&table[e4[0] & 0x3ff]);
    __m128 ws2 = _mm_load_ss(&table[e4[1] & 0x3ff]);
    __m128 ws3 = _mm_load_ss(&table[e4[2] & 0x3ff]);
    __m128 ws4 = _mm_load_ss(&table[e4[3] & 0x3ff]);
    __m128 ws = _mm_movelh_ps(_mm_unpacklo_ps(ws1, ws2), _mm_unpacklo_ps(ws3, ws4));
    ws1 = _mm_load_ss(&table[(e4[0] + 1) & 0x3ff]);
    ws2 = _mm_load_ss(&table[(e4[1] + 1) & 0x3ff]);
    ws3 = _mm_load_ss(&table[(e4[2] + 1) & 0x3ff]);
    ws4 = _mm_load_ss(&table[(e4[3] + 1) & 0x3ff]);
    __m128 wsn = _mm_movelh_ps(_mm_unpacklo_ps(ws1, ws2), _mm_unpacklo_ps(ws3, ws4));

    x = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(one, a), ws), _mm_mul_ps(a, wsn));

    return x;
}

__m128 OJD(QuadWaveshaperState *__restrict s, __m128 x, __m128 drive)
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
