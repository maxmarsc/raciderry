/*
  ==============================================================================

    WavetableOscillator.h
    Created: 24 Sep 2020 4:35:55pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Utils/CustomSmoothValue.h"

namespace engine
{

using SmoothedFrequency = utils::CustomSmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative>;

/**
 * @class engine::WavetableOscillator
 * @brief A simple wavetable oscillator with linear interpolation
 * 
 * Based on the juce::dsp::Oscillator design
 * 
 * The oscillator will use the entire AudioSampleBuffer provided as the waveform
 * The more higher the samplerate, the more points the wavetable should contains
 * in order to avoid interpolation artifacts
 */
class WavetableOscillator
{
public:
    /**
     * @brief Construct a new Wavetable Oscillator object
     * 
     * @param wavetable A reference to the sample buffer to use as a waveform. 
     * The buffer does not need to be filled at build time, but should be 
     * initialised before calling WavetableOscillator::prepare method.
     */
    WavetableOscillator(const juce::AudioSampleBuffer& wavetable);

//==============================================================================
    /// juce::dsp::Oscillator like methods
    void setFrequency(float newFreq, bool force=false) noexcept;
    void prepare(float sampleRate, int blockSize) noexcept;
    void reset() noexcept;
    void process(const juce::dsp::ProcessContextReplacing<float>& context) noexcept;

//==============================================================================
    /**
     * @brief Set the Glide time time, default to 0
     * 
     * @param glideTime New glide time in seconds
     */
    void setGlide(float glideTime) noexcept;

private:
    forcedinline float getNextSample() noexcept;

//==============================================================================
    const juce::AudioSampleBuffer&          m_wavetable;
    SmoothedFrequency                       m_frequency;
    float                                   m_currentIndex;
    float                                   m_tableDelta;
    float                                   m_tableSizeOverSampleRate;
    float                                   m_sampleRate;
    float                                   m_glide;
};

} // namespace engine
