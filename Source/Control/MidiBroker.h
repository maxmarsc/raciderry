/*
  ==============================================================================

    MidiBroker.h
    Created: 24 Aug 2020 4:58:30pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Control/ControllableParameter.h"

namespace control
{

class MidiBroker : public juce::MidiInputCallback
{
public:
    MidiBroker();
    ~MidiBroker() { clearSingletonInstance(); }

    void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& msg) override; 

    const juce::MidiBuffer& getNoteMidiBuffer() noexcept;
    ControllableParameter getParameter(const juce::Identifier&);

    JUCE_DECLARE_SINGLETON(MidiBroker, false)
private:
    void initParameters();
    void handleNoteMessage(const juce::MidiMessage& msg);
    void handleControllerMessage(const juce::MidiMessage& msg);

    // Midi notes handling
    juce::MidiBuffer                        m_rtMidiBuffer;
    juce::MidiBuffer                        m_atomicMidiBuffer;
    juce::Atomic<bool>                      m_atomicBufferIsLocked;

    // Midi control handling
    std::map<int, ControllableParameter>    m_midiCCToParameterMap;

    // Parameters holding
    std::map<juce::Identifier, ControllableParameter>   m_idToParameterMap;
};


}//namespace control