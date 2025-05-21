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
#ifndef SST_WAVESHAPER_SST_WAVESHAPERS_H
#define SST_WAVESHAPER_SST_WAVESHAPERS_H

/** Parent namespace for all Surge Synth Team code. */
namespace sst
{

/** DSP code for Surge waveshapers */
namespace waveshapers
{

/**
 * Utility code needed for Surge waveshapers.
 * Note that this code may be moved to a submodule
 * at some point in the future.
 */
namespace utilities
{
}

} // namespace waveshapers
} // namespace sst

#include "waveshapers/QuadWaveshaper.h"

#include "waveshapers/QuadWaveshaper_Impl.h"

#endif // _SST_CPP_WAVESHAPERS_H
