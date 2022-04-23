#ifndef SST_WAVESHAPERS_WAVESHAPERCONFIGURATION_H
#define SST_WAVESHAPERS_WAVESHAPERCONFIGURATION_H

namespace sst::waveshapers
{

enum class WaveshaperType
{
    wst_none = 0,
    wst_soft,
    wst_hard,
    wst_asym,
    wst_sine,
    wst_digital,

    // XT waves start here
    wst_cheby2,
    wst_cheby3,
    wst_cheby4,
    wst_cheby5,

    wst_fwrectify,
    wst_poswav,
    wst_negwav,
    wst_softrect,

    wst_singlefold,
    wst_dualfold,
    wst_westfold,

    // additive harmonics
    wst_add12,
    wst_add13,
    wst_add14,
    wst_add15,
    wst_add12345,
    wst_addsaw3,
    wst_addsqr3,

    wst_fuzz,
    wst_fuzzsoft,
    wst_fuzzheavy,
    wst_fuzzctr,
    wst_fuzzsoftedge,

    wst_sinpx,
    wst_sin2xpb,
    wst_sin3xpb,
    wst_sin7xpb,
    wst_sin10xpb,

    wst_2cyc,
    wst_7cyc,
    wst_10cyc,

    wst_2cycbound,
    wst_7cycbound,
    wst_10cycbound,

    wst_zamsat,
    wst_ojd,

    wst_softfold,

    n_ws_types,
};

const char wst_names[(int)WaveshaperType::n_ws_types][32] = {"Off",
                                                             "Soft",
                                                             "Hard",
                                                             "Asymmetric",
                                                             "Sine",
                                                             "Digital",
                                                             "Soft Harmonic 2",
                                                             "Soft Harmonic 3",
                                                             "Soft Harmonic 4",
                                                             "Soft Harmonic 5",
                                                             "Full Wave",
                                                             "Half Wave Positive",
                                                             "Half Wave Negative",
                                                             "Soft Rectifier",
                                                             "Single Fold",
                                                             "Double Fold",
                                                             "West Coast Fold",
                                                             "Additive 1+2",
                                                             "Additive 1+3",
                                                             "Additive 1+4",
                                                             "Additive 1+5",
                                                             "Additive 12345",
                                                             "Additive Saw 3",
                                                             "Additive Square 3",

                                                             "Fuzz",
                                                             "Fuzz Soft Clip",
                                                             "Heavy Fuzz",
                                                             "Fuzz Center",
                                                             "Fuzz Soft Edge",

                                                             "Sin+x",
                                                             "Sin 2x + x",
                                                             "Sin 3x + x",
                                                             "Sin 7x + x",
                                                             "Sin 10x + x",
                                                             "2 Cycle",
                                                             "7 Cycle",
                                                             "10 Cycle",
                                                             "2 Cycle Bound",
                                                             "7 Cycle Bound",
                                                             "10 Cycle Bound",
                                                             "Medium",
                                                             "OJD",
                                                             "Soft Single Fold"};
} // namespace sst::waveshapers

#endif // SST_WAVESHAPERS_WAVESHAPERCONFIGURATION_H
