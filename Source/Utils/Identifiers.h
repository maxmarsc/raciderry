/*
  ==============================================================================

    Identifiers.h
    Created: 9 Sep 2020 6:09:32pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/**
 * @file This file contains all the identifiers of the projects
 * @note All these identifiers must be valid XLM identifiers ! ie: no spaces,
 * must start with a letter or an underscore.
 */

namespace identifiers
{

namespace controls
{

/**
 * @brief Unique identifier for each controllable parameters 
 */
const juce::Identifier  ATTACK("ATTACK");
const juce::Identifier  DECAY("DECAY");
const juce::Identifier  SUSTAIN("SUSTAIN");
const juce::Identifier  RELEASE("RELEASE");

const juce::Identifier  WAVEFORM_RATIO("WAVEFORM_RATIO");
const juce::Identifier  GLIDE("GLIDE");

const juce::Identifier  CUTOFF("CUTOFF");
const juce::Identifier  RESONANCE("RESONANCE");
const juce::Identifier  DRIVE("DRIVE");
const juce::Identifier  ENV_MOD("ENV_MOD");
const juce::Identifier  FILTER_MIX("FILTER_MIX");

const juce::Identifier  ACCENT("ACCENT");
const juce::Identifier  ACCENT_DECAY("ACCENT_DECAY");

} //namespace controls

namespace miscellaneous
{

const juce::Identifier  PRESETS("PRESETS");

}// namespace miscellaneous

} // namespace identifiers
