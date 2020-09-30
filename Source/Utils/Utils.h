/*
  ==============================================================================

    Utils.h
    Created: 20 Aug 2020 1:54:21pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <functional>

#include <JuceHeader.h>

namespace utils {

/**
 * @brief Generates a random phase btw 0 and 2pi
 * @note This method uses static random generator, so the first call will be longer
 */
float generateRandomPhase();

/**
 * @brief Returns the sinewave function as a std::function
 */
const std::function<float(float)>& getSineWave();

/**
 * @brief Recast the radian value within [0; 2pi] range
 */
float shiftRadians(float radianValueToShift);

namespace waveform
{

float saw(float phase);
float square(float phase);

void loadWavetableFromBinaryWaveFile(
        juce::AudioSampleBuffer& bufferToAllocate,
        const void* sourceData,
        size_t sourceDataSize);

} // namespace waveform

}//namespace utils

