#include "TestUtils.h"

TEST_CASE("Saturator Tests")
{
    SECTION("SOFT")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_soft,
                            {0.0f, 0.197634f, 0.777588f, 0.983887f, 0.465698f, -0.0996966f,
                             -0.819147f, -0.983887f}});
    }

    SECTION("MEDIUM")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_zamsat,
                            {0.0f, 0.36f, 1.0f, 1.0f, 0.75f, -0.19f, -1.0f, -1.0f}});
    }

    SECTION("HARD")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_hard,
                            {0.0f, 0.2f, 1.0f, 1.0f, 0.5f, -0.1f, -1.0f, -1.0f}});
    }

    SECTION("ASYM")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_asym,
                            {9.59536e-09f, 0.142403f, 0.42973f, 0.501537f, 0.295492f, -0.0838315f,
                             -1.12282f, -1.72613f}});
    }

    SECTION("OJD")
    {
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_ojd,
                            {0.0f, 0.2f, 0.975f, 1.0f, 0.5f, -0.1f, -0.871429f, -1.0f}});
    }
}
