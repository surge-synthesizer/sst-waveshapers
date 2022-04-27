#ifndef SST_WAVESHAPERS_QUADWAVESHAPER_IMPL_H
#define SST_WAVESHAPERS_QUADWAVESHAPER_IMPL_H

#include "QuadWaveshaper.h"
#include "WaveshaperConfiguration.h"

#include "Saturators.h"
#include "Effects.h"
#include "Harmonics.h"
#include "Rectifiers.h"
#include "Wavefolders.h"
#include "Fuzzes.h"
#include "Trigonometrics.h"

namespace sst::waveshapers
{
inline QuadWaveshaperPtr GetQuadWaveshaper(WaveshaperType type)
{
    switch (type)
    {
    // saturators
    case WaveshaperType::wst_soft:
        return TANH;
    case WaveshaperType::wst_zamsat: // medium
        return ZAMSAT;
    case WaveshaperType::wst_hard:
        return CLIP;
    case WaveshaperType::wst_asym:
        return ASYM_SSE2;
    case WaveshaperType::wst_ojd:
        return OJD;

    // effects
    case WaveshaperType::wst_sine:
        return SINUS_SSE2;
    case WaveshaperType::wst_digital:
        return DIGI_SSE2;

    // harmonic
    case WaveshaperType::wst_cheby2: // soft harmonic 2
        return CHEBY_CORE<cheb2_kernel, true>;
    case WaveshaperType::wst_cheby3: // soft harmonic 3
        return CHEBY_CORE<cheb3_kernel, true>;
    case WaveshaperType::wst_cheby4: // soft harmonic 4
        return CHEBY_CORE<cheb4_kernel, true>;
    case WaveshaperType::wst_cheby5: // soft harmonic 5
        return CHEBY_CORE<cheb5_kernel, true>;
    case WaveshaperType::wst_add12:
        return Plus12;
    case WaveshaperType::wst_add13:
        return Plus13;
    case WaveshaperType::wst_add14:
        return Plus14;
    case WaveshaperType::wst_add15:
        return Plus15;
    case WaveshaperType::wst_add12345:
        return Plus12345;
    case WaveshaperType::wst_addsaw3:
        return PlusSaw3;
    case WaveshaperType::wst_addsqr3:
        return PlusSqr3;

    // rectifier
    case WaveshaperType::wst_fwrectify:
        return ADAA_FULL_WAVE;
    case WaveshaperType::wst_softrect:
        return ADAA_SOFTRECT_WAVE;
    case WaveshaperType::wst_poswav:
        return ADAA_POS_WAVE<0, 1>;
    case WaveshaperType::wst_negwav:
        return ADAA_NEG_WAVE<0, 1>;

    // wavefolder
    case WaveshaperType::wst_softfold:
        return SoftOneFold;
    case WaveshaperType::wst_singlefold:
        return WAVEFOLDER<singleFoldADAA>;
    case WaveshaperType::wst_dualfold:
        return WAVEFOLDER<dualFoldADAA>;
    case WaveshaperType::wst_westfold:
        return WAVEFOLDER<westCoastFoldADAA>;

    // fuzzes
    case WaveshaperType::wst_fuzz:
        return TableEval<FuzzTable<1>, 1024, CLIP>;
    case WaveshaperType::wst_fuzzheavy:
        return TableEval<FuzzTable<3>, 1024, CLIP>;
    case WaveshaperType::wst_fuzzctr:
        return TableEval<FuzzCtrTable, 2048, TANH>;
    case WaveshaperType::wst_fuzzsoft:
        return TableEval<FuzzTable<1>, 1024, TANH>;
    case WaveshaperType::wst_fuzzsoftedge:
        return TableEval<FuzzEdgeTable, 2048, TANH>;

    // trigonometric
    case WaveshaperType::wst_sinpx:
        return TableEval<SinPlusX, 1024, CLIP, false>;
    case WaveshaperType::wst_sin2xpb:
        return TableEval<SinNXPlusXBound<2>, 2048, CLIP, false>;
    case WaveshaperType::wst_sin3xpb:
        return TableEval<SinNXPlusXBound<3>, 2048, CLIP, false>;
    case WaveshaperType::wst_sin7xpb:
        return TableEval<SinNXPlusXBound<7>, 2048, CLIP, false>;
    case WaveshaperType::wst_sin10xpb:
        return TableEval<SinNXPlusXBound<10>, 2048, CLIP, false>;
    case WaveshaperType::wst_2cyc:
        return TableEval<SinNX<2>, 2048, CLIP, false>;
    case WaveshaperType::wst_7cyc:
        return TableEval<SinNX<7>, 2048, CLIP, false>;
    case WaveshaperType::wst_10cyc:
        return TableEval<SinNX<10>, 2048, CLIP, false>;
    case WaveshaperType::wst_2cycbound:
        return TableEval<SinNXBound<2>, 2048, CLIP, false>;
    case WaveshaperType::wst_7cycbound:
        return TableEval<SinNXBound<7>, 2048, CLIP, false>;
    case WaveshaperType::wst_10cycbound:
        return TableEval<SinNXBound<10>, 2048, CLIP, false>;

    // no waveshaper type
    case WaveshaperType::wst_none:
    case WaveshaperType::n_ws_types:
        break;
    }

    return nullptr;
}

inline void initializeWaveshaperRegister(WaveshaperType type, float R[n_waveshaper_registers])
{
    switch (type)
    {
    default:
    {
        for (int i = 0; i < n_waveshaper_registers; ++i)
            R[i] = 0.f;
    }
    break;
    }
    return;
}
} // namespace sst::waveshapers

#endif // SST_WAVESHAPERS_QUADWAVESHAPER_IMPL_H
