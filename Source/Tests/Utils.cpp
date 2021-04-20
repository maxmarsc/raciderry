/*
  ==============================================================================

    Utils.cpp
    Created: 4 Feb 2021 11:44:31pm
    Author:  maxime

  ==============================================================================
*/

#include "Utils.h"

namespace tests
{

juce::MidiMessage GenerateRandomMidiMessage(juce::Random& rng, uint channel)
{
    if (channel == 0 || channel > 16) {
        channel = 1 + rng.nextInt(16);
    }

    auto msgType = rng.nextInt(5);
    switch(msgType) {
        case 0:
            // noteOn
            return juce::MidiMessage::noteOn(channel, rng.nextInt(128), rng.nextFloat());
        case 1:
            // noteOff
            return juce::MidiMessage::noteOff(channel, rng.nextInt(128), rng.nextFloat());
        case 2:
            // pitch wheel
            return juce::MidiMessage::pitchWheel(channel, rng.nextInt(16384));
        case 3:
            // Program change
            return juce::MidiMessage::programChange(channel, rng.nextInt(128));
        case 4:
        default:
            // CC
            return juce::MidiMessage::controllerEvent(channel, rng.nextInt(127), rng.nextInt(127));
    }
}

} // namespace tests
