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

/**
 * @class engine::NoiseGenerator
 * @brief A simple noise generator to use on engine signal to add some randomness
 * to the sound.
 * 
 * One instance should be shared among the whole engine. Call getNoiseFactor() 
 * to get a factor around 1.f to multiply your signal with.
 * 
 * This noise generator is based on a uniform distribution among 
 * [1.0 - range: 1.0 + range]
 */
class NoiseGenerator
{
public:
    /**
     * @brief Construct a new Noise Generator object
     * 
     * @param range The float range to vary around 1.f. Be careful when using
     * values higher than 0.05, it tends to produce a really audible and instable
     * effect.
     */
    NoiseGenerator(float range);

    /**
     * @brief Compute a random factor around 1.f to multiply your signal with to
     * add some randomness to a parameter.
     */
    forcedinline float getNoiseFactor() noexcept
    {
        return m_distrib(m_generator);
    }

private:
    std::mt19937                            m_generator;
    std::uniform_real_distribution<float>   m_distrib;
};

} // namespace engine