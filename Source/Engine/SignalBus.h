/*
  ==============================================================================

    VoltageBus.h
    Created: 8 Sep 2020 6:22:06pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace engine
{

/**
 * @class engine::SignalBus
 * @brief A small communication bus dedicated to atomic lock-free communication
 * within the audio thread
 * 
 * Implements the JUCE's singleton pattern.
 * 
 * Use this class when you want to propagate signal values (like env values)
 * between differents modules of the engine. 
 * 
 * To add a new value to the bus, add a new value to the SignalBugs::SignalId 
 * enum and use it as a key
 */
class SignalBus
{
public:
    /**
     * @brief List the differents signal ids availables in the bus
     * 
     * When adding a new value make sure it is bounded by both 0 and SignalId::MAX
     */
    enum SignalId
    {
        VEG = 0,            // VCA Envelope
        AEG = 1,            // Accent Envelope

        /* This should always be the highest value */
        MAX
    };

//==============================================================================
    SignalBus();
    ~SignalBus() { clearSingletonInstance(); }

//==============================================================================
    /**
     * @brief Atomically reads the signal from the bus
     * 
     * @param voltageId The id of the signal to read
     * @return The value of the signal if the id is a valid one, 0 otherwise
     */
    forcedinline float readSignal(SignalId voltageId) const noexcept
    {
        if (voltageId >= 0 && voltageId < SignalId::MAX)
        {
            return m_voltageArray[voltageId].get();
        }
        return 0.;
    };

    /**
     * @brief Atomically write the new signal value in the bus
     * 
     * @param voltageId The id of the signal to write. If the id is not valid,
     * this method does nothing
     * @param voltage The new value to write
     */
    forcedinline void updateSignal(SignalId voltageId, float voltage) noexcept
    {
        if (voltageId >= 0 && voltageId < SignalId::MAX)
        {
            m_voltageArray[voltageId].set(voltage);
        }
    };

    JUCE_DECLARE_SINGLETON(SignalBus, false)
private:
//==============================================================================
    juce::Atomic<float>                      m_voltageArray[MAX];
};

} //namespace engine