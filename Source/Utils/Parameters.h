/*
  ==============================================================================

    Identifiers.h
    Created: 21 Aug 2020 8:35:54pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/**
 * @file This files hold the values of all the parameters of the synthesiser, not
 * only the controllable ones.
 * 
 * This should never be included from a header file. This content will probably
 * move to an XML file at some point to avoid recompiling when testing new 
 * values
 */

namespace parameters
{

namespace device
{
    const juce::AudioDeviceManager::AudioDeviceSetup PISOUND_SETUP({
        juce::String("pisound, ; Direct hardware device without any conversions"),
        juce::String("pisound, ; Direct hardware device without any conversions"),
        96000,
        256,
        0,
        true,
        2,
        true
    });

    const juce::AudioDeviceManager::AudioDeviceSetup DEV_SETUP({
        juce::String("Playback/recording through the PulseAudio sound server"),
        juce::String("Playback/recording through the PulseAudio sound server"),
        96000,
        256,
        0,
        true,
        2,
        true
    });
} // namespace device

namespace midiCC
{
    /* The midi CC assignement for every controllable parameter */

    // ADSR
    constexpr int           ATTACK = 73;
    constexpr int           DECAY = 75;
    constexpr int           SUSTAIN = 64;
    constexpr int           RELEASE = 72;

    // Accent
    constexpr int           ACCENT_DECAY = 82;      // GP 7
    constexpr int           ACCENT = 83;            // GP 8

    // OSC
    constexpr int           WAVEFORM_RATIO = 80;    // General purpose 5
    constexpr int           GLIDE = 81;             // General purpose 6

    // FILTER
    constexpr int           CUTOFF = 16;            // General purpose 1
    constexpr int           RESONANCE = 17;         // General purpose 2
    constexpr int           FILTER_DRIVE = 18;      // General purpose 3
    constexpr int           ENV_MOD = 19;            // General purpose 4
}

namespace values
{
    /* The actual default and range values of each controllable parameter */

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

    // Oscillator glide in seconds
    constexpr float        GLIDE_DEFAULT = 0.0;
    constexpr float        GLIDE_MIN = 0.0;
    constexpr float        GLIDE_MAX = 1.0;

    // Limiter values
    constexpr float         LIMITER_RELEASE_MS = 10.0;
    constexpr float         LIMITER_THRESHOLD_DB = -0.1;

    // Accent
    constexpr float         ACCENT_DEFAULT = 0.;
    constexpr float         ACCENT_MIN = 0.;
    constexpr float         ACCENT_MAX = 1.0;
    constexpr float         ACC_DEC_DEFAULT = 0.3;
    constexpr float         ACC_DEC_MIN = 0.010;
    constexpr float         ACC_DEC_MAX = 1.0;

    // Filter
    constexpr float         CUTOFF_DEFAULT =  440.0;   // Hz
    constexpr float         CUTOFF_MIN = 40.0;
    constexpr float         CUTOFF_MAX = 6000.0;
    constexpr float         RESONANCE_DEFAULT = 0.0;     // > 0.
    constexpr float         RESONANCE_MIN = 0.0;
    constexpr float         RESONANCE_MAX = 8.5;
    constexpr float         DRIVE_DEFAULT = 1.10;        // > 1.
    constexpr float         DRIVE_MIN = 1.0;
    constexpr float         DRIVE_MAX = 1.12;
    constexpr float         ENV_MOD_DEFAULT = 0.0;
    constexpr float         ENV_MOD_MIN = 0.0;
    constexpr float         ENV_MOD_MAX = 1.0;
}


}//namespace parameters