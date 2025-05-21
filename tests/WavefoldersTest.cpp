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

TEST_CASE("Wavefolders Test")
{
    SECTION("SOFT FOLD")
    {
        TestUtils::runTest(
            {sst::waveshapers::WaveshaperType::wst_softfold,
             {0.0f, 0.467285f, 0.90918f, 0.625f, 0.869507f, -0.245654f, -0.882202f, -0.625f}});
    }

    SECTION("SINGLE FOLD")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_singlefold,
                            {0.0f, -0.142857f, -0.764689f, -0.827755f, -0.859596f, -0.285728f,
                             0.725475f, 0.817184f}});
    }

    SECTION("DUAL FOLD")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_dualfold,
                            {0.0f, 0.333333f, 0.104167f, -0.524872f, -0.445129f, 0.46034f,
                             -0.0380738f, 0.477488f}});
    }

    SECTION("WEST COAST FOLD")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_westfold,
                            {0.0f, 0.339372f, -0.0524914f, -0.0560328f, -0.0216139f, -0.0378961f,
                             -0.0502611f, 0.117152f}});
    }
}
