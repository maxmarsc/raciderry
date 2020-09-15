/*
  ==============================================================================

    VoltageBus.cpp
    Created: 8 Sep 2020 6:22:06pm
    Author:  maxime

  ==============================================================================
*/

#include "SignalBus.h"

namespace engine 
{

SignalBus::SignalBus()
{
    for (auto id = 0; id < SignalBus::SignalId::MAX; ++id)
    {
        m_voltageArray[id].set(0.);
    }
}

//==============================================================================
JUCE_IMPLEMENT_SINGLETON(SignalBus)

} // namespace engine