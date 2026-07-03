#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/DelayLookAndFeel.h"
#include "UI/RotaryKnob.h"
#include "UI/TempoDial.h"
#include "UI/ToggleSwitch.h"
#include "UI/AutoGainIndicator.h"
#include "UI/IconToggleButton.h"
#include "UI/VUMeter.h"
#include "UI/WireframeVisualizer.h"

class DelayVSTAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit DelayVSTAudioProcessorEditor (DelayVSTAudioProcessor&);
    ~DelayVSTAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    DelayVSTAudioProcessor& audioProcessor;
    DelayLookAndFeel lookAndFeel;

    // Top panel
    WireframeVisualizer visualizer;
    juce::Label logoLabel;

    // Mid bar
    ToggleSwitch modeSwitch;
    AutoGainIndicator autoGainIndicator;

    // Left controls
    TempoDial tempoDial;
    juce::Label tempoCaption;
    RotaryKnob regenKnob;
    RotaryKnob mixKnob;
    IconToggleButton brightnessButton;
    IconToggleButton colorButton;
    IconToggleButton sparkleButton;

    // Right controls
    VUMeter inMeter, outMeter;
    RotaryKnob outputKnob;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> regenAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayVSTAudioProcessorEditor)
};
