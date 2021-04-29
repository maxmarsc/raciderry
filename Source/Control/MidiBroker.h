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
 * @class control::MidiBroker
 * @brief Performs the translation btw midi protocol and internal logic
 */
class MidiBroker : public juce::MidiInputCallback
{
public:
    MidiBroker();
    ~MidiBroker() {}
    
    /**
     * @brief Get the next note buffer
     * 
     * @return const juce::MidiBuffer& A midi buffer containing every note 
     * note message received since the last call
     * @note Lock-free & thread safe, might return an empty buffer if the 
     * main buffer is locked due to an incoming message
     */
    juce::MidiBuffer getNoteMidiBuffer() noexcept;

    std::weak_ptr<ParameterMap> getIdToParameterMap();
    int getMidiChannel() const { return m_globalChannel; };

//==============================================================================
    /**
     * @name juce::MidiInputCallback overrides.
     */
    ///@{
    void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& msg) override;
    ///@}
//==============================================================================

private:
    void initControllableParameters();
    void initPresets();
    void serializePresets();
    void loadPreset(int presetId);
    void saveToPreset(int presetId);
    void handleNoteMessage(const juce::MidiMessage& msg);
    void handleControllerMessage(const juce::MidiMessage& msg);

//==============================================================================

    // Midi notes handling
    juce::SpinLock                          m_spinLock;
    juce::MidiBuffer                        m_midiBuffer;
    juce::MidiBuffer                        m_fallbackMidiBuffer;
    int                                     m_globalChannel;
    int                                     m_savePatchCC;

    // Parameters mapping
    std::map<int, ControllableParameter>    m_midiCCToParameterMap;
    std::shared_ptr<ParameterMap>           m_idToParameterMap;

    // Presets mapping
    std::unique_ptr<juce::XmlElement>       m_presets;
    bool                                    m_readyToSavePreset;
};


}//namespace control