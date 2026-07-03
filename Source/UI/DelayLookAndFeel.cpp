#include "DelayLookAndFeel.h"

DelayLookAndFeel::DelayLookAndFeel()
{
    setColour (juce::Slider::thumbColourId, DelayColours::cyan);
    setColour (juce::ToggleButton::tickColourId, DelayColours::cyan);
}

juce::Font DelayLookAndFeel::getUIFont (float height, juce::Font::FontStyleFlags style)
{
   #if JUCE_WINDOWS
    return juce::Font ("Segoe UI", height, style);
   #elif JUCE_MAC
    return juce::Font ("Helvetica Neue", height, style);
   #else
    return juce::Font (height, style);
   #endif
}

void DelayLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                          float sliderPosProportional, float rotaryStartAngle,
                                          float rotaryEndAngle, juce::Slider&)
{
    auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) width, (float) height).reduced (2.0f);
    const auto centre = bounds.getCentre();
    const auto outerRadius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;

    // Dashed track ring (matches CSS .knob-track dashed border)
    {
        juce::Path dashedRing;
        dashedRing.addEllipse (centre.x - outerRadius, centre.y - outerRadius, outerRadius * 2.0f, outerRadius * 2.0f);
        float dashLengths[] = { 3.0f, 4.0f };
        juce::Path dashed;
        juce::PathStrokeType (1.0f).createDashedStroke (dashed, dashedRing, dashLengths, 2);
        g.setColour (juce::Colour (0x73a0aec0));
        g.fillPath (dashed);
    }

    const float knobRadius = outerRadius * 0.82f;
    const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    // Soft drop shadow beneath the knob disc
    {
        juce::Path shadowPath;
        shadowPath.addEllipse (centre.x - knobRadius, centre.y - knobRadius + 1.5f, knobRadius * 2.0f, knobRadius * 2.0f);
        g.setColour (juce::Colours::black.withAlpha (0.15f));
        g.fillPath (shadowPath);
    }

    // Knob disc — radial gradient simulating the CSS "circle at 35% 35%" highlight
    {
        juce::Point<float> highlightPoint = centre.translated (-knobRadius * 0.3f, -knobRadius * 0.3f);
        juce::ColourGradient grad (DelayColours::knobHighlight, highlightPoint,
                                    DelayColours::knobShadowCol, centre.translated (knobRadius, knobRadius),
                                    true);
        g.setGradientFill (grad);
        g.fillEllipse (centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);

        g.setColour (DelayColours::knobBorder);
        g.drawEllipse (centre.x - knobRadius, centre.y - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f, 1.2f);
    }

    // Pointer dot
    {
        const float pointerDist = knobRadius * 0.72f;
        juce::Point<float> pointerPos = centre.getPointOnCircumference (pointerDist, angle);
        const float dotSize = juce::jmax (3.0f, knobRadius * 0.11f);
        g.setColour (DelayColours::textDark.withAlpha (0.85f));
        g.fillEllipse (pointerPos.x - dotSize * 0.5f, pointerPos.y - dotSize * 0.5f, dotSize, dotSize);
    }
}
