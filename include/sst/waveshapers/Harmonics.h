#ifndef SST_WAVESHAPERS_HARMONICS_H
#define SST_WAVESHAPERS_HARMONICS_H

#include "QuadWaveshaper.h"
#include "Saturators.h"
#include "DCBlocker.h"

namespace sst::waveshapers
{
template <__m128 (*K)(__m128), bool useDCBlock>
__m128 CHEBY_CORE(QuadWaveshaperState *__restrict s, __m128 x, __m128 drive)
{
    static const auto m1 = _mm_set1_ps(-1.0f);
    static const auto p1 = _mm_set1_ps(1.0f);

    auto bound = K(_mm_max_ps(_mm_min_ps(x, p1), m1));

    if (useDCBlock)
    {
        bound = dcBlock<0, 1>(s, bound);
    }
    return TANH(s, bound, drive);
}

inline __m128 cheb2_kernel(__m128 x) // 2 x^2 - 1
{
    static const auto m1 = _mm_set1_ps(1);
    static const auto m2 = _mm_set1_ps(2);
    return _mm_sub_ps(_mm_mul_ps(m2, _mm_mul_ps(x, x)), m1);
}

inline __m128 cheb3_kernel(__m128 x) // 4 x^3 - 3 x
{
    static const auto m4 = _mm_set1_ps(4);
    static const auto m3 = _mm_set1_ps(3);
    auto x2 = _mm_mul_ps(x, x);
    auto v4x2m3 = _mm_sub_ps(_mm_mul_ps(m4, x2), m3);
    return _mm_mul_ps(v4x2m3, x);
}

inline __m128 cheb4_kernel(__m128 x) // 8 x^4 - 8 x^2 + 1
{
    static const auto m1 = _mm_set1_ps(1);
    static const auto m8 = _mm_set1_ps(8);
    auto x2 = _mm_mul_ps(x, x);
    auto x4mx2 = _mm_mul_ps(x2, _mm_sub_ps(x2, m1)); // x^2 * ( x^2 - 1)
    return _mm_add_ps(_mm_mul_ps(m8, x4mx2), m1);
}

inline __m128 cheb5_kernel(__m128 x) // 16 x^5 - 20 x^3 + 5 x
{
    static const auto m16 = _mm_set1_ps(16);
    static const auto mn20 = _mm_set1_ps(-20);
    static const auto m5 = _mm_set1_ps(5);

    auto x2 = _mm_mul_ps(x, x);
    auto x3 = _mm_mul_ps(x2, x);
    auto x5 = _mm_mul_ps(x3, x2);
    auto t1 = _mm_mul_ps(m16, x5);
    auto t2 = _mm_mul_ps(mn20, x3);
    auto t3 = _mm_mul_ps(m5, x);
    return _mm_add_ps(t1, _mm_add_ps(t2, t3));
}

// Implement sum_n w_i T_i
template <int len> struct ChebSeries
{
    ChebSeries(const std::initializer_list<float> &f)
    {
        auto q = f.begin();
        auto idx = 0;
        for (int i = 0; i < len; ++i)
            weights[i] = _mm_setzero_ps();
        while (idx < len && q != f.end())
        {
            weights[idx] = _mm_set1_ps(*q);
            ++idx;
            ++q;
        }
    }
    __m128 weights[len];
    __m128 eval(__m128 x) const // x bound on 0,1
    {
        static const auto p2 = _mm_set1_ps(2.0);
        auto Tm1 = _mm_set1_ps(1.0);
        auto Tn = x;
        auto res = _mm_add_ps(weights[0], _mm_mul_ps(weights[1], x));
        for (int idx = 2; idx < len; ++idx)
        {
            auto nxt = _mm_sub_ps(_mm_mul_ps(_mm_mul_ps(p2, Tn), x), Tm1);
            Tm1 = Tn;
            Tn = nxt;
            res = _mm_add_ps(res, _mm_mul_ps(weights[idx], Tn));
        }
        return res;
    }
};

inline __m128 Plus12(QuadWaveshaperState *__restrict s, __m128 in, __m128 drive)
{
    static const auto ser = ChebSeries<3>({0, 0.5, 0.5});
    static const auto scale = _mm_set1_ps(0.66);
    return dcBlock<0, 1>(s, ser.eval(TANH(s, _mm_mul_ps(in, scale), drive)));
}

inline __m128 Plus13(QuadWaveshaperState *__restrict s, __m128 in, __m128 drive)
{
    static const auto ser = ChebSeries<4>({0, 0.5, 0, 0.5});
    static const auto scale = _mm_set1_ps(0.66);
    return ser.eval(TANH(s, _mm_mul_ps(in, scale), drive));
}

inline __m128 Plus14(QuadWaveshaperState *__restrict s, __m128 in, __m128 drive)
{
    static const auto ser = ChebSeries<5>({0, 0.5, 0, 0, 0.5});
    static const auto scale = _mm_set1_ps(0.66);
    return dcBlock<0, 1>(s, ser.eval(TANH(s, _mm_mul_ps(in, scale), drive)));
}

inline __m128 Plus15(QuadWaveshaperState *__restrict s, __m128 in, __m128 drive)
{
    static const auto ser = ChebSeries<6>({0, 0.5, 0, 0, 0, 0.5});
    static const auto scale = _mm_set1_ps(0.66);
    return ser.eval(TANH(s, _mm_mul_ps(in, scale), drive));
}

inline __m128 Plus12345(QuadWaveshaperState *__restrict s, __m128 in, __m128 drive)
{
    static const auto ser = ChebSeries<6>({0, 0.2, 0.2, 0.2, 0.2, 0.2});
    static const auto scale = _mm_set1_ps(0.66);
    return dcBlock<0, 1>(s, ser.eval(TANH(s, _mm_mul_ps(in, scale), drive)));
}

inline __m128 PlusSaw3(QuadWaveshaperState *__restrict s, __m128 in, __m128 drive)
{
    static const float fac = 0.9f / (1.f + 0.5 + 0.25);
    static const auto ser = ChebSeries<4>({0, -fac, fac * 0.5f, -fac * 0.25f});
    static const auto scale = _mm_set1_ps(-0.66); // flip direction
    return dcBlock<0, 1>(s, ser.eval(TANH(s, _mm_mul_ps(in, scale), drive)));
}

inline __m128 PlusSqr3(QuadWaveshaperState *__restrict s, __m128 in, __m128 drive)
{
    static const float fac = 0.9f / (1.f - 0.25 + 1.0 / 16.0);
    static const auto ser = ChebSeries<6>({0, fac, 0, -fac * 0.25f, 0, fac / 16.f});
    static const auto scale = _mm_set1_ps(0.66);
    return ser.eval(TANH(s, _mm_mul_ps(in, scale), drive));
}

} // namespace sst::waveshapers

#endif // SST_WAVESHAPERS_HARMONICS_H
