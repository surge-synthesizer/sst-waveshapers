#ifndef SST_WAVESHAPERS_WAVEFOLDERS_H
#define SST_WAVESHAPERS_WAVEFOLDERS_H

#include "QuadWaveshaper.h"
#include "ADAA.h"

namespace sst::waveshapers
{
template <int pts> struct FolderADAA
{
    FolderADAA(std::initializer_list<float> xi, std::initializer_list<float> yi)
    {
        auto xiv = xi.begin();
        auto yiv = yi.begin();
        for (int i = 0; i < pts; ++i)
        {
            xs[i] = *xiv;
            ys[i] = *yiv;

            xiv++;
            yiv++;
        }

        slopes[pts - 1] = 0;
        dxs[pts - 1] = 0;

        intercepts[0] = -xs[0] * ys[0];
        for (int i = 0; i < pts - 1; ++i)
        {
            dxs[i] = xs[i + 1] - xs[i];
            slopes[i] = (ys[i + 1] - ys[i]) / dxs[i];
            auto vLeft = slopes[i] * dxs[i] * dxs[i] / 2 + ys[i] * xs[i + 1] + intercepts[i];
            auto vRight = ys[i + 1] * xs[i + 1];
            intercepts[i + 1] = -vRight + vLeft;
        }

        for (int i = 0; i < pts; ++i)
        {
            xS[i] = SIMD_MM(set1_ps)(xs[i]);
            yS[i] = SIMD_MM(set1_ps)(ys[i]);
            mS[i] = SIMD_MM(set1_ps)(slopes[i]);
            cS[i] = SIMD_MM(set1_ps)(intercepts[i]);
        }
    }

    inline void evaluate(SIMD_M128 x, SIMD_M128 &f, SIMD_M128 &adf)
    {
        const auto p05 = SIMD_MM(set1_ps)(0.5f);
        SIMD_M128 rangeMask[pts - 1], val[pts - 1], adVal[pts - 1];

        for (int i = 0; i < pts - 1; ++i)
        {
            rangeMask[i] =
                SIMD_MM(and_ps)(SIMD_MM(cmpge_ps)(x, xS[i]), SIMD_MM(cmplt_ps)(x, xS[i + 1]));
            auto ox = SIMD_MM(sub_ps)(x, xS[i]);
            val[i] = SIMD_MM(add_ps)(SIMD_MM(mul_ps)(mS[i], ox), yS[i]);
            adVal[i] = SIMD_MM(add_ps)(
                SIMD_MM(mul_ps)(SIMD_MM(mul_ps)(ox, ox), SIMD_MM(mul_ps)(mS[i], p05)),
                SIMD_MM(add_ps)(SIMD_MM(mul_ps)(yS[i], x), cS[i]));
#if DEBUG_WITH_PRINT
            if (rangeMask[i][0] != 0)
                std::cout << _D(x[0]) << _D(rangeMask[i][0]) << _D(xS[i][0]) << _D(xS[i + 1][0])
                          << _D(ox[0]) << _D(cS[i][0]) << _D(mS[i][0]) << _D(yS[i][0])
                          << _D(val[i][0]) << _D(adVal[i][0]) << std::endl;
#endif
        }
        auto res = SIMD_MM(and_ps)(rangeMask[0], val[0]);
        auto adres = SIMD_MM(and_ps)(rangeMask[0], adVal[0]);
        for (int i = 1; i < pts - 1; ++i)
        {
            res = SIMD_MM(add_ps)(res, SIMD_MM(and_ps)(rangeMask[i], val[i]));
            adres = SIMD_MM(add_ps)(adres, SIMD_MM(and_ps)(rangeMask[i], adVal[i]));
        }
        f = res;
        adf = adres;
    }
    float xs[pts], ys[pts], dxs[pts], slopes[pts], intercepts[pts];

    SIMD_M128 xS[pts], yS[pts], dxS[pts], mS[pts], cS[pts];
};

inline void singleFoldADAA(SIMD_M128 x, SIMD_M128 &f, SIMD_M128 &adf)
{
    static auto folder = FolderADAA<4>({-10, -0.7, 0.7, 10}, {-1, 1, -1, 1});
    folder.evaluate(x, f, adf);
}

inline void dualFoldADAA(SIMD_M128 x, SIMD_M128 &f, SIMD_M128 &adf)
{
    static auto folder =
        FolderADAA<8>({-10, -3, -1, -0.3, 0.3, 1, 3, 10}, {-1, -0.9, 1, -1, 1, -1, 0.9, 1});
    folder.evaluate(x, f, adf);
}

inline void westCoastFoldADAA(SIMD_M128 x, SIMD_M128 &f, SIMD_M128 &adf)
{
    // Factors based on
    // DAFx-17 DAFX-194 Virtual Analog Buchla 259 Wavefolder
    // by Sequeda, Pontynen, Valimaki and Parker
    static auto folder = FolderADAA<14>(
        {-10, -2, -1.0919091909190919, -0.815881588158816, -0.5986598659865987, -0.3598359835983597,
         -0.11981198119811971, 0.11981198119811971, 0.3598359835983597, 0.5986598659865987,
         0.8158815881588157, 1.0919091909190919, 2, 10},
        {1, 0.9, -0.679765619488133, 0.5309659972270625, -0.6255506631744251, 0.5991799179917987,
         -0.5990599059905986, 0.5990599059905986, -0.5991799179917987, 0.6255506631744251,
         -0.5309659972270642, 0.679765619488133, -0.9, -1});
    folder.evaluate(x, f, adf);
}

template <void F(SIMD_M128, SIMD_M128 &, SIMD_M128 &)>
SIMD_M128 WAVEFOLDER(QuadWaveshaperState *__restrict s, SIMD_M128 x, SIMD_M128 drive)
{
    x = SIMD_MM(mul_ps)(x, drive);
    return ADAA<F, 0, 1>(s, x);
}

inline SIMD_M128 SoftOneFold(QuadWaveshaperState *__restrict, SIMD_M128 x, SIMD_M128 drive)
{
    // x / (0.4 + 0.7 x^2)
    auto y = SIMD_MM(mul_ps)(x, drive);
    auto y2 = SIMD_MM(mul_ps)(y, y);

    const auto p04 = SIMD_MM(set1_ps)(0.4f);
    const auto p07 = SIMD_MM(set1_ps)(0.7f);

    auto num = SIMD_MM(add_ps)(p04, SIMD_MM(mul_ps)(p07, y2));

    return SIMD_MM(mul_ps)(y, SIMD_MM(rcp_ps)(num));
}

inline SIMD_M128 LINFOLD_SSE2(QuadWaveshaperState *__restrict s, SIMD_M128 in, SIMD_M128 drive)
{
    // The following code is heavily derived from Vital's linear fold (GPLv3, compatible with SST)
    // I think there might be some optimizations to be done here, but I haven't messed with it yet
    // Lots of constants, yikes
    const auto mfour = SIMD_MM(set1_ps)(-4.f);
    const auto two = SIMD_MM(set1_ps)(2.f);
    const auto one = SIMD_MM(set1_ps)(1.f);
    const auto p75 = SIMD_MM(set1_ps)(0.75f);
    const auto p25 = SIMD_MM(set1_ps)(0.25f);
    const auto zero = SIMD_MM(set1_ps)(0.f);

    auto x = SIMD_MM(mul_ps)(in, drive);
    // Prescale the input value
    x = SIMD_MM(mul_ps)(x, p25);
    x = SIMD_MM(add_ps)(x, p75);

    // Now, perform a modulus by 1
    SIMD_M128I e = SIMD_MM(cvtps_epi32)(x);
    auto a = SIMD_MM(sub_ps)(x, SIMD_MM(cvtepi32_ps)(e));
    a = SIMD_MM(add_ps)(a, SIMD_MM(and_ps)(one, SIMD_MM(cmplt_ps)(a, zero)));

    // Finally, scale the output value
    a = SIMD_MM(mul_ps)(a, mfour);
    a = SIMD_MM(add_ps)(a, two);

    // Absolute value
    uint32_t v = 0x7fffffff; // Trick C++ into initializing a float mask that clears the sign bit
    a = SIMD_MM(and_ps)(a, SIMD_MM(set1_ps)(*((float *)&v)));

    // Now finish up by shifting down a bit...
    a = SIMD_MM(sub_ps)(a, one);

    return a;
}

// Sine fold is implemented in SINUS_SSE2 in Effects.h via template args

} // namespace sst::waveshapers

#endif // SST_WAVESHAPERS_WAVEFOLDERS_H
