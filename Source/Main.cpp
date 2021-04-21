/*
  ==============================================================================

    This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include <JuceHeader.h>

#include <unistd.h>
#include <cstdlib>
#include <iostream>

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

    device_manager->initialise(0, 2, nullptr, true, "", &parameters::device::PISOUND_SETUP);
    if (device_manager->getCurrentAudioDevice() == nullptr) 
    {
        // If the pisound config is not available we try with custom pulseaudio
        device_manager->initialise(0, 2, nullptr, true, "", &parameters::device::DEV_SETUP);
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
        std::cout << "Connected to : " << device->getName() << std::endl;
    }

    // Start the audio thread
    device_manager->addAudioCallback(&engine);

    // Start listening to MIDI inputs
    auto midiMonitor = control::MidiDeviceMonitor(*device_manager);
    device_manager->addMidiInputDeviceCallback("", &midiBroker);

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
