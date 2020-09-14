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

/**
 * @class MidiBroker
 * @brief Singleton instance, performing the translation btw midi protocol and 
 * internal logic
 * 
 * This implements the JUCE's singleton pattern. Once created it can be declared
 * as the Midi message handler for any DeviceManager.
 */
class MidiBroker : public juce::MidiInputCallback
{
public:
    MidiBroker();
    ~MidiBroker() { clearSingletonInstance(); }
    
    /**
     * @brief Get the next note buffer
     * 
     * @return const juce::MidiBuffer& A midi buffer containing every note 
     * note message received since the last call
     * @note This should only be called by the audio thread, the access is
     * atomic lock-free but not thread-safe.
     */
    const juce::MidiBuffer& getNoteMidiBuffer() noexcept;
    /**
     * @brief Get the Parameter object corresponding to the given identifier
     * 
     * @param id The identifier look for
     */
    ControllableParameter getParameter(const juce::Identifier& id);

//==============================================================================
    /**
     * @name juce::MidiInputCallback overrides.
     */
    ///@{
    void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& msg) override;
    ///@}
//==============================================================================

    JUCE_DECLARE_SINGLETON(MidiBroker, false)
private:
    void initParameters();
    void handleNoteMessage(const juce::MidiMessage& msg);
    void handleControllerMessage(const juce::MidiMessage& msg);

//==============================================================================

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