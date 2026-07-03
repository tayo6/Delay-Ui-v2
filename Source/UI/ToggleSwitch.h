#pragma once
#include <JuceHeader.h>
#include "DelayLookAndFeel.h"

/** Pill-shaped Studio/Creative mode switch with flanking labels, matching the
    HTML reference's ".toggle-switch" widget. Backed by a boolean parameter. */
class ToggleSwitch : public juce::Component
{
public:
    ToggleSwitch (juce::AudioProcessorValueTreeState& state, const juce::String& paramID,
                  juce::String leftLabel, juce::String rightLabel);

    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseUp (const juce::MouseEvent&) override;

private:
    juce::String leftText, rightText;
    juce::Rectangle<int> switchBounds;

    juce::AudioProcessorValueTreeState& apvts;
    std::unique_ptr<juce::ParameterAttachment> attachment;
    juce::RangedAudioParameter* parameter = nullptr;
    bool isOn = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToggleSwitch)
};
