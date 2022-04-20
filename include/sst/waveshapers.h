#ifndef _SST_CPP_WAVESHAPERS_H
#define _SST_CPP_WAVESHAPERS_H

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
