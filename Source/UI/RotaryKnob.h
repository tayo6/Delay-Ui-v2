#pragma once
#include <JuceHeader.h>
#include "DelayLookAndFeel.h"

/** A rotary slider with a text label underneath, styled to match the plugin's
    brushed-metal knob look (see DelayLookAndFeel::drawRotarySlider). */
class RotaryKnob : public juce::Component
{
public:
    RotaryKnob (const juce::String& labelText, DelayLookAndFeel& lnf);

    void resized() override;
    void paint (juce::Graphics&) override;

    juce::Slider slider;

private:
    juce::String label;
    DelayLookAndFeel& lookAndFeel;
};
