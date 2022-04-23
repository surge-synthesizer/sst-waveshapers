message(STATUS "Configuring sst-waveshapers examples...")

# The examples need JUCE
CPMAddPackage("gh:juce-framework/JUCE#6.1.5")

add_subdirectory(waveshapers_example_plugin)
