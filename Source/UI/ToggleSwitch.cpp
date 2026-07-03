#include "ToggleSwitch.h"

ToggleSwitch::ToggleSwitch (juce::AudioProcessorValueTreeState& state, const juce::String& paramID,
                             juce::String leftLabel, juce::String rightLabel)
    : leftText (std::move (leftLabel)), rightText (std::move (rightLabel)), apvts (state)
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

void ToggleSwitch::resized()
{
    const int switchWidth = 32, switchHeight = 17;
    switchBounds = juce::Rectangle<int> (0, 0, switchWidth, switchHeight)
                       .withCentre (getLocalBounds().getCentre());
}

void ToggleSwitch::mouseUp (const juce::MouseEvent& e)
{
    if (! e.mouseWasDraggedSinceMouseDown())
    {
        attachment->setValueAsCompleteGesture (isOn ? 0.0f : 1.0f);
    }
}

void ToggleSwitch::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();

    const auto font = DelayLookAndFeel::getUIFont (9.0f, juce::Font::bold);
    g.setFont (font);

    const int labelWidth = 58;
    auto leftArea  = bounds.removeFromLeft (labelWidth);
    auto rightArea = bounds.removeFromRight (labelWidth);

    g.setColour (isOn ? DelayColours::labelGrey : DelayColours::textDark);
    g.drawText (leftText, leftArea, juce::Justification::centredRight);

    g.setColour (isOn ? DelayColours::textDark : DelayColours::labelGrey);
    g.drawText (rightText, rightArea, juce::Justification::centredLeft);

    // Pill track
    auto pill = switchBounds.toFloat();
    g.setColour (isOn ? DelayColours::offToggle : DelayColours::purpleToggle);
    g.fillRoundedRectangle (pill, pill.getHeight() * 0.5f);

    // Handle
    const float handleSize = pill.getHeight() - 4.0f;
    const float handleX = isOn ? (pill.getRight() - handleSize - 2.0f) : (pill.getX() + 2.0f);
    g.setColour (juce::Colours::white);
    g.fillEllipse (handleX, pill.getY() + 2.0f, handleSize, handleSize);
}
