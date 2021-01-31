/*
  ==============================================================================

    MidiDeviceMonitor.h
    Created: 17 Oct 2020 2:33:59pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace control
{

/**
 * @class control::MidiDeviceMonitor
 * @brief A tiny utility class which peridically watch for available MIDI 
 * connections and automatically enable MIDI routing to the given device
 * 
 */
class MidiDeviceMonitor : public juce::Timer
{
public:
    /**
     * @brief Construct a new Midi Device Monitor object
     * 
     * @param deviceManager The device manager to link the MIDI device to
     */
    MidiDeviceMonitor(juce::AudioDeviceManager& deviceManager);

    /**
     * @name juce::Timer overrides.
     */
    ///@{
    void timerCallback() override;
    ///@}

private:
    juce::AudioDeviceManager&           r_deviceManager;
    std::unordered_set<juce::String>    m_enabledDevices;
};

} // namespace control
