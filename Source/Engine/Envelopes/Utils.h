/*
  ==============================================================================

    Utils.h
    Created: 18 Sep 2020 10:44:40pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace engine
{

/**
 * @brief Computes the coeff for a step of an exponential envelope
 * 
 * @param rateInSample      The rate of the concerned step
 * @param targetRatio       The ratio of the concerned step
 * @return double  The computed coefficient
 */
forcedinline double computeExpEnvCoeff(int rateInSample, double targetRatio)
{
    if (rateInSample > 0)
    {
        // May need some optimization if too cpu-hungry
        return exp(-log((1.0 + targetRatio) / targetRatio) / rateInSample);
    }
    
    return 0.;
}

} // namespace engine

