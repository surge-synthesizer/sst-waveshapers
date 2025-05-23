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

TEST_CASE("Click At Outset")
{
    for (int shindex = 1; shindex < (int)sst::waveshapers::WaveshaperType::n_ws_types; ++shindex)
    {
        if (shindex == (int)sst::waveshapers::WaveshaperType::wst_digital)
            continue;

        sst::waveshapers::QuadWaveshaperState wss{};

        auto wsptr =
            sst::waveshapers::GetQuadWaveshaper(sst::waveshapers::WaveshaperType::wst_none);

        auto wstype = sst::waveshapers::WaveshaperType(shindex);
        float drivef alignas(16)[4] = {0.25, 0.5, 1, 1.5};

        INFO("Changing Waveshaper Type to " << sst::waveshapers::wst_names[shindex]);
        float R alignas(16)[sst::waveshapers::n_waveshaper_registers];
        initializeWaveshaperRegister(wstype, R);
        for (int i = 0; i < sst::waveshapers::n_waveshaper_registers; ++i)
        {
            wss.R[i] = SIMD_MM(set1_ps)(R[i]);
        }

        wss.init = SIMD_MM(cmpeq_ps)(SIMD_MM(setzero_ps()), SIMD_MM(setzero_ps()));
        wsptr = sst::waveshapers::GetQuadWaveshaper(wstype);

        float din alignas(16)[4] = {0, 0, 0, 0};
        auto dat = SIMD_MM(load_ps)(din);
        auto drv = SIMD_MM(load_ps)(drivef);

        dat = wsptr(&wss, dat, drv);

        float res alignas(16)[4];

        SIMD_MM(store_ps)(res, dat);

        float rsum{0.f};
        for (int i = 0; i < 4; ++i)
            rsum += std::fabs(res[i]);
        REQUIRE(rsum < 1e-5);
        if (rsum > 1e-5)
        {
            std::cout << sst::waveshapers::wst_names[shindex] << " " << drivef[0] << "->" << res[0]
                      << " " << drivef[1] << "->" << res[1] << " " << drivef[2] << "->" << res[2]
                      << " " << drivef[3] << "->" << res[3] << " " << std::endl;
        }
    }
}
