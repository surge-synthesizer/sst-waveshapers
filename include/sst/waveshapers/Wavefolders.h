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
            xS[i] = _mm_set1_ps(xs[i]);
            yS[i] = _mm_set1_ps(ys[i]);
            mS[i] = _mm_set1_ps(slopes[i]);
            cS[i] = _mm_set1_ps(intercepts[i]);
        }
    }

    inline void evaluate(__m128 x, __m128 &f, __m128 &adf)
    {
        const auto p05 = _mm_set1_ps(0.5f);
        __m128 rangeMask[pts - 1], val[pts - 1], adVal[pts - 1];

        for (int i = 0; i < pts - 1; ++i)
        {
            rangeMask[i] = _mm_and_ps(_mm_cmpge_ps(x, xS[i]), _mm_cmplt_ps(x, xS[i + 1]));
            auto ox = _mm_sub_ps(x, xS[i]);
            val[i] = _mm_add_ps(_mm_mul_ps(mS[i], ox), yS[i]);
            adVal[i] = _mm_add_ps(_mm_mul_ps(_mm_mul_ps(ox, ox), _mm_mul_ps(mS[i], p05)),
                                  _mm_add_ps(_mm_mul_ps(yS[i], x), cS[i]));
#if DEBUG_WITH_PRINT
            if (rangeMask[i][0] != 0)
                std::cout << _D(x[0]) << _D(rangeMask[i][0]) << _D(xS[i][0]) << _D(xS[i + 1][0])
                          << _D(ox[0]) << _D(cS[i][0]) << _D(mS[i][0]) << _D(yS[i][0])
                          << _D(val[i][0]) << _D(adVal[i][0]) << std::endl;
#endif
        }
        auto res = _mm_and_ps(rangeMask[0], val[0]);
        auto adres = _mm_and_ps(rangeMask[0], adVal[0]);
        for (int i = 1; i < pts - 1; ++i)
        {
            res = _mm_add_ps(res, _mm_and_ps(rangeMask[i], val[i]));
            adres = _mm_add_ps(adres, _mm_and_ps(rangeMask[i], adVal[i]));
        }
        f = res;
        adf = adres;
    }
    float xs[pts], ys[pts], dxs[pts], slopes[pts], intercepts[pts];

    __m128 xS[pts], yS[pts], dxS[pts], mS[pts], cS[pts];
};

inline void singleFoldADAA(__m128 x, __m128 &f, __m128 &adf)
{
    static auto folder = FolderADAA<4>({-10, -0.7, 0.7, 10}, {-1, 1, -1, 1});
    folder.evaluate(x, f, adf);
}

inline void dualFoldADAA(__m128 x, __m128 &f, __m128 &adf)
{
    static auto folder =
        FolderADAA<8>({-10, -3, -1, -0.3, 0.3, 1, 3, 10}, {-1, -0.9, 1, -1, 1, -1, 0.9, 1});
    folder.evaluate(x, f, adf);
}

inline void westCoastFoldADAA(__m128 x, __m128 &f, __m128 &adf)
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

template <void F(__m128, __m128 &, __m128 &)>
__m128 WAVEFOLDER(QuadWaveshaperState *__restrict s, __m128 x, __m128 drive)
{
    x = _mm_mul_ps(x, drive);
    return ADAA<F, 0, 1>(s, x);
}

inline __m128 SoftOneFold(QuadWaveshaperState *__restrict, __m128 x, __m128 drive)
{
    // x / (0.4 + 0.7 x^2)
    auto y = _mm_mul_ps(x, drive);
    auto y2 = _mm_mul_ps(y, y);

    const auto p04 = _mm_set1_ps(0.4f);
    const auto p07 = _mm_set1_ps(0.7f);

    auto num = _mm_add_ps(p04, _mm_mul_ps(p07, y2));

    return _mm_mul_ps(y, _mm_rcp_ps(num));
}

inline __m128 LINFOLD_SSE2(QuadWaveshaperState *__restrict s, __m128 in, __m128 drive)
{
    // The following code is heavily derived from Vital's linear fold (GPLv3, compatible with SST)
    // I think there might be some optimizations to be done here, but I haven't messed with it yet
    // Lots of constants, yikes
    const __m128 mfour = _mm_set1_ps(-4.f);
    const __m128 two = _mm_set1_ps(2.f);
    const __m128 one = _mm_set1_ps(1.f);
    const __m128 p75 = _mm_set1_ps(0.75f);
    const __m128 p25 = _mm_set1_ps(0.25f);
    const __m128 zero = _mm_set1_ps(0.f);

    __m128 x = _mm_mul_ps(in, drive);
    // Prescale the input value
    x = _mm_mul_ps(x, p25);
    x = _mm_add_ps(x, p75);

    // Now, perform a modulus by 1
    __m128i e = _mm_cvtps_epi32(x);
    __m128 a = _mm_sub_ps(x, _mm_cvtepi32_ps(e));
    a = _mm_add_ps(a, _mm_and_ps(one, _mm_cmplt_ps(a, zero)));

    // Finally, scale the output value
    a = _mm_mul_ps(a, mfour);
    a = _mm_add_ps(a, two);
    
    // Absolute value
    uint32_t v = 0x7fffffff; // Trick C++ into initializing a float mask that clears the sign bit
    a = _mm_and_ps(a, _mm_set1_ps(*((float*)&v)));

    // Now finish up by shifting down a bit...
    a = _mm_sub_ps(a, one);

    return a;
}

// Sine fold is implemented in SINUS_SSE2 in Effects.h via template args

} // namespace sst::waveshapers

#endif // SST_WAVESHAPERS_WAVEFOLDERS_H
