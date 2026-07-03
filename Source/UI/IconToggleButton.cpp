#include "IconToggleButton.h"

IconToggleButton::IconToggleButton (juce::AudioProcessorValueTreeState& state, const juce::String& paramID,
                                     IconType icon, juce::String caption)
    : apvts (state), iconType (icon), captionText (std::move (caption))
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
    setMouseCursor (juce::MouseCursor::PointingHandCursor);
}

void IconToggleButton::mouseUp (const juce::MouseEvent& e)
{
    if (! e.mouseWasDraggedSinceMouseDown())
        attachment->setValueAsCompleteGesture (isOn ? 0.0f : 1.0f);
}

void IconToggleButton::drawBrightnessIcon (juce::Graphics& g, juce::Rectangle<float> area, juce::Colour colour) const
{
    const auto centre = area.getCentre();
    const float r = area.getWidth() * 0.35f;

    juce::Path clip;
    clip.addEllipse (centre.x - r, centre.y - r, r * 2.0f, r * 2.0f);

    juce::Graphics::ScopedSaveState save (g);
    g.reduceClipRegion (clip);

    g.setColour (colour);
    const int numStripes = 9;
    for (int i = 0; i < numStripes; ++i)
    {
        const float y = area.getY() + area.getHeight() * ((float) i + 0.5f) / (float) numStripes;
        g.drawLine (area.getX(), y, area.getRight(), y, 1.4f);
    }

    g.drawLine (centre.x, area.getY(), centre.x, area.getBottom(), 1.6f);
    g.setColour (colour);
    g.drawEllipse (centre.x - r, centre.y - r, r * 2.0f, r * 2.0f, 1.4f);
}

void IconToggleButton::drawColorIcon (juce::Graphics& g, juce::Rectangle<float> area, juce::Colour colour) const
{
    const auto centre = area.getCentre();
    const float r = area.getWidth() * 0.155f;
    const float petalDist = r;

    g.setColour (colour);
    g.drawEllipse (centre.x - r, centre.y - r, r * 2.0f, r * 2.0f, 1.3f);

    for (int i = 0; i < 6; ++i)
    {
        const float angle = juce::MathConstants<float>::twoPi * (float) i / 6.0f;
        const auto p = centre.getPointOnCircumference (petalDist, angle);
        g.drawEllipse (p.x - r, p.y - r, r * 2.0f, r * 2.0f, 1.3f);
    }
}

static juce::Path makeSparkleStar (float radius)
{
    juce::Path p;
    p.startNewSubPath (0.0f, -radius);
    p.quadraticTo (0.0f, 0.0f, radius, 0.0f);
    p.quadraticTo (0.0f, 0.0f, 0.0f, radius);
    p.quadraticTo (0.0f, 0.0f, -radius, 0.0f);
    p.quadraticTo (0.0f, 0.0f, 0.0f, -radius);
    p.closeSubPath();
    return p;
}

void IconToggleButton::drawSparkleIcon (juce::Graphics& g, juce::Rectangle<float> area, juce::Colour colour) const
{
    struct StarSpec { float relX, relY, relSize; };
    static const StarSpec stars[] = {
        { 0.50f, 0.28f, 0.24f },
        { 0.33f, 0.47f, 0.19f },
        { 0.67f, 0.50f, 0.20f },
        { 0.48f, 0.72f, 0.26f },
        { 0.24f, 0.24f, 0.11f },
        { 0.76f, 0.22f, 0.11f },
        { 0.22f, 0.74f, 0.11f },
        { 0.78f, 0.72f, 0.12f },
    };

    g.setColour (colour);
    const float base = area.getWidth();
    for (auto& s : stars)
    {
        auto star = makeSparkleStar (base * s.relSize * 0.5f);
        star.applyTransform (juce::AffineTransform::translation (area.getX() + base * s.relX,
                                                                   area.getY() + base * s.relY));
        g.fillPath (star);
    }
}

void IconToggleButton::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto captionArea = bounds.removeFromBottom (16.0f);
    auto iconArea = bounds.reduced (4.0f);

    const auto colour = isOn ? DelayColours::cyan
                              : (hovering ? DelayColours::cyan.withAlpha (0.7f) : DelayColours::textDark.withAlpha (0.85f));

    switch (iconType)
    {
        case IconType::Brightness: drawBrightnessIcon (g, iconArea, colour); break;
        case IconType::Color:      drawColorIcon (g, iconArea, colour); break;
        case IconType::Sparkle:    drawSparkleIcon (g, iconArea, colour); break;
    }

    g.setFont (DelayLookAndFeel::getUIFont (8.0f, juce::Font::bold));
    g.setColour (isOn ? DelayColours::cyan : DelayColours::labelGrey);
    g.drawText (captionText, captionArea.toNearestInt(), juce::Justification::centred);
}
