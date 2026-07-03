#include "PluginProcessor.h"
#include "PluginEditor.h"

const juce::StringArray DelayVSTAudioProcessor::tempoDivisionLabels { "1/32", "1/16", "1/8", "1/4", "1/2", "1/1" };

const juce::String DelayVSTAudioProcessor::tempoParamID      = "tempo";
const juce::String DelayVSTAudioProcessor::regenParamID      = "regen";
const juce::String DelayVSTAudioProcessor::mixParamID        = "mix";
const juce::String DelayVSTAudioProcessor::outputParamID     = "output";
const juce::String DelayVSTAudioProcessor::modeParamID       = "mode";
const juce::String DelayVSTAudioProcessor::autoGainParamID   = "autoGain";
const juce::String DelayVSTAudioProcessor::brightnessParamID = "brightness";
const juce::String DelayVSTAudioProcessor::colorParamID      = "color";
const juce::String DelayVSTAudioProcessor::sparkleParamID    = "sparkle";

DelayVSTAudioProcessor::DelayVSTAudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

DelayVSTAudioProcessor::~DelayVSTAudioProcessor() = default;

juce::AudioProcessorValueTreeState::ParameterLayout DelayVSTAudioProcessor::createParameterLayout()
{
    using namespace juce;
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back (std::make_unique<AudioParameterChoice> (
        ParameterID { tempoParamID, 1 }, "Tempo", tempoDivisionLabels, 2));

    params.push_back (std::make_unique<AudioParameterFloat> (
        ParameterID { regenParamID, 1 }, "Regen",
        NormalisableRange<float> (0.0f, 100.0f, 0.1f), 35.0f, "%"));

    params.push_back (std::make_unique<AudioParameterFloat> (
        ParameterID { mixParamID, 1 }, "Mix",
        NormalisableRange<float> (0.0f, 100.0f, 0.1f), 40.0f, "%"));

    params.push_back (std::make_unique<AudioParameterFloat> (
        ParameterID { outputParamID, 1 }, "Output",
        NormalisableRange<float> (-24.0f, 12.0f, 0.1f), 0.0f, "dB"));

    params.push_back (std::make_unique<AudioParameterBool> (
        ParameterID { modeParamID, 1 }, "Creative Mode", false));

    params.push_back (std::make_unique<AudioParameterBool> (
        ParameterID { autoGainParamID, 1 }, "Auto Gain", true));

    params.push_back (std::make_unique<AudioParameterBool> (
        ParameterID { brightnessParamID, 1 }, "Brightness", false));

    params.push_back (std::make_unique<AudioParameterBool> (
        ParameterID { colorParamID, 1 }, "Color", false));

    params.push_back (std::make_unique<AudioParameterBool> (
        ParameterID { sparkleParamID, 1 }, "Sparkle", false));

    return { params.begin(), params.end() };
}

void DelayVSTAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate       = sampleRate;
    spec.maximumBlockSize = (juce::uint32) samplesPerBlock;
    spec.numChannels      = 1;

    for (auto& ch : channels)
    {
        ch.delayLine.prepare (spec);
        ch.delayLine.setMaximumDelayInSamples ((int) (sampleRate * 4.0));
        ch.delayLine.reset();
        ch.feedbackFilterState = 0.0f;
        ch.lfoPhase = 0.0f;
    }

    delayTimeSmoother.reset (sampleRate, 0.05);
    delayTimeSmoother.setCurrentAndTargetValue (0.25f);

    outputGainSmoother.reset (sampleRate, 0.02);
    outputGainSmoother.setCurrentAndTargetValue (1.0f);
}

void DelayVSTAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayVSTAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    return true;
}
#endif

double DelayVSTAudioProcessor::getTempoDivisionSeconds (int index, double bpm) const
{
    const double quarterNote = 60.0 / juce::jmax (20.0, bpm);
    switch (index)
    {
        case 0: return quarterNote / 8.0;  // 1/32
        case 1: return quarterNote / 4.0;  // 1/16
        case 2: return quarterNote / 2.0;  // 1/8
        case 3: return quarterNote;        // 1/4
        case 4: return quarterNote * 2.0;  // 1/2
        case 5: return quarterNote * 4.0;  // 1/1
        default: return quarterNote / 2.0;
    }
}

void DelayVSTAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const auto totalNumInputChannels  = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    const int tempoIndex   = (int) apvts.getRawParameterValue (tempoParamID)->load();
    const float regen      = apvts.getRawParameterValue (regenParamID)->load() / 100.0f;
    const float mix        = apvts.getRawParameterValue (mixParamID)->load() / 100.0f;
    const float outputDb   = apvts.getRawParameterValue (outputParamID)->load();
    const bool  creative   = apvts.getRawParameterValue (modeParamID)->load() > 0.5f;
    const bool  autoGainOn = apvts.getRawParameterValue (autoGainParamID)->load() > 0.5f;
    const bool  bright     = apvts.getRawParameterValue (brightnessParamID)->load() > 0.5f;
    const bool  colorOn    = apvts.getRawParameterValue (colorParamID)->load() > 0.5f;
    const bool  sparkleOn  = apvts.getRawParameterValue (sparkleParamID)->load() > 0.5f;

    double bpm = lastKnownBpm;
    if (auto* currentPlayHead = getPlayHead())
    {
        if (auto position = currentPlayHead->getPosition())
        {
            if (auto hostBpm = position->getBpm())
                bpm = *hostBpm;
        }
    }
    lastKnownBpm = (float) bpm;

    const double targetDelaySeconds = getTempoDivisionSeconds (tempoIndex, bpm);
    delayTimeSmoother.setTargetValue ((float) targetDelaySeconds);

    const float outGainTarget = juce::Decibels::decibelsToGain (outputDb) *
        (autoGainOn ? (1.0f / (1.0f + regen * 0.5f)) : 1.0f);
    outputGainSmoother.setTargetValue (outGainTarget);

    const float feedbackAmount   = regen * (creative ? 1.05f : 0.95f);
    const float lfoRateHz        = colorOn ? 0.35f : 0.0f;
    const float lfoDepthSamples  = colorOn ? (float) (currentSampleRate * 0.0015) : 0.0f;
    const float brightCoeff      = bright ? 0.15f : 0.55f; // one-pole coeff in feedback path (lower = brighter)

    const int numSamples  = buffer.getNumSamples();
    const int numChannels = juce::jmin (buffer.getNumChannels(), 2);

    float peakIn[2]  = { 0.0f, 0.0f };
    float peakOut[2] = { 0.0f, 0.0f };

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* data = buffer.getWritePointer (ch);
        auto& state = channels[(size_t) ch];

        for (int n = 0; n < numSamples; ++n)
        {
            const float dry = data[n];
            peakIn[ch] = juce::jmax (peakIn[ch], std::abs (dry));

            const float delaySeconds = delayTimeSmoother.getNextValue();
            float delaySamples = (float) (delaySeconds * currentSampleRate);

            if (colorOn)
            {
                state.lfoPhase += lfoRateHz / (float) currentSampleRate;
                if (state.lfoPhase > 1.0f)
                    state.lfoPhase -= 1.0f;
                delaySamples += std::sin (state.lfoPhase * juce::MathConstants<float>::twoPi) * lfoDepthSamples;
            }

            delaySamples = juce::jlimit (1.0f, (float) (currentSampleRate * 4.0 - 1.0), delaySamples);
            state.delayLine.setDelay (delaySamples);

            const float wet = state.delayLine.popSample (0);

            // Brightness: one-pole lowpass smoothing inside the feedback loop.
            state.feedbackFilterState += brightCoeff * (wet - state.feedbackFilterState);
            float filteredFeedback = state.feedbackFilterState;

            // Sparkle: gentle harmonic saturation on the repeats.
            if (sparkleOn)
                filteredFeedback = std::tanh (filteredFeedback * 1.6f) * 0.85f + filteredFeedback * 0.15f;

            float feedbackSample = dry + filteredFeedback * feedbackAmount;
            if (creative)
                feedbackSample = std::tanh (feedbackSample * 1.15f); // subtle console-style saturation

            state.delayLine.pushSample (0, feedbackSample);

            float outSample = dry * (1.0f - mix) + wet * mix;
            outSample *= outputGainSmoother.getNextValue();

            peakOut[ch] = juce::jmax (peakOut[ch], std::abs (outSample));
            data[n] = outSample;
        }
    }

    inputLevelL.store (peakIn[0]);
    inputLevelR.store (numChannels > 1 ? peakIn[1] : peakIn[0]);
    outputLevelL.store (peakOut[0]);
    outputLevelR.store (numChannels > 1 ? peakOut[1] : peakOut[0]);
}

juce::AudioProcessorEditor* DelayVSTAudioProcessor::createEditor()
{
    return new DelayVSTAudioProcessorEditor (*this);
}

void DelayVSTAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void DelayVSTAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName (apvts.state.getType()))
        apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayVSTAudioProcessor();
}
