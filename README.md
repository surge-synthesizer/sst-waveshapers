# SST Waveshapers

Tests: [![CI Build Status](https://dev.azure.com/surge-synthesizer/surge/_apis/build/status/surge-synthesizer.sst-waveshapers?branchName=main)](https://dev.azure.com/surge-synthesizer/surge/_apis/build/status/surge-synthesizer.sst-waveshapers?branchName=main)

This repository contains the waveshapers from the
[Surge Synthesizer](https://github.com/surge-synthesizer/surge)
as a submodule.

Currently a work-in-progress. Check back later!

## Usage

`sst-waveshapers` is a header-only library, so it is possible to use the library
simply by adding `include/` to your header search paths, and writing
`#include <sst/waveshapers.h>` somewhere in your source code. However, it is
recommended to use the provided CMake configuration.

```cmake
add_subdirectory(path/to/sst-waveshapers)
target_link_libraries(My-Killer-App PUBLIC sst-waveshapers)
```

**Note: the sst-waveshapers library uses raw SSE SIMD intrinsics. If you
are planning to use this library in a context that may not support SSE
intrinsics, it is recommended to also link with
[simde](https://github.com/simd-everywhere/simde).**

## Building Unit Tests

To build and run the sst-waveshapers unit tests:
```bash
cmake -Bbuild
cmake --build build
./build/test-binary/sst-waveshapers-tests
```

## License
The code in this repository is licensed under the General Public License v3.
