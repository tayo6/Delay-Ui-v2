#include "PluginEditor.h"

DelayVSTAudioProcessorEditor::DelayVSTAudioProcessorEditor (DelayVSTAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
      modeSwitch (p.apvts, DelayVSTAudioProcessor::modeParamID, "STUDIO", "CREATIVE"),
      autoGainIndicator (p.apvts, DelayVSTAudioProcessor::autoGainParamID),
      tempoDial (p.apvts, DelayVSTAudioProcessor::tempoParamID),
      regenKnob ("REGEN", lookAndFeel),
      mixKnob ("MIX", lookAndFeel),
      brightnessButton (p.apvts, DelayVSTAudioProcessor::brightnessParamID, IconType::Brightness, "BRIGHTNESS"),
      colorButton (p.apvts, DelayVSTAudioProcessor::colorParamID, IconType::Color, "COLOR"),
      sparkleButton (p.apvts, DelayVSTAudioProcessor::sparkleParamID, IconType::Sparkle, "SPARKLE"),
      inMeter ("IN"), outMeter ("OUT"),
      outputKnob ("OUTPUT", lookAndFeel)
{
    setLookAndFeel (&lookAndFeel);

    addAndMakeVisible (visualizer);

    logoLabel.setText ("DELAY  \u25BC", juce::dontSendNotification);
    logoLabel.setFont (DelayLookAndFeel::getUIFont (13.0f, juce::Font::bold).withExtraKerningFactor (0.12f));
    logoLabel.setColour (juce::Label::textColourId, DelayColours::cyan);
    logoLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (logoLabel);

    addAndMakeVisible (modeSwitch);
    addAndMakeVisible (autoGainIndicator);

    addAndMakeVisible (tempoDial);
    tempoCaption.setText ("TEMPO", juce::dontSendNotification);
    tempoCaption.setFont (DelayLookAndFeel::getUIFont (9.0f, juce::Font::bold));
    tempoCaption.setColour (juce::Label::textColourId, DelayColours::labelGrey);
    tempoCaption.setJustificationType (juce::Justification::centred);
    addAndMakeVisible (tempoCaption);

    addAndMakeVisible (regenKnob);
    addAndMakeVisible (mixKnob);

    addAndMakeVisible (brightnessButton);
    addAndMakeVisible (colorButton);
    addAndMakeVisible (sparkleButton);

    addAndMakeVisible (inMeter);
    addAndMakeVisible (outMeter);
    inMeter.setLevelSources (&audioProcessor.inputLevelL, &audioProcessor.inputLevelR);
    outMeter.setLevelSources (&audioProcessor.outputLevelL, &audioProcessor.outputLevelR);

    addAndMakeVisible (outputKnob);

    regenAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        p.apvts, DelayVSTAudioProcessor::regenParamID, regenKnob.slider);
    mixAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        p.apvts, DelayVSTAudioProcessor::mixParamID, mixKnob.slider);
    outputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        p.apvts, DelayVSTAudioProcessor::outputParamID, outputKnob.slider);

    setResizable (false, false);
    setSize (530, 440);
}

DelayVSTAudioProcessorEditor::~DelayVSTAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void DelayVSTAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (DelayColours::panelLight);

    // Mid bar background + divider
    auto bounds = getLocalBounds();
    bounds.removeFromTop (160);
    auto midBar = bounds.removeFromTop (38);
    g.setColour (DelayColours::midBar);
    g.fillRect (midBar);
    g.setColour (DelayColours::divider);
    g.fillRect (midBar.withTop (midBar.getBottom() - 1));

    // Bottom panel background
    g.setColour (DelayColours::panelBottom);
    g.fillRect (bounds);

    // Vertical divider between left/right controls
    const int leftWidth = juce::roundToInt (530.0f * 0.78f);
    auto dividerX = bounds.getX() + leftWidth;
    g.setColour (DelayColours::divider);
    g.fillRect (juce::Rectangle<int> (dividerX, bounds.getY(), 1, bounds.getHeight()));

    // Outer frame border to match the CSS panel border
    g.setColour (DelayColours::panelBorder);
    g.drawRect (getLocalBounds(), 1);
}

void DelayVSTAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    // ---------------- Top panel ----------------
    auto topPanel = bounds.removeFromTop (160);
    visualizer.setBounds (topPanel);
    auto header = topPanel.reduced (20, 16);
    logoLabel.setBounds (header.removeFromTop (18).removeFromLeft (120));

    // ---------------- Mid bar ----------------
    auto midBar = bounds.removeFromTop (38).reduced (24, 0);
    modeSwitch.setBounds (midBar.removeFromLeft (140));
    autoGainIndicator.setBounds (midBar.removeFromRight (70));

    // ---------------- Bottom panel ----------------
    const int leftWidth = juce::roundToInt ((float) getWidth() * 0.78f);
    auto leftControls  = bounds.removeFromLeft (leftWidth).reduced (24, 0);
    leftControls.removeFromTop (24);
    leftControls.removeFromBottom (20);

    auto rightControls = bounds.reduced (0, 20);
    rightControls.removeFromLeft (1); // account for divider line

    // Knobs row (top of left controls)
    auto knobsRow = leftControls.removeFromTop (82);
    const int tempoW = 82, regenW = 68, mixW = 92;
    auto tempoArea = knobsRow.removeFromLeft (tempoW);
    tempoDial.setBounds (tempoArea.removeFromTop (66).withSizeKeepingCentre (66, 66));
    tempoCaption.setBounds (tempoArea);

    knobsRow.removeFromLeft ((knobsRow.getWidth() - regenW - mixW) / 3); // spacer approximating justify-content:space-between
    auto regenArea = knobsRow.removeFromLeft (regenW);
    regenKnob.setBounds (regenArea);

    auto mixArea = knobsRow.removeFromRight (mixW);
    mixKnob.setBounds (mixArea);

    // Buttons row (bottom of left controls)
    auto buttonsRow = leftControls.removeFromBottom (74);
    const int btnW = buttonsRow.getWidth() / 3;
    brightnessButton.setBounds (buttonsRow.removeFromLeft (btnW));
    colorButton.setBounds (buttonsRow.removeFromLeft (btnW));
    sparkleButton.setBounds (buttonsRow);

    // Right controls: meters on top, output knob at bottom
    auto metersArea = rightControls.removeFromTop (122);
    const int meterW = (metersArea.getWidth() - 14) / 2;
    auto inArea = metersArea.removeFromLeft (meterW);
    metersArea.removeFromLeft (14);
    auto outArea = metersArea.removeFromLeft (meterW);
    inMeter.setBounds (inArea);
    outMeter.setBounds (outArea);

    outputKnob.setBounds (rightControls.removeFromBottom (78).withSizeKeepingCentre (68, 78));
}
