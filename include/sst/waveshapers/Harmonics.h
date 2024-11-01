#ifndef SST_WAVESHAPERS_HARMONICS_H
#define SST_WAVESHAPERS_HARMONICS_H

#include "QuadWaveshaper.h"
#include "Saturators.h"
#include "DCBlocker.h"

namespace sst::waveshapers
{
template <SIMD_M128 (*K)(SIMD_M128), bool useDCBlock>
SIMD_M128 CHEBY_CORE(QuadWaveshaperState *__restrict s, SIMD_M128 x, SIMD_M128 drive)
{
    const auto m1 = SIMD_MM(set1_ps)(-1.0f);
    const auto p1 = SIMD_MM(set1_ps)(1.0f);

    auto bound = K(SIMD_MM(max_ps)(SIMD_MM(min_ps)(x, p1), m1));

    if (useDCBlock)
    {
        bound = dcBlock<0, 1>(s, bound);
    }
    return TANH(s, bound, drive);
}

inline SIMD_M128 cheb2_kernel(SIMD_M128 x) // 2 x^2 - 1
{
    const auto m1 = SIMD_MM(set1_ps)(1);
    const auto m2 = SIMD_MM(set1_ps)(2);
    return SIMD_MM(sub_ps)(SIMD_MM(mul_ps)(m2, SIMD_MM(mul_ps)(x, x)), m1);
}

inline SIMD_M128 cheb3_kernel(SIMD_M128 x) // 4 x^3 - 3 x
{
    const auto m4 = SIMD_MM(set1_ps)(4);
    const auto m3 = SIMD_MM(set1_ps)(3);
    auto x2 = SIMD_MM(mul_ps)(x, x);
    auto v4x2m3 = SIMD_MM(sub_ps)(SIMD_MM(mul_ps)(m4, x2), m3);
    return SIMD_MM(mul_ps)(v4x2m3, x);
}

inline SIMD_M128 cheb4_kernel(SIMD_M128 x) // 8 x^4 - 8 x^2 + 1
{
    const auto m1 = SIMD_MM(set1_ps)(1);
    const auto m8 = SIMD_MM(set1_ps)(8);
    auto x2 = SIMD_MM(mul_ps)(x, x);
    auto x4mx2 = SIMD_MM(mul_ps)(x2, SIMD_MM(sub_ps)(x2, m1)); // x^2 * ( x^2 - 1)
    return SIMD_MM(add_ps)(SIMD_MM(mul_ps)(m8, x4mx2), m1);
}

inline SIMD_M128 cheb5_kernel(SIMD_M128 x) // 16 x^5 - 20 x^3 + 5 x
{
    const auto m16 = SIMD_MM(set1_ps)(16);
    const auto mn20 = SIMD_MM(set1_ps)(-20);
    const auto m5 = SIMD_MM(set1_ps)(5);

    auto x2 = SIMD_MM(mul_ps)(x, x);
    auto x3 = SIMD_MM(mul_ps)(x2, x);
    auto x5 = SIMD_MM(mul_ps)(x3, x2);
    auto t1 = SIMD_MM(mul_ps)(m16, x5);
    auto t2 = SIMD_MM(mul_ps)(mn20, x3);
    auto t3 = SIMD_MM(mul_ps)(m5, x);
    return SIMD_MM(add_ps)(t1, SIMD_MM(add_ps)(t2, t3));
}

// Implement sum_n w_i T_i
template <int len> struct ChebSeries
{
    constexpr ChebSeries(const std::initializer_list<float> &f)
    {
        auto q = f.begin();
        auto idx = 0;
        for (int i = 0; i < len; ++i)
            weights[i] = 0.f; // SIMD_MM(setzero_ps)();
        while (idx < len && q != f.end())
        {
            weights[idx] = *q;
            ++idx;
            ++q;
        }
    }
    float weights[len]{};
    SIMD_M128 eval(SIMD_M128 x) const // x bound on 0,1
    {
        const auto p2 = SIMD_MM(set1_ps)(2.0);
        auto Tm1 = SIMD_MM(set1_ps)(1.0);
        auto Tn = x;
        auto res = SIMD_MM(add_ps)(SIMD_MM(set1_ps)(weights[0]),
                                   SIMD_MM(mul_ps)(SIMD_MM(set1_ps)(weights[1]), x));
        for (int idx = 2; idx < len; ++idx)
        {
            auto nxt = SIMD_MM(sub_ps)(SIMD_MM(mul_ps)(SIMD_MM(mul_ps)(p2, Tn), x), Tm1);
            Tm1 = Tn;
            Tn = nxt;
            res = SIMD_MM(add_ps)(res, SIMD_MM(mul_ps)(SIMD_MM(set1_ps)(weights[idx]), Tn));
        }
        return res;
    }
};

inline SIMD_M128 Plus12(QuadWaveshaperState *__restrict s, SIMD_M128 in, SIMD_M128 drive)
{
    static constexpr auto ser = ChebSeries<3>({0, 0.5, 0.5});
    const auto scale = SIMD_MM(set1_ps)(0.66);
    return dcBlock<0, 1>(s, ser.eval(TANH(s, SIMD_MM(mul_ps)(in, scale), drive)));
}

inline SIMD_M128 Plus13(QuadWaveshaperState *__restrict s, SIMD_M128 in, SIMD_M128 drive)
{
    static constexpr auto ser = ChebSeries<4>({0, 0.5, 0, 0.5});
    const auto scale = SIMD_MM(set1_ps)(0.66);
    return ser.eval(TANH(s, SIMD_MM(mul_ps)(in, scale), drive));
}

inline SIMD_M128 Plus14(QuadWaveshaperState *__restrict s, SIMD_M128 in, SIMD_M128 drive)
{
    static constexpr auto ser = ChebSeries<5>({0, 0.5, 0, 0, 0.5});
    const auto scale = SIMD_MM(set1_ps)(0.66);
    return dcBlock<0, 1>(s, ser.eval(TANH(s, SIMD_MM(mul_ps)(in, scale), drive)));
}

inline SIMD_M128 Plus15(QuadWaveshaperState *__restrict s, SIMD_M128 in, SIMD_M128 drive)
{
    static constexpr auto ser = ChebSeries<6>({0, 0.5, 0, 0, 0, 0.5});
    const auto scale = SIMD_MM(set1_ps)(0.66);
    return ser.eval(TANH(s, SIMD_MM(mul_ps)(in, scale), drive));
}

inline SIMD_M128 Plus12345(QuadWaveshaperState *__restrict s, SIMD_M128 in, SIMD_M128 drive)
{
    static constexpr auto ser = ChebSeries<6>({0, 0.2, 0.2, 0.2, 0.2, 0.2});
    const auto scale = SIMD_MM(set1_ps)(0.66);
    return dcBlock<0, 1>(s, ser.eval(TANH(s, SIMD_MM(mul_ps)(in, scale), drive)));
}

inline SIMD_M128 PlusSaw3(QuadWaveshaperState *__restrict s, SIMD_M128 in, SIMD_M128 drive)
{
    static constexpr float fac = 0.9f / (1.f + 0.5 + 0.25);
    static constexpr auto ser = ChebSeries<4>({0, -fac, fac * 0.5f, -fac * 0.25f});
    const auto scale = SIMD_MM(set1_ps)(-0.66); // flip direction
    return dcBlock<0, 1>(s, ser.eval(TANH(s, SIMD_MM(mul_ps)(in, scale), drive)));
}

inline SIMD_M128 PlusSqr3(QuadWaveshaperState *__restrict s, SIMD_M128 in, SIMD_M128 drive)
{
    static constexpr float fac = 0.9f / (1.f - 0.25 + 1.0 / 16.0);
    static constexpr auto ser = ChebSeries<6>({0, fac, 0, -fac * 0.25f, 0, fac / 16.f});
    const auto scale = SIMD_MM(set1_ps)(0.66);
    return ser.eval(TANH(s, SIMD_MM(mul_ps)(in, scale), drive));
}

} // namespace sst::waveshapers

#endif // SST_WAVESHAPERS_HARMONICS_H
