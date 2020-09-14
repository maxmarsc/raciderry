/*
  ==============================================================================

    Utils.h
    Created: 20 Aug 2020 1:54:21pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <functional>

namespace utils {

float generateRandomPhase();
const std::function<float(float)>& getSineWave();
float shiftRadians(float radianValueToShift);

namespace waveform
{

float saw(float phase);
float square(float phase);

} // namespace waveform

}//namespace utils

