/*
  ==============================================================================

    DualOscillator.h
    Created: 30 Aug 2020 7:22:42pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Control/ControllableParameter.h"

namespace engine
{

class DualOscillator
{
public:
    DualOscillator();

    void setFrequency(float newFrequency, bool force=false);
    void prepare(float sampleRate, int blockSize) noexcept;
    void reset();
    void process(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples);

private:
    juce::dsp::Oscillator<float>                m_osc1;
    juce::dsp::Oscillator<float>                m_osc2;
    juce::AudioBuffer<float>                    m_mixingBuffer;
    
    control::ControllableParameter              m_oscRatio;

};

} // namespace engine
