#ifndef SST_WAVESHAPERS_QUADWAVESHAPER_H
#define SST_WAVESHAPERS_QUADWAVESHAPER_H

#pragma once

#include "sst/utilities/globals.h"
#include "WaveshaperConfiguration.h"

namespace sst::waveshapers
{

constexpr int n_waveshaper_registers = 4;

/*
 * These are defined in QuadFilterWaveshapers.cpp
 */
struct alignas(16) QuadWaveshaperState
{
    SIMD_M128 R[n_waveshaper_registers];
    SIMD_M128 init;
};

typedef SIMD_M128 (*QuadWaveshaperPtr)(QuadWaveshaperState *__restrict, SIMD_M128 in,
                                       SIMD_M128 drive);

QuadWaveshaperPtr GetQuadWaveshaper(WaveshaperType type);

/*
 * Given the very first sample inbound to a new voice session, return the
 * first set of registers for that voice.
 */
void initializeWaveshaperRegister(
    WaveshaperType type,
    float R[n_waveshaper_registers]); // @TODO: not sure we need this? R is being copied...

} // namespace sst::waveshapers

#endif // SST_WAVESHAPERS_QUADWAVESHAPER_H
