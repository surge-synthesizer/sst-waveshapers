#ifndef SST_WAVESHAPERS_WAVESHAPERCONFIGURATION_H
#define SST_WAVESHAPERS_WAVESHAPERCONFIGURATION_H

#include <vector>
#include <utility>

namespace sst::waveshapers
{

// Since these are streamed in some properties, please only add items to the end
// Grouping and ordering in uis happens via the groupname below not the order here
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

    wst_linearfold,
    wst_sinefold,

    n_ws_types,
};

const char wst_names[(int)WaveshaperType::n_ws_types][32] = {
    "Off",
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
    "Soft Single Fold",
    "Linear Fold",
    "Sine Fold",
};

inline std::vector<std::pair<int, std::string>> WaveshaperGroupName()
{
    static std::vector<std::pair<int, std::string>> mapping;
    if (mapping.empty())
    {
        auto lmap = mapping;
        auto p = [&lmap](sst::waveshapers::WaveshaperType i, const std::string &s) {
            lmap.emplace_back((int)i, s);
        };

        // Obviously improve this
        p(sst::waveshapers::WaveshaperType::wst_none, "");

        p(sst::waveshapers::WaveshaperType::wst_soft, "Saturator");
        p(sst::waveshapers::WaveshaperType::wst_zamsat, "Saturator");
        p(sst::waveshapers::WaveshaperType::wst_hard, "Saturator");
        p(sst::waveshapers::WaveshaperType::wst_asym, "Saturator");
        p(sst::waveshapers::WaveshaperType::wst_ojd, "Saturator");

        p(sst::waveshapers::WaveshaperType::wst_sine, "Effect");
        p(sst::waveshapers::WaveshaperType::wst_digital, "Effect");

        p(sst::waveshapers::WaveshaperType::wst_cheby2, "Harmonic");
        p(sst::waveshapers::WaveshaperType::wst_cheby3, "Harmonic");
        p(sst::waveshapers::WaveshaperType::wst_cheby4, "Harmonic");
        p(sst::waveshapers::WaveshaperType::wst_cheby5, "Harmonic");

        p(sst::waveshapers::WaveshaperType::wst_add12, "Harmonic");
        p(sst::waveshapers::WaveshaperType::wst_add13, "Harmonic");
        p(sst::waveshapers::WaveshaperType::wst_add14, "Harmonic");
        p(sst::waveshapers::WaveshaperType::wst_add15, "Harmonic");
        p(sst::waveshapers::WaveshaperType::wst_add12345, "Harmonic");
        p(sst::waveshapers::WaveshaperType::wst_addsaw3, "Harmonic");
        p(sst::waveshapers::WaveshaperType::wst_addsqr3, "Harmonic");

        p(sst::waveshapers::WaveshaperType::wst_fwrectify, "Rectifiers");
        p(sst::waveshapers::WaveshaperType::wst_poswav, "Rectifiers");
        p(sst::waveshapers::WaveshaperType::wst_negwav, "Rectifiers");
        p(sst::waveshapers::WaveshaperType::wst_softrect, "Rectifiers");

        p(sst::waveshapers::WaveshaperType::wst_softfold, "Wavefolder");
        p(sst::waveshapers::WaveshaperType::wst_singlefold, "Wavefolder");
        p(sst::waveshapers::WaveshaperType::wst_dualfold, "Wavefolder");
        p(sst::waveshapers::WaveshaperType::wst_westfold, "Wavefolder");
        p(sst::waveshapers::WaveshaperType::wst_linearfold, "Wavefolder");
        p(sst::waveshapers::WaveshaperType::wst_sinefold, "Wavefolder");

        p(sst::waveshapers::WaveshaperType::wst_fuzz, "Fuzz");
        p(sst::waveshapers::WaveshaperType::wst_fuzzheavy, "Fuzz");
        p(sst::waveshapers::WaveshaperType::wst_fuzzctr, "Fuzz");
        p(sst::waveshapers::WaveshaperType::wst_fuzzsoft, "Fuzz");
        p(sst::waveshapers::WaveshaperType::wst_fuzzsoftedge, "Fuzz");

        p(sst::waveshapers::WaveshaperType::wst_sinpx, "Trigonometric");
        p(sst::waveshapers::WaveshaperType::wst_sin2xpb, "Trigonometric");
        p(sst::waveshapers::WaveshaperType::wst_sin3xpb, "Trigonometric");
        p(sst::waveshapers::WaveshaperType::wst_sin7xpb, "Trigonometric");
        p(sst::waveshapers::WaveshaperType::wst_sin10xpb, "Trigonometric");
        p(sst::waveshapers::WaveshaperType::wst_2cyc, "Trigonometric");
        p(sst::waveshapers::WaveshaperType::wst_7cyc, "Trigonometric");
        p(sst::waveshapers::WaveshaperType::wst_10cyc, "Trigonometric");
        p(sst::waveshapers::WaveshaperType::wst_2cycbound, "Trigonometric");
        p(sst::waveshapers::WaveshaperType::wst_7cycbound, "Trigonometric");
        p(sst::waveshapers::WaveshaperType::wst_10cycbound, "Trigonometric");

        mapping = lmap;
    }
    return mapping;
}
} // namespace sst::waveshapers

#endif // SST_WAVESHAPERS_WAVESHAPERCONFIGURATION_H
