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

class Sound : public juce::SynthesiserSound
{
public:
    Sound();

    bool appliesToNote(int) override;
    bool appliesToChannel(int) override;
};

}//namespace engine