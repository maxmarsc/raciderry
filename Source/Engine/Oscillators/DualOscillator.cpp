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

constexpr double        WAFEFORM_GENERAL_GAIN = 0.5;

DualOscillator::DualOscillator(Bindings bindings)
    : m_wavetable1(),
      m_wavetable2(),
      m_wtOsc1(m_wavetable1, bindings),
      m_wtOsc2(m_wavetable2, bindings),
      r_noiseGenerator(bindings.r_noiseGenerator),
      m_mixingBuffer(),
      m_oscRatio()
{
    // Load the wavetables in the buffers
    utils::waveform::loadWavetableFromBinaryWaveFile(m_wavetable1, 
            BinaryData::waveform_saw_wav, BinaryData::waveform_saw_wavSize);
    utils::waveform::loadWavetableFromBinaryWaveFile(m_wavetable2,
            BinaryData::waveform_square_wav, BinaryData::waveform_square_wavSize);

    // Get the controllable parameters
    auto parameterMap = bindings.m_parameterMap.lock();

    m_oscRatio = (*parameterMap)[identifiers::controls::WAVEFORM_RATIO];
    m_glide = (*parameterMap)[identifiers::controls::GLIDE];
    jassert(m_oscRatio.isValid());
    jassert(m_glide.isValid());

    // Set osc glide
    m_wtOsc1.setGlide(m_glide.getCurrentValue());
    m_wtOsc2.setGlide(m_glide.getCurrentValue());
}

//==============================================================================
void DualOscillator::setFrequency(float newFrequency, bool force) noexcept
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

void DualOscillator::reset() noexcept
{
    m_wtOsc1.reset();
    m_wtOsc2.reset();
}

void DualOscillator::process(juce::AudioBuffer<float>& outputBuffer, int startSample, 
        int numSamples) noexcept
{
    // Init
    m_mixingBuffer.clear();
    auto block = juce::dsp::AudioBlock<float>(
        outputBuffer.getArrayOfWritePointers(),
        outputBuffer.getNumChannels(),
        startSample,
        numSamples
    );

    // We get the controllable values for the whole block
    auto glide = m_glide.getCurrentValue();
    auto ratio = m_oscRatio.getCurrentValue() * r_noiseGenerator.getNoiseFactor();
    m_wtOsc1.setGlide(glide * r_noiseGenerator.getNoiseFactor());
    m_wtOsc2.setGlide(glide * r_noiseGenerator.getNoiseFactor());

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

