/*
  ==============================================================================

    Utils.h
    Created: 20 Dec 2020 3:25:43pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#ifndef __TESTS_UTILS_H__
#define __TESTS_UTILS_H__

namespace tests
{

juce::MidiMessage GenerateRandomMidiMessage(juce::Random& rng, uint channel = 0);

namespace category
{

    const juce::String control("control");

namespace engine
{
    const juce::String bindings("bindings");
    const juce::String oscillators("oscillators");
    const juce::String envelopes("envelopes");
    const juce::String filters("filters");
    const juce::String synth("synth");

} // namespace engine

} // namespace category

} // namespace tests

#endif //__TESTS_UTILS_H__
