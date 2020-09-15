/*
  ==============================================================================

    RaciderryVoice.h
    Created: 20 Aug 2020 3:24:47pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Engine/EnvelopeGenerator.h"

namespace engine {

class DualOscillator;

class Voice : public juce::SynthesiserVoice
{
public:
    Voice();

    std::weak_ptr<DualOscillator> getOscPtr();

    bool canPlaySound(juce::SynthesiserSound*) override { return true; };
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, 
                   int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    void setCurrentPlaybackSampleRate(double newRate) override;

private:
    EnvelopeGenerator                                            m_envelope;
    std::shared_ptr<DualOscillator>                 m_osc;

    juce::Atomic<bool>                              m_noteStarted;
};

}//namespace engine