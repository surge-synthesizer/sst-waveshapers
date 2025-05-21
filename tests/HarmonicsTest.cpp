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

TEST_CASE("Harmonics Test")
{
    SECTION("Soft Harmonic 2")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_cheby2,
                            {-0.983887f, -0.981756f, -0.777578f, 0.983957f, -0.964114f, -0.983526f,
                             -0.668806f, 0.984113f}});
    }

    SECTION("Soft Harmonic 3")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_cheby3,
                            {0.0f, -0.53699f, -0.984323f, 0.984047f, -0.902664f, 0.291438f,
                             0.98229f, -0.983905f}});
    }

    SECTION("Soft Harmonic 4")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_cheby4,
                            {0.983887f, 0.972811f, -0.777908f, 0.984184f, 0.80417f, 0.981804f,
                             -0.902898f, 0.984195f}});
    }

    SECTION("Soft Harmonic 5")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_cheby5,
                            {0.0f, 0.759491f, 0.777828f, 0.984238f, 0.977895f, -0.462347f,
                             -0.429785f, -0.983954f}});
    }

    SECTION("Additive 1+2")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_add12,
                            {-0.5f, -0.417053f, 0.135844f, 0.734737f, -0.237945f, -0.528582f,
                             -0.418359f, -0.154147f}});
    }

    SECTION("Additive 1+3")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_add13,
                            {0.0f, -0.126781f, -0.183948f, 0.515924f, -0.254345f, 0.0653358f,
                             0.130524f, -0.515924f}});
    }

    SECTION("Additive 1+4")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_add14,
                            {0.5f, 0.497825f, -0.108627f, 0.281226f, 0.292698f, 0.449599f,
                             -0.771982f, -0.607728f}});
    }

    SECTION("Additive 1+5")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_add15,
                            {0.0f, 0.3716f, 0.299508f, 0.0829485f, 0.659056f, -0.194872f,
                             -0.184647f, -0.0829485f}});
    }

    SECTION("Additive 12345")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_add12345,
                            {0.0f, 0.0514506f, -0.29426f, 0.112605f, -0.00797249f, -0.0438016f,
                             -0.119992f, -0.0109151f}});
    }

    SECTION("Additive Saw 3")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_addsaw3,
                            {-0.257143f, -0.230203f, 0.0978563f, 0.62482f, -0.146698f, -0.263539f,
                             -0.262559f, -0.326252f}});
    }

    SECTION("Additive Square 3")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_addsqr3,
                            {0.0f, 0.294392f, 0.913658f, 0.895026f, 0.652641f, -0.149863f,
                             -0.92596f, -0.895026f}});
    }
}
