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
    : m_osc1(),
      m_osc2(),
      m_mixingBuffer(),
      m_oscRatio()
{
    m_osc1.initialise(utils::waveform::saw, 512);
    m_osc2.initialise(utils::waveform::square, 512);

    m_oscRatio = control::MidiBroker::getInstance()->getParameter(identifiers::controls::WAVEFORM_RATIO);
    jassert(m_oscRatio.isValid());
}

//==============================================================================
void DualOscillator::setFrequency(float newFrequency, bool force)
{
    m_osc1.setFrequency(newFrequency, force);
    m_osc2.setFrequency(newFrequency, force);
}

void DualOscillator::prepare(float sampleRate, int blockSize) noexcept
{
    m_mixingBuffer.setSize(1, blockSize);
    m_osc1.prepare({sampleRate, blockSize, 1});
    m_osc2.prepare({sampleRate, blockSize, 1});
}

void DualOscillator::reset()
{
    m_osc1.reset();
    m_osc2.reset();
}

void DualOscillator::process(juce::AudioBuffer<float>& outputBuffer, int startSample, 
        int numSamples)
{
    jassert(m_osc1.isInitialised());
    jassert(m_osc2.isInitialised());

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
        m_osc1.process(context);
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
        m_osc2.process(context);
    }
    outputBuffer.addFrom(0, startSample, m_mixingBuffer, 0, 0, numSamples, 
            ratio * WAFEFORM_GENERAL_GAIN);

}

} // namespace engine

