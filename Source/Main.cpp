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
#include "Control/MidiDeviceMonitor.h"
#include "Utils/Parameters.h"
#include "Tests/TestRunner.h"

//==============================================================================
int main (int argc, char* argv[])
{
#ifndef TESTING
    auto device_manager = std::make_unique<juce::AudioDeviceManager>();
    auto* messageManager = juce::MessageManager::getInstance();
    auto midiBroker = control::MidiBroker();
    auto engine = engine::RaciderryEngine(midiBroker);
    DBG("Created");


    sleep(1);

    // Select and init the audio device
    DBG(device_manager->initialise(0, 2, nullptr, true, "", &parameters::device::PISOUND_SETUP));
    if (device_manager->getCurrentAudioDevice() == nullptr) 
    {
        // If the pisound config is not available we try with custom pulseaudio
        DBG(device_manager->initialise(0, 2, nullptr, true, "", &parameters::device::DEV_SETUP));
    }
    if (device_manager->getCurrentAudioDevice() == nullptr) 
    {
        // If the pisound config and the dev config failed to load
        // We try with basic pulseaudio
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
    auto midiMonitor = control::MidiDeviceMonitor(*device_manager);
    device_manager->addMidiInputDeviceCallback("", &midiBroker);
    // // Start listening to MIDI inputs
    // device_manager->addMidiInputDeviceCallback("", control::MidiBroker::getInstance());
    // auto midiDeviceInfo = juce::MidiInput::getAvailableDevices();
    // for (auto& device : midiDeviceInfo)
    // {
    //     device_manager->setMidiInputDeviceEnabled(device.identifier, true);
    //     DBG(device.name);
    //     DBG(device.identifier);
    //     auto enabled = device_manager->isMidiInputDeviceEnabled(device.identifier);
    //     if(enabled)
    //     {
    //         DBG("ENABLED");
    //     } else 
    //     {
    //         DBG("DISABLED");
    //     }
    // }

    messageManager->runDispatchLoop();

    while(true) {
      sleep(1);
    //   device_manager->playTestSound();
    }

    return 0;
#else
    // Test mode
    auto testRunner = tests::TestRunner();
    auto* messageManager = juce::MessageManager::getInstance();
    testRunner.startThread(10);

    messageManager->runDispatchLoop();
    juce::MessageManager::deleteInstance();

    if (testRunner.getState() != tests::TestRunner::FINISHED) {
        return 1;
        DBG("Test failed");
    }

    return 0;

#endif
}
