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
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_fuzz,
                            {0.00000000f, 0.22037821f, 0.92791998f, 0.92782718f, 0.35789865f,
                             -0.12865001f, -1.00203609f, -1.00193584f}});
    }

    SECTION("HEAVY FUZZ")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_fuzzheavy,
                            {0.00000000f, 0.26113454f, 0.78380001f, 0.78372163f, 0.07413614f,
                             -0.18540987f, -1.00558805f, -1.00548744f}});
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
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_fuzzsoft,
                            {0.00000000f, 0.22699629f, 0.81701916f, 0.94508040f, 0.46550822f,
                             -0.02013272f, -0.72161263f, -0.86463588f},
                            5e-2});
    }

    SECTION("FUZZ SOFT EDGE")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_fuzzsoftedge,
                            {0.00000000f, 0.16798960f, 0.66430008f, 0.92657322f, 0.39519930f,
                             -0.08495888f, -0.74868125f, -0.83852774f},
                            5e-2});
    }
#endif
}
