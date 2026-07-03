# Delay — JUCE VST3 Plugin

A tempo-synced stereo delay plugin, built with **JUCE 7.0.12** (CMake), reproducing
the UI from the original HTML reference: black wireframe-cube visualizer,
brushed-metal knobs, LED VU meters, and a Studio/Creative mode switch.

## What's actually inside

This isn't just a UI skin — it's a working audio effect:

| Control      | What it does                                                              |
|--------------|----------------------------------------------------------------------------|
| **Tempo**    | Delay time synced to host BPM, note divisions 1/32 – 1/1 (drag up/down)   |
| **Regen**    | Feedback amount                                                          |
| **Mix**      | Dry/wet blend                                                            |
| **Output**   | Output trim, ±24 dB / +12 dB                                             |
| **Studio / Creative** | Clean delay vs. a saturated, slightly hotter feedback path       |
| **Auto Gain**| Compensates output level as feedback increases                          |
| **Brightness** | Darkens/brightens the repeats (one-pole filter in the feedback loop)   |
| **Color**    | Subtle LFO-modulated delay time (chorus-like movement on the repeats)   |
| **Sparkle**  | Gentle harmonic saturation on the repeats                                |
| **IN / OUT** | Real peak-metering LED ladders                                          |

## Building locally

Requires CMake ≥ 3.22 and a C++17 compiler. JUCE 7.0.12 is fetched automatically
via `FetchContent` — no submodules, no Projucer.

```bash
cmake -B build
cmake --build build --config Release --target DelayVST_VST3
```

The built plugin will be under `build/DelayVST_artefacts/Release/VST3/Delay.vst3`.

**Windows / Visual Studio:**
```bash
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target DelayVST_VST3
```

## Building via GitHub Actions

Push this repo to GitHub and the included workflow (`.github/workflows/build.yml`)
will build a Windows VST3 automatically on `windows-latest`, using the exact same
CMake configuration, and upload it as a downloadable build artifact named
**Delay-VST3-Windows** — no local Windows machine or DAW needed to produce the binary.

To trigger manually: go to the **Actions** tab → **Build VST3 (Windows)** →
**Run workflow**.

## Project layout

```
CMakeLists.txt                 Plugin target definition, FetchContent for JUCE
Source/
  PluginProcessor.h/.cpp       Audio engine (delay line, feedback, filters, metering)
  PluginEditor.h/.cpp          Top-level UI layout
  UI/
    DelayLookAndFeel.h/.cpp    Shared colours, fonts, custom rotary-knob painting
    RotaryKnob.h/.cpp          Regen / Mix / Output knobs
    TempoDial.h/.cpp           Tempo-division selector with progress ring
    ToggleSwitch.h/.cpp        Studio/Creative switch
    AutoGainIndicator.h/.cpp   Auto Gain LED toggle
    IconToggleButton.h/.cpp    Brightness / Color / Sparkle buttons
    VUMeter.h/.cpp             LED-ladder level meter
    WireframeVisualizer.h/.cpp Animated cube visualizer (top panel)
.github/workflows/build.yml    Windows CI build
```

## Notes

- Only the VST3 format is built (see `FORMATS VST3` in `CMakeLists.txt`). Add
  `AU`, `Standalone`, etc. to that list if you want other formats too.
- `PLUGIN_MANUFACTURER_CODE` / `PLUGIN_CODE` / `COMPANY_NAME` in `CMakeLists.txt`
  are placeholders — change them before distributing the plugin.
- This project was validated by compiling it end-to-end (Debug and Release/LTO)
  against JUCE 7.0.12 prior to delivery, so the Windows CI build should succeed
  on the first run.
