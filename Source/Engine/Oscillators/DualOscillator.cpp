/*
  ==============================================================================

    DualOscillator.cpp
    Created: 30 Aug 2020 7:22:42pm
    Author:  maxime

  ==============================================================================
*/

#include "DualOscillator.h"

#include "Utils/Utils.h"
#include "Utils/Identifiers.h"
#include "Control/MidiBroker.h"

namespace engine
{

constexpr double        WAFEFORM_GENERAL_GAIN = 0.75;

DualOscillator::DualOscillator()
    : m_wavetable1(),
      m_wavetable2(),
      m_wtOsc1(m_wavetable1),
      m_wtOsc2(m_wavetable2),
      m_osc1(),
      m_osc2(),
      m_mixingBuffer(),
      m_oscRatio()
{
    // Load the wavetables in the buffers
    utils::waveform::loadWavetableFromBinaryWaveFile(m_wavetable1, 
            BinaryData::waveform_saw_wav, BinaryData::waveform_saw_wavSize);
    utils::waveform::loadWavetableFromBinaryWaveFile(m_wavetable2,
            BinaryData::waveform_square_wav, BinaryData::waveform_square_wavSize);

    m_oscRatio = control::MidiBroker::getInstance()->getParameter(identifiers::controls::WAVEFORM_RATIO);
    jassert(m_oscRatio.isValid());
}

//==============================================================================
void DualOscillator::setFrequency(float newFrequency, bool force)
{
    m_wtOsc1.setFrequency(newFrequency, force);
    m_wtOsc2.setFrequency(newFrequency, force);
}

void DualOscillator::prepare(float sampleRate, int blockSize) noexcept
{
    m_mixingBuffer.setSize(1, blockSize);
    m_wtOsc1.prepare(sampleRate, blockSize);
    m_wtOsc2.prepare(sampleRate, blockSize);
}

void DualOscillator::reset()
{
    m_wtOsc1.reset();
    m_wtOsc2.reset();
}

void DualOscillator::process(juce::AudioBuffer<float>& outputBuffer, int startSample, 
        int numSamples)
{
    // Init
    m_mixingBuffer.clear();
    auto block = juce::dsp::AudioBlock<float>(
        outputBuffer.getArrayOfWritePointers(),
        outputBuffer.getNumChannels(),
        startSample,
        numSamples
    );

    // We make sure we use the same value for the whole block
    auto ratio = m_oscRatio.getCurrentValue();

    // Process and apply gain for osc n°1
    { 
        auto context = juce::dsp::ProcessContextReplacing<float>(block);
        m_wtOsc1.process(context);
    }
    outputBuffer.applyGain((1.0 - ratio) * WAFEFORM_GENERAL_GAIN);

    // Process and apply gain for osc n° 2
    block = juce::dsp::AudioBlock<float>(
        m_mixingBuffer.getArrayOfWritePointers(),
        m_mixingBuffer.getNumChannels(),
        0,
        numSamples
    );
    { 
        auto context = juce::dsp::ProcessContextReplacing<float>(block);
        m_wtOsc2.process(context);
    }
    outputBuffer.addFrom(0, startSample, m_mixingBuffer, 0, 0, numSamples, 
            ratio * WAFEFORM_GENERAL_GAIN);

}

} // namespace engine

