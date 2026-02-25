/*
 * sst-waveshaper - an open source library of waveshaper algorithms
 * by the Surge Synth Team
 *
 * Copyright 2018-2025, various authors, as described in the GitHub
 * transaction log.
 *
 * sst-jucegui is released under the GNU General Public License 3 or later
 * as found in LICENSE.md in this repository.
 *
 * All source in sst-waveshapers available at
 * https://github.com/surge-synthesizer/sst-waveshapers
 */

#include "TestUtils.h"
#include <iostream>
#include "sst/basic-blocks/simd/setup.h"
#include "sst/waveshapers.h"
#include <cmath>
#include <vector>
#include <random>
#include <algorithm>

// Thanks to Xenakios for contributing this test as a way to hunt a bug
TEST_CASE("All Items are Bounded on Change")
{
    std::vector<int> shapertypes;
    for (int i = 1; i < 45; ++i)
    {
        shapertypes.push_back(i);
    }
    std::mt19937 rng{1003};
    std::shuffle(shapertypes.begin(), shapertypes.end(), rng);
    sst::waveshapers::QuadWaveshaperState wss{};
    float R[sst::waveshapers::n_waveshaper_registers];
    auto wsptr = sst::waveshapers::GetQuadWaveshaper(sst::waveshapers::WaveshaperType::wst_none);
    int curtypeindex = -1;
    int olen = 45 * 22050;

    double sr = 44100.0 * 4;
    int ssc{0};
    for (int i = 0; i < olen; ++i)
    {
        if (i % 12001 == 0)
        {
            ssc = 0;
            ++curtypeindex;
            if (curtypeindex == shapertypes.size())
                curtypeindex = 0;
            int shindex = shapertypes[curtypeindex];
            auto wstype = sst::waveshapers::WaveshaperType(shindex);
            INFO("Changing Waveshaper Type to " << sst::waveshapers::wst_names[shindex]);
            initializeWaveshaperRegister(wstype, R);
            for (int i = 0; i < sst::waveshapers::n_waveshaper_registers; ++i)
            {
                wss.R[i] = SIMD_MM(set1_ps)(R[i]);
            }

            wss.init = SIMD_MM(cmpeq_ps)(SIMD_MM(setzero_ps()), SIMD_MM(setzero_ps()));
            wsptr = sst::waveshapers::GetQuadWaveshaper(wstype);
        }
        if (!wsptr)
            return;
        float insig = 1.0 * std::sin(M_PI * 2.0 / sr * 1000.0 * i);
        float din alignas(16)[4] = {0, 0, 0, 0};
        din[0] = insig;
        din[1] = insig;
        auto dat = SIMD_MM(load_ps)(din);
        auto drv = SIMD_MM(set1_ps)(0.25);

        int shindex = shapertypes[curtypeindex];
        auto wstype = sst::waveshapers::WaveshaperType(shindex);

        dat = wsptr(&wss, dat, drv);

        float res alignas(16)[4];

        SIMD_MM(store_ps)(res, dat);

        if (res[0] < -2.0 || res[0] > 2.0 || std::isnan(res[0]) || std::isinf(res[0]) ||
            res[1] < -2.0 || res[1] > 2.0 || std::isnan(res[1]) || std::isinf(res[1]))
        {
            INFO("Bad sample produced by waveshaper at " << ssc << " " << res[0] << " " << res[1]);
            REQUIRE(false);
        }
        ssc++;
    }
}