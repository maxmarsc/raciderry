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
#include "Engine/NoiseGenerator.h"

#include "Control/ControllableParameter.h"

namespace engine
{

/**
 * @class engine::Filter
 * @brief Wrapper for a modelisation of an analog filter
 * 
 * Wraps a modelisation of an analog filter design from
 * https://github.com/ddiakopoulos/MoogLadders
 * 
 * Holds an instance of the filter and keeps track of the different
 * parameters. This is where the accent's and envelope modulations of the
 * filter's cutoff are computed
 */
class Filter
{
public:
    Filter(NoiseGenerator& noiseGenerator);
    ~Filter() {};

//==============================================================================
    /// juce::dsp::processor like methods
    void prepare(float sampleRate, int blockSize) noexcept;
    void reset();
    /**
     * @brief Filter the audio block
     * 
     * Before processing this method computes the modulation of the filter 
     * parameters (accent, env), updates the value and then process the audio
     */
    void process(juce::dsp::ProcessContextReplacing<float>& context);

private:
//==============================================================================
    std::unique_ptr<OberheimVariationMoog>      m_oberheimFilter;
    NoiseGenerator&                             m_noiseGenerator;
    control::ControllableParameter              m_cutoffFreq;
    control::ControllableParameter              m_resonance;
    control::ControllableParameter              m_drive;
    control::ControllableParameter              m_envMod;
    control::ControllableParameter              m_accent;
};

} // namespace engine

