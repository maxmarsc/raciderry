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

class SignalBus
{
public:
    enum SignalId
    {
        MEG = 0,           // Main Envelope Generator

        /* This should always be the highest value */
        MAX
    };

    SignalBus();
    ~SignalBus() { clearSingletonInstance(); }

    forcedinline float readSignal(SignalId voltageId) noexcept
    {
        if (voltageId >= 0 && voltageId < SignalId::MAX)
        {
            return m_voltageArray[voltageId].get();
        }
        return 0.;
    };

    forcedinline void updateSignal(SignalId voltageId, float voltage) noexcept
    {
        if (voltageId >= 0 && voltageId < SignalId::MAX)
        {
            m_voltageArray[voltageId].set(voltage);
        }
    };

    JUCE_DECLARE_SINGLETON(SignalBus, false)
private:
    juce::Atomic<float>                      m_voltageArray[MAX];
};

} //namespace engine