/*
  ==============================================================================

    DualOscillator.h
    Created: 30 Aug 2020 7:22:42pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Engine/Oscillators/WavetableOscillator.h"

#include "Control/ControllableParameter.h"

namespace engine
{

/**
 * @class engine::DualOscillator
 * @brief A dual oscillator which mix ratio is controlled by the user
 * 
 * Based on the juce::dsp::Oscillator design.
 * 
 * Connected to the waveform ratio parameter, this produce a mix of two signals
 * A & B by computing ratio * A + (1 - ratio) * B. Signals are for now hardcoded
 * as saw and square signals
 */
class DualOscillator
{
public:
    DualOscillator();

//==============================================================================
    /// juce::dsp::Oscillator like methods
    void setFrequency(float newFrequency, bool force=false);
    void prepare(float sampleRate, int blockSize) noexcept;
    void reset();
    void process(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples);

private:
//==============================================================================
    juce::AudioSampleBuffer                     m_wavetable1;
    juce::AudioSampleBuffer                     m_wavetable2;
    WavetableOscillator                         m_wtOsc1;
    WavetableOscillator                         m_wtOsc2;
    juce::AudioBuffer<float>                    m_mixingBuffer;
    
    control::ControllableParameter              m_oscRatio;
};

} // namespace engine
