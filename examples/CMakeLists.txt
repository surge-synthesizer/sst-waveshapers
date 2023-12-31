message(STATUS "Configuring sst-waveshapers examples...")

# The examples need JUCE
CPMAddPackage("gh:juce-framework/JUCE#7.0.8")

add_subdirectory(waveshapers_example_plugin)
