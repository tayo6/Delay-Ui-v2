#include "WireframeVisualizer.h"

WireframeVisualizer::WireframeVisualizer()
{
    startTimerHz (30);
}

WireframeVisualizer::~WireframeVisualizer()
{
    stopTimer();
}

void WireframeVisualizer::timerCallback()
{
    phase += 1.0 / 30.0;
    if (phase > 1000.0)
        phase = 0.0;
    repaint();
}

// Draws one isometric wireframe cube. `points` are the 4 top-face and 4
// bottom-face vertices, defined in the original SVG's 380x160 viewBox space,
// which we scale/translate to fit the component. `floatOffset` and `glowAlpha`
// implement the CSS "floatBreath" keyframe animation (translateY + drop-shadow).
void WireframeVisualizer::drawCube (juce::Graphics& g, juce::Rectangle<float> viewport,
                                     const std::array<juce::Point<float>, 8>& points,
                                     float breathPhase) const
{
    const float scaleX = viewport.getWidth() / 380.0f;
    const float scaleY = viewport.getHeight() / 160.0f;
    const float scale = juce::jmin (scaleX, scaleY);

    const float floatOffset = -4.0f * breathPhase * scale;
    const float glowAlpha = juce::jmap (breathPhase, 0.0f, 1.0f, 0.5f, 0.85f);

    juce::AffineTransform transform = juce::AffineTransform::scale (scale)
                                           .translated (viewport.getX(), viewport.getY() + floatOffset);

    std::array<juce::Point<float>, 8> tp;
    for (int i = 0; i < 8; ++i)
        tp[(size_t) i] = points[(size_t) i].transformedBy (transform);

    juce::Path outline;
    // Top face
    outline.startNewSubPath (tp[0]);
    outline.lineTo (tp[1]); outline.lineTo (tp[2]); outline.lineTo (tp[3]);
    outline.closeSubPath();
    // Bottom face
    outline.startNewSubPath (tp[4]);
    outline.lineTo (tp[5]); outline.lineTo (tp[6]); outline.lineTo (tp[7]);
    outline.closeSubPath();
    // Vertical pillars
    outline.startNewSubPath (tp[0]); outline.lineTo (tp[4]);
    outline.startNewSubPath (tp[1]); outline.lineTo (tp[5]);
    outline.startNewSubPath (tp[2]); outline.lineTo (tp[6]);
    outline.startNewSubPath (tp[3]); outline.lineTo (tp[7]);

    // Soft glow: a few widened, low-alpha passes beneath the crisp line.
    for (int i = 3; i >= 1; --i)
    {
        g.setColour (DelayColours::cyan.withAlpha (glowAlpha * 0.10f));
        g.strokePath (outline, juce::PathStrokeType (1.6f * scale + (float) i * 1.8f,
                                                       juce::PathStrokeType::curved,
                                                       juce::PathStrokeType::rounded));
    }

    g.setColour (DelayColours::cyan.withAlpha (juce::jmin (1.0f, glowAlpha + 0.15f)));
    g.strokePath (outline, juce::PathStrokeType (1.7f * scale, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void WireframeVisualizer::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    juce::ColourGradient bg (juce::Colour (0xff090e12), bounds.getTopLeft(),
                              juce::Colour (0xff050709), bounds.getBottomLeft(), false);
    g.setGradientFill (bg);
    g.fillRect (bounds);

    // 6-second ease-in-out-alternate breathing cycle, matching the CSS animation.
    const double cycleSeconds = 6.0;
    const double t = std::fmod (phase, cycleSeconds) / cycleSeconds;
    const float breathA = (float) (0.5 - 0.5 * std::cos (t * juce::MathConstants<double>::twoPi));

    // Second cube runs 3s out of phase (CSS animation-delay: -3s).
    const double t2 = std::fmod (phase + cycleSeconds * 0.5, cycleSeconds) / cycleSeconds;
    const float breathB = (float) (0.5 - 0.5 * std::cos (t2 * juce::MathConstants<double>::twoPi));

    using P = juce::Point<float>;

    // Large left cube (SVG coordinates from the reference markup).
    const std::array<P, 8> largeCube {
        P (150, 38), P (185, 58), P (150, 78), P (115, 58),   // top face
        P (150, 96), P (185, 116), P (150, 136), P (115, 116) // bottom face
    };

    // Small right cube.
    const std::array<P, 8> smallCube {
        P (255, 54), P (276, 66), P (255, 78), P (234, 66),  // top face
        P (255, 86), P (276, 98), P (255, 110), P (234, 98)  // bottom face
    };

    drawCube (g, bounds, largeCube, breathA);
    drawCube (g, bounds, smallCube, breathB);
}
