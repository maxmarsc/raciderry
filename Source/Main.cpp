/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include <JuceHeader.h>

#include <unistd.h>
#include <cstdlib>

#include "Engine/Engine.h"
#include "Control/MidiBroker.h"
#include "Utils/Parameters.h"

//==============================================================================
int main (int argc, char* argv[])
{
    auto device_manager = std::make_unique<juce::AudioDeviceManager>();
    auto* messageManager = juce::MessageManager::getInstance();
    auto engine = engine::RaciderryEngine();
    DBG("Created");


    sleep(1);

    // Select and init the audio device
    DBG(device_manager->initialise(0, 2, nullptr, true, "", &parameters::device::PISOUND_SETUP));
    if (device_manager->getCurrentAudioDevice() == nullptr) 
    {
        // If the pisound config failed to load (debug on x86_64 as an example)
        // We try with pulseaudio
        device_manager->initialise(0, 2, nullptr, true, "*PulseAudio*", nullptr);
    }
    if (device_manager->getCurrentAudioDevice() == nullptr)
    {
        exit(1);
    }
    auto* device = device_manager->getCurrentAudioDevice();
    if (device)
    {
        DBG(juce::String("Connected to : ") + device->getName());
    }

    // Start the audio thread
    device_manager->addAudioCallback(&engine);

    // Start listening to MIDI inputs
    device_manager->addMidiInputDeviceCallback("", control::MidiBroker::getInstance());
    auto midiDeviceInfo = juce::MidiInput::getAvailableDevices();
    for (auto& device : midiDeviceInfo)
    {
        device_manager->setMidiInputDeviceEnabled(device.identifier, true);
        DBG(device.name);
        DBG(device.identifier);
        auto enabled = device_manager->isMidiInputDeviceEnabled(device.identifier);
        if(enabled)
        {
            DBG("ENABLED");
        } else 
        {
            DBG("DISABLED");
        }
    }

    messageManager->runDispatchLoop();

    while(true) {
      sleep(1);
    //   device_manager->playTestSound();
    }

    return 0;
}
