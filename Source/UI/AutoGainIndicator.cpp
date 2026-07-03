#include "AutoGainIndicator.h"

AutoGainIndicator::AutoGainIndicator (juce::AudioProcessorValueTreeState& state, const juce::String& paramID)
    : apvts (state)
{
    parameter = apvts.getParameter (paramID);
    jassert (parameter != nullptr);

    attachment = std::make_unique<juce::ParameterAttachment> (
        *parameter,
        [this] (float newValue)
        {
            isOn = newValue > 0.5f;
            repaint();
        },
        nullptr);

    attachment->sendInitialUpdate();
}

void AutoGainIndicator::mouseUp (const juce::MouseEvent& e)
{
    if (! e.mouseWasDraggedSinceMouseDown())
        attachment->setValueAsCompleteGesture (isOn ? 0.0f : 1.0f);
}

void AutoGainIndicator::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    auto ledBounds = bounds.removeFromRight (18).withSizeKeepingCentre (18, 11).toFloat();

    if (isOn)
    {
        g.setColour (DelayColours::ledGreen.withAlpha (0.35f));
        g.fillRoundedRectangle (ledBounds.expanded (2.0f), 3.0f);
    }

    g.setColour (isOn ? DelayColours::ledGreen : juce::Colour (0xffa0aec0));
    g.fillRoundedRectangle (ledBounds, 2.0f);

    bounds.removeFromRight (8); // gap
    g.setFont (DelayLookAndFeel::getUIFont (8.0f, juce::Font::bold));
    g.setColour (juce::Colour (0xff4a5568));
    auto textBounds = bounds;
    auto topLine = textBounds.removeFromTop (textBounds.getHeight() / 2);
    g.drawText ("AUTO", topLine, juce::Justification::centredRight);
    g.drawText ("GAIN", textBounds, juce::Justification::centredRight);
}
