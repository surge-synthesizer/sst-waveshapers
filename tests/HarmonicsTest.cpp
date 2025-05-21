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
                            {
                                0.f,
                                0.039981f,
                                0.777773f,
                                1.f,
                                0.244986f,
                                0.00947075f,
                                0.85708f,
                                1.f,
                            }});
    }

    SECTION("Soft Harmonic 3")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_cheby3,
                            {0.f, -0.536956f, -0.984123f, 0.984142f, -0.902497f, 0.29149f,
                             0.982302f, -0.984129f}});
    }

    SECTION("Soft Harmonic 4")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_cheby4,
                            {0.f, -0.157232f, -0.999996f, 0.000315666f, -0.748524f, -0.0394723f,
                             -1.f, 0.000751017f}});
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
                            {0.f, 0.0829067f, 0.635747f, 1.23454f, 0.261891f, -0.0288343f,
                             0.0813495f, 0.345475f}});
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
                            {0.f, -0.0021314f, -0.608528f, -0.218725f, -0.207151f, -0.0501574f,
                             -1.2717f, -1.10746f}});
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
                            {1.49012e-07f, 0.0269183f, 0.35495f, 0.881837f, 0.110362f, -0.00652488f,
                             -0.00557171f, -0.0692796f}});
    }

    SECTION("Additive Square 3")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_addsqr3,
                            {0.0f, 0.294392f, 0.913658f, 0.895026f, 0.652641f, -0.149863f,
                             -0.92596f, -0.895026f}});
    }
}
