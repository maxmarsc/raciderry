/*
  ==============================================================================

    Identifiers.h
    Created: 21 Aug 2020 8:35:54pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace parameters
{

namespace device
{
    const juce::AudioDeviceManager::AudioDeviceSetup PISOUND_SETUP({
        juce::String("pisound, ; Direct hardware device without any conversions"),
        juce::String("pisound, ; Direct hardware device without any conversions"),
        48000,
        256,
        0,
        true,
        2,
        true
    });
} // namespace device


// namespace ids
// {
//     const juce::Identifier  ATTACK("ATTACK");
//     const juce::Identifier  DECAY("DECAY");
//     const juce::Identifier  SUSTAIN("SUSTAIN");
//     const juce::Identifier  RELEASE("RELEASE");

//     const juce::Identifier  WAVEFORM_RATIO("WAVEFORM RATIO");

//     const juce::Identifier  CUTOFF("CUTOFF");
//     const juce::Identifier  RESONANCE("RESONANCE");
//     const juce::Identifier  DRIVE("DRIVE");
//     const juce::Identifier  ENV_MOD("ENV_MOD");
// }//namespace ids

namespace midiCC
{
    // ADSR
    constexpr int           ATTACK = 73;
    constexpr int           DECAY = 75;
    constexpr int           SUSTAIN = 64;
    constexpr int           RELEASE = 72;

    // OSC
    constexpr int           WAVEFORM_RATIO = 80;    // General purpose 5

    // FILTER
    constexpr int           CUTOFF = 16;            // General purpose 1
    constexpr int           RESONANCE = 17;         // General purpose 2
    constexpr int           FILTER_DRIVE = 18;      // General purpose 3
    constexpr int           ENV_MOD = 19;            // General purpose 4
}

namespace values
{
    // ADR values in seconds
    constexpr double        ATTACK_DEFAULT = 0.1;
    constexpr double        DECAY_DEFAULT = 0.1;
    constexpr double        RELEASE_DEFAULT = 0.1;
    constexpr double        ATTACK_MIN = 0.005;
    constexpr double        DECAY_MIN = 0.005;
    constexpr double        RELEASE_MIN = 0.02;
    constexpr double        ATTACK_MAX = 1.5;
    constexpr double        DECAY_MAX = 5.0;
    constexpr double        RELEASE_MAX = 5.0;

    // Sustain levels btw 0 - 1
    constexpr float        SUSTAIN_DEFAULT = 1.0;
    constexpr float        SUSTAIN_MIN = 0.0;
    constexpr float        SUSTAIN_MAX = 1.0;

    // Ratio btw saw and square wave
    constexpr double        WAFEFORM_RATIO_DEFAULT = 0.5;
    constexpr double        WAFEFORM_RATIO_MIN = 0.0;
    constexpr double        WAFEFORM_RATIO_MAX = 1.0;

    // Limiter values
    constexpr float         LIMITER_RELEASE_MS = 10.0;
    constexpr float         LIMITER_THRESHOLD_DB = -0.1;

    // Filter
    constexpr float         CUTOFF_DEFAULT =  11000.0;   // Hz
    constexpr float         CUTOFF_MIN = 60.0;
    constexpr float         CUTOFF_MAX = 22000.0;
    constexpr float         RESONANCE_DEFAULT = 0.0;     // > 0.
    constexpr float         RESONANCE_MIN = 0.0;
    constexpr float         RESONANCE_MAX = 10.0;
    constexpr float         DRIVE_DEFAULT = 1.0;        // > 1.
    constexpr float         DRIVE_MIN = 1.0;
    constexpr float         DRIVE_MAX = 1.5;
    constexpr float         ENV_MOD_DEFAULT = 0.0;
    constexpr float         ENV_MOD_MIN = 0.0;
    constexpr float         ENV_MOD_MAX = 1.0;
}


}//namespace parameters