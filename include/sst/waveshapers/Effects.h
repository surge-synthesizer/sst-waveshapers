#ifndef SST_WAVESHAPERS_EFFECTS_H
#define SST_WAVESHAPERS_EFFECTS_H

#include "QuadWaveshaper.h"
#include "WaveshaperTables.h"

namespace sst::waveshapers
{
inline __m128 DIGI_SSE2(QuadWaveshaperState *__restrict, __m128 in, __m128 drive)
{
    // v1.2: return (double)((int)((double)(x*p0inv*16.f+1.0)))*p0*0.0625f;
    const __m128 m16 = _mm_set1_ps(16.f);
    const __m128 m16inv = _mm_set1_ps(0.0625f);
    const __m128 mofs = _mm_set1_ps(0.5f);

    __m128 invdrive = _mm_rcp_ps(drive);
    __m128i a = _mm_cvtps_epi32(_mm_add_ps(mofs, _mm_mul_ps(invdrive, _mm_mul_ps(m16, in))));

    return _mm_mul_ps(drive, _mm_mul_ps(m16inv, _mm_sub_ps(_mm_cvtepi32_ps(a), mofs)));
}

inline __m128 SINUS_SSE2(QuadWaveshaperState *__restrict s, __m128 in, __m128 drive)
{
    const __m128 one = _mm_set1_ps(1.f);
    const __m128 m256 = _mm_set1_ps(256.f);
    const __m128 m512 = _mm_set1_ps(512.f);

    __m128 x = _mm_mul_ps(in, drive);
    x = _mm_add_ps(_mm_mul_ps(x, m256), m512);

    __m128i e = _mm_cvtps_epi32(x);
    __m128 a = _mm_sub_ps(x, _mm_cvtepi32_ps(e));
    e = _mm_packs_epi32(e, e);
    const __m128i UB = _mm_set1_epi16(0x3fe);
    e = _mm_max_epi16(_mm_min_epi16(e, UB), _mm_setzero_si128());

#if MAC
    // this should be very fast on C2D/C1D (and there are no macs with K8's)
    // GCC seems to optimize around the XMM -> int transfers so this is needed here
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

    const auto& table = globalWaveshaperTables.waveshapers[static_cast<int> (WaveshaperType::wst_sine)];
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
} // namespace sst::waveshapers

#endif // SST_WAVESHAPERS_EFFECTS_H
