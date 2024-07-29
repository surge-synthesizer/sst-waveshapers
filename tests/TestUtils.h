#ifndef SST_WAVESHAPERS_TESTUTILS_H
#define SST_WAVESHAPERS_TESTUTILS_H

#include <array>
#include <iostream>

#include <catch2/catch2.hpp>
#include <sst/waveshapers.h>

namespace TestUtils
{
constexpr bool printData = false; // change this to true when generating test data

using namespace sst::waveshapers;

using DataSet = std::array<float, 8>;
constexpr DataSet testInputData{0.0f, 0.1f, 0.5f, 1.0f, 0.25f, -0.05f, -0.55f, -1.0f};

struct TestConfig
{
    WaveshaperType wsType;
    DataSet expectedData;
};

inline void runTest(const TestConfig &config)
{
    QuadWaveshaperState wsState;
    float R[n_waveshaper_registers];
    initializeWaveshaperRegister(config.wsType, R);

    for (int i = 0; i < n_waveshaper_registers; ++i)
    {
        wsState.R[i] = _mm_set1_ps(R[i]);
    }

    wsState.init = _mm_cmpneq_ps(_mm_setzero_ps(), _mm_setzero_ps());

    auto wsPtr = GetQuadWaveshaper(config.wsType);

    DataSet actualData{};
    for (int i = 0; i < actualData.size(); ++i)
    {
        auto input = _mm_set1_ps(testInputData[i]);
        auto drive = _mm_set1_ps(2.0f);
        auto output = wsPtr(&wsState, input, drive);

        float outArr alignas(16)[4];
        _mm_store_ps(outArr, output);
        actualData[i] = outArr[0];

        if constexpr (!printData)
            REQUIRE(actualData[i] == Approx(config.expectedData[i]).margin(1.0e-4f));
    }

    if constexpr (printData)
    {
        std::cout << "{ ";
        for (int i = 0; i < actualData.size(); ++i)
            std::cout << actualData[i] << "f, ";

        std::cout << "}" << std::endl;
    }
}
} // namespace TestUtils

#endif // SST_WAVESHAPERS_TESTUTILS_H
