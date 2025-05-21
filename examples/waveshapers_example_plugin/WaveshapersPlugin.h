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
#pragma once

#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
#include <sst/waveshapers.h>

namespace ParamTags
{
const juce::String driveTag = "drive";
const juce::String waveshaperTypeTag = "ws_type";
} // namespace ParamTags

class WaveshapersPlugin : public juce::AudioProcessor
{
  public:
    WaveshapersPlugin();

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }

    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; };
    int getCurrentProgram() override { return 0; };
    void setCurrentProgram(int) override { return; };
    const juce::String getProgramName(int) override { return juce::String(); };
    void changeProgramName(int, const juce::String &) override {}

    bool isBusesLayoutSupported(const juce::AudioProcessor::BusesLayout &layouts) const override;
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

    bool hasEditor() const override { return true; }
    juce::AudioProcessorEditor *createEditor() override;

    void getStateInformation(juce::MemoryBlock &data) override;
    void setStateInformation(const void *data, int sizeInBytes) override;

    auto &getVTS() { return vts; }

  private:
    std::atomic<float> *driveParam = nullptr;
    std::atomic<float> *waveshaperTypeParam = nullptr;
    juce::AudioProcessorValueTreeState vts;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative> driveSmoothed;
    sst::waveshapers::QuadWaveshaperState wss{};
    sst::waveshapers::WaveshaperType lastType = sst::waveshapers::WaveshaperType::wst_none;

    std::unique_ptr<juce::dsp::Oversampling<float>> oversampling;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveshapersPlugin)
};
