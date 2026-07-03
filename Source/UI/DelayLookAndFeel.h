#pragma once
#include <JuceHeader.h>

namespace DelayColours
{
    const juce::Colour panelLight     { 0xfff5f7fa };
    const juce::Colour panelBottom    { 0xfff7f9fa };
    const juce::Colour panelBorder    { 0xffbbc4cc };
    const juce::Colour midBar         { 0xfff1f3f6 };
    const juce::Colour divider        { 0xffe1e4e8 };
    const juce::Colour topPanelTop    { 0xff090e12 };
    const juce::Colour topPanelBottom { 0xff050709 };
    const juce::Colour cyan           { 0xff2ce5c4 };
    const juce::Colour cyanBright     { 0xff2de5c4 };
    const juce::Colour labelGrey      { 0xff718096 };
    const juce::Colour textDark       { 0xff2d3748 };
    const juce::Colour knobBorder     { 0xffb8c8d5 };
    const juce::Colour knobHighlight  { 0xfff1f6fa };
    const juce::Colour knobShadowCol  { 0xffcbdbe7 };
    const juce::Colour purpleToggle   { 0xff8e99fc };
    const juce::Colour offToggle      { 0xffcbd5e0 };
    const juce::Colour ledOff         { 0xffcbd5e0 };
    const juce::Colour ledGreen       { 0xff52ec87 };
    const juce::Colour ledBlue        { 0xff4cdff2 };
    const juce::Colour ledOrange      { 0xfffca33d };
    const juce::Colour ledRed         { 0xffff5e5e };
}

class DelayLookAndFeel : public juce::LookAndFeel_V4
{
public:
    DelayLookAndFeel();

    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                            float sliderPosProportional, float rotaryStartAngle,
                            float rotaryEndAngle, juce::Slider&) override;

    static juce::Font getUIFont (float height, juce::Font::FontStyleFlags style = juce::Font::plain);
};
