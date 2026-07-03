cmake_minimum_required(VERSION 3.22)

project(DelayVST VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "" FORCE)

# ---------------------------------------------------------------------------
# Pull in JUCE 7.0.12 (pinned) via FetchContent so CI needs no submodules.
# ---------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        7.0.12
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

# ---------------------------------------------------------------------------
# Plugin target
# ---------------------------------------------------------------------------
juce_add_plugin(DelayVST
    COMPANY_NAME                "Wireframe Audio"
    BUNDLE_ID                   "com.wireframeaudio.delayvst"
    PLUGIN_MANUFACTURER_CODE    Wrfa
    PLUGIN_CODE                 Dlv1
    FORMATS                     VST3
    PRODUCT_NAME                "Delay"
    VST3_CATEGORIES             "Fx" "Delay"
    IS_SYNTH                    FALSE
    NEEDS_MIDI_INPUT            FALSE
    NEEDS_MIDI_OUTPUT           FALSE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD     FALSE
)

juce_generate_juce_header(DelayVST)

target_sources(DelayVST PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/UI/DelayLookAndFeel.cpp
    Source/UI/RotaryKnob.cpp
    Source/UI/TempoDial.cpp
    Source/UI/ToggleSwitch.cpp
    Source/UI/AutoGainIndicator.cpp
    Source/UI/IconToggleButton.cpp
    Source/UI/VUMeter.cpp
    Source/UI/WireframeVisualizer.cpp
)

target_compile_definitions(DelayVST PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
)

target_link_libraries(DelayVST
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
