/*
  ==============================================================================

    NoiseGenerator.h
    Created: 17 Oct 2020 1:10:28am
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <random>

#include <JuceHeader.h>

namespace engine
{

class NoiseGenerator
{
public:
    NoiseGenerator(float noisePercent);

    forcedinline float getNoiseFactor() noexcept
    {
        return m_distrib(m_generator);
    }

private:
    std::mt19937                            m_generator;
    std::uniform_real_distribution<float>   m_distrib;
};

} // namespace engine