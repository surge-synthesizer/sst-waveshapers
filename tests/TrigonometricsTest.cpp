#include "TestUtils.h"

TEST_CASE("Trigonometrics Test")
{
    SECTION("SINE + X")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_sinpx,
                            {0.0f, -0.387783f, 1.0f, 1.0f, -0.5f, 0.209018f, -1.0f, -1.0f}});
    }

    SECTION("SINE 2X + X")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_sin2xpb,
                            {0.0f, 0.960849f, 1.0f, 1.0f, 0.5f, -0.629015f, -1.0f, -1.0f}});
    }

    SECTION("SINE 3X + X")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_sin3xpb,
                            {0.0f, 0.960852f, 1.0f, 1.0f, 0.0f, -0.828136f, -1.0f, -1.0f}});
    }

    SECTION("SINE 7X + X")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_sin7xpb,
                            {0.0f, -0.560889f, 1.0f, 1.0f, 0.0f, -0.828203f, -1.0f, -1.0f}});
    }

    SECTION("SINE 10X + X")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_sin10xpb,
                            {0.0f, 0.20001f, 1.0f, 1.0f, 0.5f, -0.0999869f, -1.0f, -1.0f}});
    }

    //============================
    SECTION("2 CYCLE")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_2cyc,
                            {0.0f, 0.951061f, -2.44929e-16f, -2.44929e-16f, 1.22465e-16f,
                             -0.587792f, 2.44929e-16f, 2.44929e-16f}});
    }

    SECTION("7 CYCLE")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_7cyc,
                            {0.0f, -0.95111f, 8.57253e-16f, 8.57253e-16f, -1.0f, -0.809122f,
                             -8.57253e-16f, -8.57253e-16f}});
    }

    SECTION("10 CYCLE")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_10cyc,
                            {0.0f, 3.11434e-06f, -1.22465e-15f, -1.22465e-15f, 6.12323e-16f,
                             -4.10154e-06f, 1.22465e-15f, 1.22465e-15f}});
    }

    SECTION("2 CYCLE BOUND")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_2cycbound,
                            {0.0f, 0.760849f, -0.0f, -0.0f, 6.12323e-17f, -0.529015f, 0.0f, 0.0f}});
    }

    SECTION("7 CYCLE BOUND")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_7cycbound,
                            {0.0f, -0.760889f, 0.0f, 0.0f, -0.5f, -0.728203f, -0.0f, -0.0f}});
    }

    SECTION("10 CYCLE BOUND")
    {
        TestUtils::runTest(
            {sst::waveshapers::WaveshaperType::wst_10cycbound,
             {0.0f, 9.6783e-06f, -0.0f, -0.0f, 3.06162e-16f, 1.30795e-05f, 0.0f, 0.0f}});
    }
}
