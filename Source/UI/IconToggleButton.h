#pragma once
#include <JuceHeader.h>
#include "DelayLookAndFeel.h"

enum class IconType { Brightness, Color, Sparkle };

/** Icon + caption toggle button, redrawing one of the three custom vector
    icons from the HTML reference (striped circle / rosette / star cluster). */
class IconToggleButton : public juce::Component
{
public:
    IconToggleButton (juce::AudioProcessorValueTreeState& state, const juce::String& paramID,
                       IconType icon, juce::String caption);

    void paint (juce::Graphics&) override;
    void mouseUp (const juce::MouseEvent&) override;
    void mouseEnter (const juce::MouseEvent&) override { hovering = true; repaint(); }
    void mouseExit (const juce::MouseEvent&) override  { hovering = false; repaint(); }

private:
    void drawBrightnessIcon (juce::Graphics&, juce::Rectangle<float> area, juce::Colour) const;
    void drawColorIcon (juce::Graphics&, juce::Rectangle<float> area, juce::Colour) const;
    void drawSparkleIcon (juce::Graphics&, juce::Rectangle<float> area, juce::Colour) const;

    juce::AudioProcessorValueTreeState& apvts;
    std::unique_ptr<juce::ParameterAttachment> attachment;
    juce::RangedAudioParameter* parameter = nullptr;

    IconType iconType;
    juce::String captionText;
    bool isOn = false;
    bool hovering = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IconToggleButton)
};
