/*
  ==============================================================================

    NoiseGenerator.cpp
    Created: 17 Oct 2020 1:10:28am
    Author:  maxime

  ==============================================================================
*/

#include "NoiseGenerator.h"

namespace engine
{

NoiseGenerator::NoiseGenerator(float noisePercent)
{
    auto randomDevice = std::random_device();
    m_generator.seed(randomDevice());
    m_distrib.param(std::uniform_real_distribution<float>::param_type(
        1. - noisePercent,
        1. + noisePercent
    ));
}

} // namespace engine
