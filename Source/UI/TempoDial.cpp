#include "TempoDial.h"
#include "../PluginProcessor.h"

TempoDial::TempoDial (juce::AudioProcessorValueTreeState& state, const juce::String& paramID)
    : apvts (state)
{
    parameter = apvts.getParameter (paramID);
    jassert (parameter != nullptr);

    attachment = std::make_unique<juce::ParameterAttachment> (
        *parameter,
        [this] (float newValue)
        {
            currentIndex = juce::roundToInt (newValue);
            repaint();
        },
        nullptr);

    currentIndex = juce::roundToInt (parameter->convertFrom0to1 (parameter->getValue()));
    attachment->sendInitialUpdate();
    setMouseCursor (juce::MouseCursor::UpDownResizeCursor);
}

TempoDial::~TempoDial() = default;

void TempoDial::setIndex (int newIndex, bool notifyHost)
{
    newIndex = juce::jlimit (0, DelayVSTAudioProcessor::numTempoDivisions - 1, newIndex);
    if (newIndex == currentIndex)
        return;

    currentIndex = newIndex;
    if (notifyHost)
        attachment->setValueAsPartOfGesture ((float) currentIndex);
    repaint();
}

void TempoDial::mouseDown (const juce::MouseEvent& e)
{
    isDragging = true;
    dragStartY = e.getPosition().y;
    dragStartIndex = currentIndex;
    attachment->beginGesture();
}

void TempoDial::mouseDrag (const juce::MouseEvent& e)
{
    if (! isDragging)
        return;

    const int deltaY = dragStartY - e.getPosition().y;
    const int steps = deltaY / 18; // step spacing sensitivity
    setIndex (dragStartIndex + steps, true);
}

void TempoDial::mouseUp (const juce::MouseEvent&)
{
    if (isDragging)
    {
        isDragging = false;
        attachment->endGesture();
    }
}

void TempoDial::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced (2.0f);
    const auto centre = bounds.getCentre();
    const float radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const float ringThickness = 3.5f;

    // Background ring
    juce::Path bgRing;
    bgRing.addCentredArc (centre.x, centre.y, radius, radius, 0.0f,
                           0.0f, juce::MathConstants<float>::twoPi, true);
    g.setColour (juce::Colour (0xffe2e8f0));
    g.strokePath (bgRing, juce::PathStrokeType (ringThickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Progress ring (mirrors the SVG stroke-dashoffset animation, starting at 12 o'clock)
    const float progress = (float) (currentIndex + 1) / (float) DelayVSTAudioProcessor::numTempoDivisions;
    juce::Path progressRing;
    progressRing.addCentredArc (centre.x, centre.y, radius, radius, 0.0f,
                                 0.0f, progress * juce::MathConstants<float>::twoPi, true);
    g.setColour (DelayColours::cyan);
    g.strokePath (progressRing, juce::PathStrokeType (ringThickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Centre text
    g.setFont (DelayLookAndFeel::getUIFont (14.0f, juce::Font::plain));
    g.setColour (DelayColours::textDark);
    g.drawText (DelayVSTAudioProcessor::tempoDivisionLabels[currentIndex], getLocalBounds(), juce::Justification::centred);
}
