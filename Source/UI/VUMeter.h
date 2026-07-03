#pragma once
#include <JuceHeader.h>
#include "DelayLookAndFeel.h"

/** A single LED-column level meter (24 segments: green/blue/orange/red bands),
    matching the HTML reference's ".meter-leds" widget. Polls an external level
    source via a Timer for smooth, host-independent animation. */
class VUMeter : public juce::Component, private juce::Timer
{
public:
    explicit VUMeter (juce::String labelText);
    ~VUMeter() override;

    void paint (juce::Graphics&) override;

    /** Provide the two channel level sources this meter should follow (peak, 0-1+). */
    void setLevelSources (const std::atomic<float>* channelL, const std::atomic<float>* channelR);

private:
    void timerCallback() override;

    juce::String label;
    const std::atomic<float>* levelSourceL = nullptr;
    const std::atomic<float>* levelSourceR = nullptr;
    float smoothedLevel = 0.0f;

    static constexpr int totalLEDs = 24;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VUMeter)
};
