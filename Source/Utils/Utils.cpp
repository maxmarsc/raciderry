/*
  ==============================================================================

    Utils.cpp
    Created: 20 Aug 2020 1:54:21pm
    Author:  maxime

  ==============================================================================
*/

#include "Utils.h"

#include <random>

#include <JuceHeader.h>

namespace utils {

float generateRandomPhase()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.f, juce::MathConstants<float>::twoPi);

    return dis(gen);
}

const std::function<float(float)>& getSineWave()
{
    static const auto function = std::function<float(float)>(sinf);
    return function;
}

float shiftRadians(float radianValueToShift)
{
    if (radianValueToShift < 0.)
    {
        radianValueToShift *= -1;
        radianValueToShift = fmod(radianValueToShift, juce::MathConstants<float>::twoPi);
        radianValueToShift = juce::MathConstants<float>::twoPi - radianValueToShift;
    }
    else if (radianValueToShift >= juce::MathConstants<float>::twoPi)
    {
        radianValueToShift = fmod(radianValueToShift, juce::MathConstants<float>::twoPi);
    }

    return radianValueToShift;
}

namespace waveform
{

float saw(float phase)
{
    // We ensure the value is btw 0 and 2pi
    phase = shiftRadians(phase);
    jassert(phase >= 0. && phase < juce::MathConstants<float>::twoPi);

    return (juce::MathConstants<float>::pi - phase) / juce::MathConstants<float>::pi;
}

float square(float phase)
{
    // We ensure the value is btw 0 and 2pi
    phase = shiftRadians(phase);
    jassert(phase >= 0. && phase < juce::MathConstants<float>::twoPi);

    if (phase < juce::MathConstants<float>::pi)
    {
        return 1.f;
    }
    
    else return 0.f;
}

void loadWavetableFromBinaryWaveFile(
        juce::AudioSampleBuffer& bufferToAllocate,
        const void* sourceData,
        size_t sourceDataSize)
{
    // The given audio buffer should be empty
    jassert(bufferToAllocate.getNumChannels() == 0);
    jassert(bufferToAllocate.getNumSamples() == 0);

    // We create a reader for the audio file
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    auto reader = std::unique_ptr<juce::AudioFormatReader>(
            formatManager.createReaderFor(
                std::make_unique<juce::MemoryInputStream>(
                    sourceData, sourceDataSize, false)));

    jassert(reader->numChannels == 1);

    // We allocate the buffer to make it hold the audio data
    bufferToAllocate.setSize(1, reader->lengthInSamples);
    reader->read(&bufferToAllocate, 0, reader->lengthInSamples, 0, true, true);
}

} // namespace waveform

}//namespace utils