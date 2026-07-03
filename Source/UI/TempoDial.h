#pragma once
#include <JuceHeader.h>
#include "DelayLookAndFeel.h"

/** Circular tempo-division selector: drag vertically to step through note
    divisions (1/32 ... 1/1). Draws a progress ring + centred text, matching
    the HTML reference's "tempo-control" widget. */
class TempoDial : public juce::Component
{
public:
    explicit TempoDial (juce::AudioProcessorValueTreeState& state, const juce::String& paramID);
    ~TempoDial() override;

    void paint (juce::Graphics&) override;
    void mouseDown (const juce::MouseEvent&) override;
    void mouseDrag (const juce::MouseEvent&) override;
    void mouseUp (const juce::MouseEvent&) override;

private:
    void setIndex (int newIndex, bool notifyHost);

    juce::AudioProcessorValueTreeState& apvts;
    juce::RangedAudioParameter* parameter = nullptr;
    std::unique_ptr<juce::ParameterAttachment> attachment;

    int currentIndex = 2;
    int dragStartIndex = 2;
    int dragStartY = 0;
    bool isDragging = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TempoDial)
};
