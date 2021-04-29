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
        192000,
        64,
        0,
        true,
        2,
        true
    });

    const juce::AudioDeviceManager::AudioDeviceSetup DEV_SETUP({
        juce::String("Playback/recording through the PulseAudio sound server"),
        juce::String("Playback/recording through the PulseAudio sound server"),
        96000,
        64,
        0,
        true,
        2,
        true
    });
} // namespace device

namespace files
{
    const auto PRESETS_FILE = juce::String("presets.xml");
    const auto PARAMETERS = juce::String("/etc/raciderry.json");
}

struct Parameter {
    int     m_cc = -1;
    float   m_default = 0.f;
    float   m_min = 0.f;
    float   m_max = 0.f;

    Parameter() = default;

    static const std::map<juce::Identifier, Parameter>   loadParameters(
            int& globalChannel, int& savePatchCC);
private:
    Parameter(const juce::var& data);
};

namespace values
{
    // Limiter values
    constexpr float         LIMITER_RELEASE_MS = 10.0;
    constexpr float         LIMITER_THRESHOLD_DB = -0.1;
}


}//namespace parameters