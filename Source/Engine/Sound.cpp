/*
  ==============================================================================

    SineWaveSound.cpp
    Created: 20 Aug 2020 3:09:20pm
    Author:  maxime

  ==============================================================================
*/

#include "Sound.h"

namespace engine {

Sound::Sound()
{
    /// Nothing to do here
}

bool Sound::appliesToNote(int note)
{
    return true;
}

bool Sound::appliesToChannel(int channel)
{
    return true;
}

}//namespace engine