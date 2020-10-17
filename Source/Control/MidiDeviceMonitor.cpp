/*
  ==============================================================================

    MidiDeviceMonitor.cpp
    Created: 17 Oct 2020 2:33:59pm
    Author:  maxime

  ==============================================================================
*/

#include "MidiDeviceMonitor.h"

constexpr auto CHECK_INTERVAL_MS = 500;

namespace control
{

MidiDeviceMonitor::MidiDeviceMonitor(juce::AudioDeviceManager& deviceManager)
    : m_deviceManager(deviceManager)
{
    startTimer(CHECK_INTERVAL_MS);
}

void MidiDeviceMonitor::timerCallback()
{
    auto midiDeviceInfo = juce::MidiInput::getAvailableDevices();
    
    for (auto& device : midiDeviceInfo)
    {
        if (!m_deviceManager.isMidiInputDeviceEnabled(device.identifier))
        {
            DBG("Connecting to " + device.name);
            m_deviceManager.setMidiInputDeviceEnabled(device.identifier, true);

            if (m_deviceManager.isMidiInputDeviceEnabled(device.identifier))
            {
                DBG("Successfully connected");
            }
        }
    }
}

} // namespace control
