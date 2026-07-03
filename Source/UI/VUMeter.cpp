#include "VUMeter.h"

VUMeter::VUMeter (juce::String labelText) : label (std::move (labelText))
{
    startTimerHz (30);
}

VUMeter::~VUMeter()
{
    stopTimer();
}

void VUMeter::setLevelSources (const std::atomic<float>* channelL, const std::atomic<float>* channelR)
{
    levelSourceL = channelL;
    levelSourceR = channelR;
}

void VUMeter::timerCallback()
{
    float target = 0.0f;
    if (levelSourceL != nullptr) target = juce::jmax (target, levelSourceL->load());
    if (levelSourceR != nullptr) target = juce::jmax (target, levelSourceR->load());

    // Fast attack, slower release — classic VU ballistics.
    if (target > smoothedLevel)
        smoothedLevel += (target - smoothedLevel) * 0.5f;
    else
        smoothedLevel += (target - smoothedLevel) * 0.15f;

    repaint();
}

void VUMeter::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    auto labelArea = bounds.removeFromBottom (12);
    auto ledArea = bounds.reduced (0, 2);

    g.setColour (juce::Colour (0xffe2e8f0));
    g.fillRoundedRectangle (ledArea.toFloat(), 2.0f);

    const int litCount = juce::jlimit (0, totalLEDs, juce::roundToInt (smoothedLevel * (float) totalLEDs));

    const float padding = 2.0f;
    const float gap = 1.5f;
    const float usableHeight = (float) ledArea.getHeight() - padding * 2.0f;
    const float ledHeight = (usableHeight - gap * (float) (totalLEDs - 1)) / (float) totalLEDs;
    const float ledWidth = (float) ledArea.getWidth() - padding * 2.0f;

    for (int i = 0; i < totalLEDs; ++i)
    {
        const float y = (float) ledArea.getBottom() - padding - ledHeight - (float) i * (ledHeight + gap);
        const float x = (float) ledArea.getX() + padding;

        juce::Colour c = DelayColours::ledOff;
        bool lit = i < litCount;

        if (lit)
        {
            if (i < 16)      c = DelayColours::ledGreen;
            else if (i < 19) c = DelayColours::ledBlue;
            else if (i < 22) c = DelayColours::ledOrange;
            else             c = DelayColours::ledRed;

            g.setColour (c.withAlpha (0.45f));
            g.fillRoundedRectangle (x - 0.5f, y - 0.5f, ledWidth + 1.0f, ledHeight + 1.0f, 0.5f);
        }

        g.setColour (lit ? c : DelayColours::ledOff);
        g.fillRoundedRectangle (x, y, ledWidth, ledHeight, 0.5f);
    }

    g.setFont (DelayLookAndFeel::getUIFont (8.0f, juce::Font::bold));
    g.setColour (DelayColours::labelGrey);
    g.drawText (label, labelArea, juce::Justification::centred);
}
