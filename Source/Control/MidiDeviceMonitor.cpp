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
    : r_deviceManager(deviceManager)
{
    startTimer(CHECK_INTERVAL_MS);
}

void MidiDeviceMonitor::timerCallback()
{
    auto midiDeviceInfo = juce::MidiInput::getAvailableDevices();
    
    std::unordered_set<juce::String> available_devices;

    for (auto& device : midiDeviceInfo)
    {
        available_devices.insert(device.identifier);

        if (! m_enabledDevices.count(device.identifier))
        {
            // If not registered, we add it
            jassert(! r_deviceManager.isMidiInputDeviceEnabled(device.identifier));

            DBG("Connecting to " + device.name + " [" + device.identifier + "]");
            r_deviceManager.setMidiInputDeviceEnabled(device.identifier, true);

            if (r_deviceManager.isMidiInputDeviceEnabled(device.identifier))
            {
                DBG("Successfully connected");
                m_enabledDevices.insert(device.identifier);
            }
        }
        else
        {
            // It registered, we assert it is registered to the AudioDeviceManager
            jassert(r_deviceManager.isMidiInputDeviceEnabled(device.identifier));
        }
    }

    auto to_delete = std::vector<juce::String>();
    for (auto& enabled_device : m_enabledDevices)
    {
        if (available_devices.count(enabled_device) < 1)
        {
            r_deviceManager.setMidiInputDeviceEnabled(enabled_device, false);
            DBG("Disconnected from [" + enabled_device + "]");
            to_delete.push_back(enabled_device);
        }
    }

    for (auto& device_to_delete : to_delete)
    {
        m_enabledDevices.erase(device_to_delete);
    }
}

} // namespace control
