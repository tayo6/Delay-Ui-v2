#pragma once
#include <JuceHeader.h>
#include "DelayLookAndFeel.h"

/** "AUTO GAIN" label + small LED rectangle indicator, click to toggle. */
class AutoGainIndicator : public juce::Component
{
public:
    AutoGainIndicator (juce::AudioProcessorValueTreeState& state, const juce::String& paramID);

    void paint (juce::Graphics&) override;
    void mouseUp (const juce::MouseEvent&) override;

private:
    juce::AudioProcessorValueTreeState& apvts;
    std::unique_ptr<juce::ParameterAttachment> attachment;
    juce::RangedAudioParameter* parameter = nullptr;
    bool isOn = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoGainIndicator)
};
