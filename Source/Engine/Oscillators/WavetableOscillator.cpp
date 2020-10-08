/*
  ==============================================================================

    WavetableOscillator.cpp
    Created: 24 Sep 2020 4:35:55pm
    Author:  maxime

  ==============================================================================
*/

#include "WavetableOscillator.h"

namespace engine
{

WavetableOscillator::WavetableOscillator(const juce::AudioSampleBuffer& wavetable)
    : m_wavetable(wavetable),
      m_frequency(440.0f),
      m_currentIndex(0.0f),
      m_tableDelta(0.0f),
      m_tableSizeOverSampleRate(0.0f),
      m_sampleRate(0.0),
      m_glide(0.0)
{
    // Nothing to do here
}

void WavetableOscillator::setFrequency(float newFreq, bool force) noexcept
{
    if (force)
    {
        m_frequency.setCurrentAndTargetValue(newFreq);
        m_tableDelta = m_frequency.getCurrentValue() * m_tableSizeOverSampleRate;
        return;
    }

    m_frequency.setTargetValue(newFreq);
}

void WavetableOscillator::prepare(float sampleRate, int blockSize) noexcept
{
    jassert(m_wavetable.getNumChannels() == 1);
    jassert(m_wavetable.getNumSamples() > 0);

    m_tableSizeOverSampleRate = float(m_wavetable.getNumSamples()) / sampleRate;
    m_currentIndex = 0.0f;
    m_tableDelta = 0.0f;
    m_sampleRate = sampleRate;
    m_frequency.reset(m_sampleRate, m_glide);
}

void WavetableOscillator::reset() noexcept
{
    // Nothing to do here
}

void WavetableOscillator::process(const juce::dsp::ProcessContextReplacing<float>& context) noexcept
{
    auto& outblock = context.getOutputBlock();
    jassert(outblock.getNumChannels() == 1);
    auto numSamples = outblock.getNumSamples();
    auto* data = outblock.getChannelPointer(0);

    // Processing with freq smoothing, should not happens to often
    while (m_frequency.isSmoothing() && numSamples--)
    {
        m_tableDelta = m_frequency.getNextValue() * m_tableSizeOverSampleRate;
        *data = getNextSample();
        ++data;
    }

    // Processing without freq smoothing
    while (numSamples--)
    {
        *data = getNextSample();
        ++data;
    }
}

void WavetableOscillator::setGlide(float glideTime) noexcept
{
    jassert(glideTime >= 0);
    m_glide = glideTime;

    if (glideTime > 0)
    {
        m_frequency.update(m_sampleRate, glideTime);
    }
}

forcedinline float WavetableOscillator::getNextSample() noexcept
{
    auto tableSize = m_wavetable.getNumSamples();
    auto* table = m_wavetable.getReadPointer(0);

    auto idx0 = (unsigned int) m_currentIndex;
    auto idx1 = idx0 == (tableSize - 1) ? (unsigned int) 0 : idx0 + 1;

    auto frac = m_currentIndex - idx0;

    auto value0 = table[idx0];
    auto value1 = table[idx1];

    auto interpolatedSample = value0 + frac * (value1 - value0);

    m_currentIndex += m_tableDelta;

    if (m_currentIndex > (float) tableSize)
    {
        m_currentIndex -= (float) tableSize;
    }

    return interpolatedSample;
}

} // namespace engine
