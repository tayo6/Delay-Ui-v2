#pragma once
#include <JuceHeader.h>
#include "DelayLookAndFeel.h"

/** Animated isometric wireframe-cube visualizer that reproduces the HTML
    reference's SVG "floatBreath" animation: two cube outlines gently
    float/glow out of phase with one another. Purely decorative — driven by
    a 6-second breathing cycle, no audio analysis required to match the
    reference (which itself uses a fixed CSS animation, not live audio). */
class WireframeVisualizer : public juce::Component, private juce::Timer
{
public:
    WireframeVisualizer();
    ~WireframeVisualizer() override;

    void paint (juce::Graphics&) override;

private:
    void timerCallback() override;
    void drawCube (juce::Graphics&, juce::Rectangle<float> viewport,
                    const std::array<juce::Point<float>, 8>& normalisedPoints,
                    float breathPhase) const;

    double phase = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WireframeVisualizer)
};
