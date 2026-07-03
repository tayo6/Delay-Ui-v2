#include "RotaryKnob.h"

RotaryKnob::RotaryKnob (const juce::String& labelText, DelayLookAndFeel& lnf)
    : label (labelText), lookAndFeel (lnf)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setRotaryParameters (juce::degreesToRadians (-135.0f), juce::degreesToRadians (135.0f), true);
    slider.setTextBoxStyle (juce::Slider::NoTextBox, true, 0, 0);
    slider.setLookAndFeel (&lookAndFeel);
    slider.setMouseDragSensitivity (140);
    addAndMakeVisible (slider);
}

void RotaryKnob::resized()
{
    auto bounds = getLocalBounds();
    auto labelArea = bounds.removeFromBottom (16);
    juce::ignoreUnused (labelArea);
    slider.setBounds (bounds);
}

void RotaryKnob::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    auto labelArea = bounds.removeFromBottom (16);

    g.setFont (DelayLookAndFeel::getUIFont (9.0f, juce::Font::bold));
    g.setColour (DelayColours::labelGrey);
    g.drawText (label, labelArea, juce::Justification::centred);
}
