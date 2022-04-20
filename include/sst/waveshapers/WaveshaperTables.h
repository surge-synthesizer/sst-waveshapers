#ifndef SST_WAVESHAPERS_WAVESHAPERTABLES_H
#define SST_WAVESHAPERS_WAVESHAPERTABLES_H

#include <cmath>

#include "WaveshaperConfiguration.h"

namespace sst::waveshapers
{
/** Lookup table for waveshapers */
struct WaveshaperTables
{
    static constexpr int tableSize = 1024;
    float waveshapers alignas(16)[static_cast<int> (WaveshaperType::n_ws_types)][tableSize];

    static inline double shafted_tanh(double x) { return (std::exp(x) - std::exp(-x * 1.2)) / (std::exp(x) + std::exp(-x)); }

    WaveshaperTables()
    {
        constexpr double mult = 1.0 / 32.0;
        for (int i = 0; i < tableSize; i++)
        {
            double x = ((double)i - 512.0) * mult;

            waveshapers[static_cast<int> (WaveshaperType::wst_soft)][i] = (float)std::tanh(x);

            waveshapers[static_cast<int> (WaveshaperType::wst_hard)][i] = (float)std::pow(std::tanh(std::pow(std::abs(x), 5.0)), 0.2);
            if (x < 0)
                waveshapers[static_cast<int> (WaveshaperType::wst_hard)][i] = -waveshapers[static_cast<int> (WaveshaperType::wst_hard)][i];

            waveshapers[static_cast<int> (WaveshaperType::wst_asym)][i] = (float)shafted_tanh(x + 0.5) - shafted_tanh(0.5);
            waveshapers[static_cast<int> (WaveshaperType::wst_sine)][i] = (float)std::sin((double)((double)i - 512.0) * M_PI / 512.0);
            waveshapers[static_cast<int> (WaveshaperType::wst_digital)][i] = (float)std::tanh(x);
        }
    }
};

const static WaveshaperTables globalWaveshaperTables;
}

#endif // SST_WAVESHAPERS_WAVESHAPERTABLES_H
