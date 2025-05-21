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
#include "WaveshapersPlugin.h"
// #include "FiltersPluginEditor.h"

WaveshapersPlugin::WaveshapersPlugin()
    : juce::AudioProcessor(BusesProperties()
                               .withInput("Input", juce::AudioChannelSet::stereo(), true)
                               .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      vts(*this, nullptr, juce::Identifier("Parameters"), createParameters())
{
    using namespace ParamTags;
    driveParam = vts.getRawParameterValue(driveTag);
    waveshaperTypeParam = vts.getRawParameterValue(waveshaperTypeTag);
}

juce::AudioProcessorValueTreeState::ParameterLayout WaveshapersPlugin::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    using VTSParam = juce::AudioProcessorValueTreeState::Parameter;
    using namespace ParamTags;

    params.push_back(std::make_unique<VTSParam>(
        driveTag, "Drive", juce::String(), juce::NormalisableRange{-24.0f, 24.0f}, 0.0f,
        [](float dbVal) { return juce::String(dbVal, 2, false) + " dB"; },
        [](const juce::String &s) { return s.getFloatValue(); }));

    juce::StringArray waveshaperTypeChoices;
    for (const auto &filter_type_name : sst::waveshapers::wst_names)
        waveshaperTypeChoices.add(filter_type_name);

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        waveshaperTypeTag, "Waveshaper Type", waveshaperTypeChoices, 0));

    return {params.begin(), params.end()};
}

bool WaveshapersPlugin::isBusesLayoutSupported(
    const juce::AudioProcessor::BusesLayout &layouts) const
{
    // only supports mono and stereo
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // input and output layout must be the same
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void WaveshapersPlugin::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    const auto numChannels = getTotalNumInputChannels();
    oversampling = std::make_unique<juce::dsp::Oversampling<float>>(
        numChannels, 1, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR);
    oversampling->initProcessing(samplesPerBlock);

    driveSmoothed.reset(sampleRate * (double)oversampling->getOversamplingFactor(), 0.05);
}

void WaveshapersPlugin::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &)
{
    // set up waveshaper
    const auto newType = static_cast<sst::waveshapers::WaveshaperType>((int)*waveshaperTypeParam);
    if (newType != lastType)
    {
        lastType = newType;

        float R[sst::waveshapers::n_waveshaper_registers];
        initializeWaveshaperRegister(lastType, R);

        for (int i = 0; i < sst::waveshapers::n_waveshaper_registers; ++i)
        {
            wss.R[i] = _mm_set1_ps(R[i]);
        }

        wss.init = _mm_cmpeq_ps(_mm_setzero_ps(), _mm_setzero_ps());
    }

    driveSmoothed.setTargetValue(juce::Decibels::decibelsToGain(driveParam->load()));
    auto wsptr = sst::waveshapers::GetQuadWaveshaper(lastType);

    // now upsample
    auto &&block = juce::dsp::AudioBlock<float>{buffer};
    auto &&osBlock = oversampling->processSamplesUp(block);

    const auto osNumChannels = osBlock.getNumChannels();
    auto *leftData = osBlock.getChannelPointer(0);
    auto *rightData = osBlock.getChannelPointer(1 % osNumChannels);

    const auto osNumSamples = (int)osBlock.getNumSamples();

    // process data
    if (wsptr)
    {
        float din alignas(16)[4] = {0, 0, 0, 0};
        for (int i = 0; i < osNumSamples; ++i)
        {
            din[0] = leftData[i];
            din[1] = rightData[i];

            auto dat = _mm_load_ps(din);
            auto drv = _mm_set1_ps(driveSmoothed.getNextValue());

            dat = wsptr(&wss, dat, drv);

            float res alignas(16)[4];

            _mm_store_ps(res, dat);

            leftData[i] = res[0];
            rightData[i] = res[1];
        }
    }

    // and downsample
    oversampling->processSamplesDown(block);
}

juce::AudioProcessorEditor *WaveshapersPlugin::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

void WaveshapersPlugin::getStateInformation(juce::MemoryBlock &data)
{
    auto state = vts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, data);
}

void WaveshapersPlugin::setStateInformation(const void *data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr)
        if (xmlState->hasTagName(vts.state.getType()))
            vts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// This creates new instances of the plugin
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter() { return new WaveshapersPlugin(); }
