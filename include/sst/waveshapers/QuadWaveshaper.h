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
#ifndef SST_WAVESHAPER_SST_WAVESHAPERS_QUADWAVESHAPER_H
#define SST_WAVESHAPER_SST_WAVESHAPERS_QUADWAVESHAPER_H

#ifndef SST_WAVESHAPER_SST_WAVESHAPERS_QUADWAVESHAPER_H
#define SST_WAVESHAPER_SST_WAVESHAPERS_QUADWAVESHAPER_H

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

#endif // SST_WAVESHAPER_SST_WAVESHAPERS_QUADWAVESHAPER_H
