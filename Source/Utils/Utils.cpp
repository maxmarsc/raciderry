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

} // namespace waveform

}//namespace utils