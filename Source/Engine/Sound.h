/*
  ==============================================================================

    SineWaveSound.h
    Created: 20 Aug 2020 3:09:20pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace engine {

/**
 * @brief Simple inheritance of juce::SynthesiserSound.
 */
class Sound : public juce::SynthesiserSound
{
public:
    Sound();

//==============================================================================
    /**
     * @name juce::SynthesiserSound overrides.
     */
    ///@{
    bool appliesToNote(int) override;
    bool appliesToChannel(int) override;
    ///@}
};

}//namespace engine