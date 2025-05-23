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

TEST_CASE("Rectifiers Test")
{
    SECTION("FULL WAVE")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_fwrectify,
                            {0.0f, 0.1f, 0.6f, 1.0f, 0.749817f, 0.216677f, 0.549987f, 1.0f}});
    }

    SECTION("HALF WAVE POSITIVE")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_poswav,
                            {0.0f, 0.1f, 0.6f, 1.0f, 0.749817f, 0.208344f, 0.0f, 0.0f}});
    }

    SECTION("HALF WAVE NEGATIVE")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_negwav,
                            {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.00833374f, -0.549987f, -1.0f}});
    }

    SECTION("SOFT RECTIFIER")
    {
        TestUtils::runTest(
            {sst::waveshapers::WaveshaperType::wst_softrect,
             {0.f, 0.19766f, 0.854048f, 0.999996f, 0.998494f, 0.41006f, 0.853818f, 1.f}});
    }
}
