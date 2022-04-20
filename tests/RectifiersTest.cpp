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
        TestUtils::runTest({sst::waveshapers::WaveshaperType::wst_softrect,
                            {-0.983887f, -0.969348f, -0.674638f, 0.777768f, 0.465924f, -0.940015f,
                             -0.674389f, 0.819212f}});
    }
}
