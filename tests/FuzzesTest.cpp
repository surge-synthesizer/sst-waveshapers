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
#if SKIP_FUZZES
    REQUIRE(true);
#else
    SECTION("FUZZ")
    {
        TestUtils::runTest(
            {sst::waveshapers::WaveshaperType::wst_fuzz,
             {0.f, 0.220378f, 0.92792f, 0.927827f, 0.357899f, -0.116849f, -1.00204f, -1.00194f}});
    }

    SECTION("HEAVY FUZZ")
    {
        TestUtils::runTest(
            {sst::waveshapers::WaveshaperType::wst_fuzzheavy,
             {0.f, 0.261135f, 0.7838f, 0.783722f, 0.0741361f, -0.150006f, -1.00559f, -1.00549f}});
    }

    SECTION("FUZZ CENTER")
    {
        TestUtils::runTest(
            {sst::waveshapers::WaveshaperType::wst_fuzzctr,
             {0.f, -0.241235f, 0.385376f, 0.591691f, 0.0723392f, -0.231693f, -1.21146f, -1.37649f},
             5e-2});
    }

    SECTION("FUZZ SOFT CLIP")
    {
        TestUtils::runTest(
            {sst::waveshapers::WaveshaperType::wst_fuzzsoft,
             {0.f, 0.211718f, 0.763057f, 0.803661f, 0.425659f, -0.117334f, -0.623519f, -0.944568f},
             5e-2});
    }

    SECTION("FUZZ SOFT EDGE")
    {
        TestUtils::runTest(
            {sst::waveshapers::WaveshaperType::wst_fuzzsoftedge,
             {0.f, 0.16799f, 0.6643f, 0.978468f, 0.395231f, -0.0849648f, -0.748686f, -0.838533f},
             5e-2});
    }
#endif
}
