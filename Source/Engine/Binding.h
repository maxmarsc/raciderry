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

struct Bindings 
{
    std::weak_ptr<std::map<juce::Identifier, control::ControllableParameter>> m_parameterMap;
    NoiseGenerator& m_noiseGeneratorRef;
    SignalBus& m_signalBusRef;
};

} // namespace engine
