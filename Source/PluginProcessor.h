#pragma once

#include <JuceHeader.h>
#include <array>
#include <atomic>

/**
    Core audio engine for the Delay plugin.

    Signal path per channel:
        input -> [tempo-synced delay line] -> wet
        feedback loop: dry + (filtered, optionally saturated) wet * regen -> pushed back into line
        output = dry*(1-mix) + wet*mix, then output trim (with optional auto-gain compensation)
*/
class DelayVSTAudioProcessor : public juce::AudioProcessor
{
public:
    DelayVSTAudioProcessor();
    ~DelayVSTAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    using AudioProcessor::processBlock;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }

    bool acceptsMidi() const override  { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 4.0; }

    int getNumPrograms() override                          { return 1; }
    int getCurrentProgram() override                       { return 0; }
    void setCurrentProgram (int) override                  {}
    const juce::String getProgramName (int) override       { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

    // Peak levels, updated every block, polled by the editor's VU meter timer.
    std::atomic<float> inputLevelL  { 0.0f };
    std::atomic<float> inputLevelR  { 0.0f };
    std::atomic<float> outputLevelL { 0.0f };
    std::atomic<float> outputLevelR { 0.0f };

    static constexpr int numTempoDivisions = 6;
    static const juce::StringArray tempoDivisionLabels;

    static const juce::String tempoParamID, regenParamID, mixParamID, outputParamID,
                               modeParamID, autoGainParamID, brightnessParamID,
                               colorParamID, sparkleParamID;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    double getTempoDivisionSeconds (int index, double bpm) const;

    struct ChannelState
    {
        juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> delayLine { 192000 };
        float feedbackFilterState = 0.0f;
        float lfoPhase = 0.0f;
    };

    std::array<ChannelState, 2> channels;

    double currentSampleRate = 44100.0;
    juce::SmoothedValue<float> delayTimeSmoother;
    juce::SmoothedValue<float> outputGainSmoother;

    float lastKnownBpm = 120.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayVSTAudioProcessor)
};
