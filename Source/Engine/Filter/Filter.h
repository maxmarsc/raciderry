/*
  ==============================================================================

    Filter.h
    Created: 4 Sep 2020 5:01:38pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Engine/Filter/OberheimVariationModel.h"

#include "Control/ControllableParameter.h"

namespace engine
{

class Filter
{
public:
    Filter();
    ~Filter() {};

    void prepare(float sampleRate, int blockSize) noexcept;
    void reset();
    void process(juce::dsp::ProcessContextReplacing<float>& context);

private:
    juce::dsp::LadderFilter<float>              m_ladderFilter;
    std::unique_ptr<OberheimVariationMoog>      m_oberheimFilter;
    control::ControllableParameter              m_cutoffFreq;
    control::ControllableParameter              m_resonance;
    control::ControllableParameter              m_drive;
    control::ControllableParameter              m_accent;
};

} // namespace engine

