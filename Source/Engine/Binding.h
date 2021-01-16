/*
  ==============================================================================

    Binding.h
    Created: 4 Jan 2021 3:07:49pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Control/ControllableParameter.h"
#include "Engine/SignalBus.h"
#include "Engine/NoiseGenerator.h"

namespace engine
{

/**
 * @struct enging::Bindings
 * @brief Holds weak and strongs references to engine bindings.
 * 
 * Store reference to all the bindings a engine module could have
 *  - m_parameterMap : Holds the MIDI parameters a module could connect to
 *  - r_noiseGenerator : The noise generator modules can use to modulate their parameter
 *  - r_signalBus : The signal bus modules can use to share signal to each other
*/
struct Bindings
{
    std::weak_ptr<std::map<juce::Identifier, control::ControllableParameter>> m_parameterMap;
    NoiseGenerator& r_noiseGenerator;
    SignalBus& r_signalBus;
};

} // namespace engine
