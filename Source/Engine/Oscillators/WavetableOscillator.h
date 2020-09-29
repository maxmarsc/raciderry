/*
  ==============================================================================

    WavetableOscillator.h
    Created: 24 Sep 2020 4:35:55pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace engine
{

using SmoothedFrequency = juce::SmoothedValue<float, juce::ValueSmoothingTypes::Multiplicative>;

class WavetableOscillator
{
public:
    WavetableOscillator(const juce::AudioSampleBuffer& wavetable);

    void setFrequency(float newFreq, bool force=false) noexcept;
    void prepare(float sampleRate, int blockSize) noexcept;
    void reset() noexcept;
    void process(const juce::dsp::ProcessContextReplacing<float>& context) noexcept;

private:
    forcedinline float getNextSample() noexcept;

    const juce::AudioSampleBuffer&          m_wavetable;
    SmoothedFrequency                       m_frequency;
    float                                   m_currentIndex;
    float                                   m_tableDelta;
    float                                   m_tableSizeOverSampleRate;

};

} // namespace engine
