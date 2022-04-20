#ifndef SST_WAVESHAPERS_WAVESHAPERLUT_H
#define SST_WAVESHAPERS_WAVESHAPERLUT_H

#include "QuadWaveshaper.h"

namespace sst::waveshapers
{
template <int xRes, int xCenter, int size>
__m128 WS_LUT(QuadWaveshaperState *__restrict, const float *table, __m128 in, __m128 drive)
{
    const __m128 one = _mm_set1_ps(1.f);
    const __m128 m32 = _mm_set1_ps(xRes);
    const __m128 m512 = _mm_set1_ps(xCenter);
    const __m128i UB = _mm_set1_epi16(size - 1);

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

    __m128 ws1 = _mm_load_ss(&table[e4[0] & size]);
    __m128 ws2 = _mm_load_ss(&table[e4[1] & size]);
    __m128 ws3 = _mm_load_ss(&table[e4[2] & size]);
    __m128 ws4 = _mm_load_ss(&table[e4[3] & size]);
    __m128 ws = _mm_movelh_ps(_mm_unpacklo_ps(ws1, ws2), _mm_unpacklo_ps(ws3, ws4));
    ws1 = _mm_load_ss(&table[(e4[0] + 1) & size]);
    ws2 = _mm_load_ss(&table[(e4[1] + 1) & size]);
    ws3 = _mm_load_ss(&table[(e4[2] + 1) & size]);
    ws4 = _mm_load_ss(&table[(e4[3] + 1) & size]);
    __m128 wsn = _mm_movelh_ps(_mm_unpacklo_ps(ws1, ws2), _mm_unpacklo_ps(ws3, ws4));

    x = _mm_add_ps(_mm_mul_ps(_mm_sub_ps(one, a), ws), _mm_mul_ps(a, wsn));

    return x;
}
}

#endif // SST_WAVESHAPERS_WAVESHAPERLUT_H
