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

class MidiDeviceMonitor : public juce::Timer
{
public:
    MidiDeviceMonitor(juce::AudioDeviceManager& deviceManager);

    void timerCallback() override;

private:
    juce::AudioDeviceManager&       m_deviceManager;
};

} // namespace control
