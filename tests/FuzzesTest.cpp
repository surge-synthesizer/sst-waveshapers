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

TEST_CASE("Fuzzes Test")
{
    SECTION("FUZZ")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_fuzz,
                            {0.0113002f, 0.231677f, 0.939218f, 0.939124f, 0.369194f, -0.105554f,
                             -0.990744f, -0.990645f}});
    }

    SECTION("HEAVY FUZZ")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_fuzzheavy,
                            {0.0339005f, 0.295032f, 0.817694f, 0.817612f, 0.108023f, -0.116123f,
                             -0.971712f, -0.971614f}});
    }

    SECTION("FUZZ CENTER")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_fuzzctr,
                            {0.392422f, 0.137087f, 0.777535f, 0.983756f, 0.465164f, 0.163438f,
                             -0.81944f, -0.984097f},
                            5e-2});
    }

    SECTION("FUZZ SOFT CLIP")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_fuzzsoft,
                            {-0.0498637f, 0.176861f, 0.769438f, 0.882686f, 0.411067f, -0.0660712f,
                             -0.874844f, -0.924149f},
                            5e-2});
    }

    SECTION("FUZZ SOFT EDGE")
    {
        TestUtils::runTest(
            {sst::waveshapers::WaveshaperType::wst_fuzzsoftedge,
             {0.0f, 0.167961f, 0.65832f, 1.00573f, 0.395507f, -0.0849603f, -0.75362f, -0.826739f},
             5e-2});
    }
}
