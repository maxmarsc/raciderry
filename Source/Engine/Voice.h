/*
  ==============================================================================

    RaciderryVoice.h
    Created: 20 Aug 2020 3:24:47pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Engine/Envelopes/VCAEnvelope.h"
#include "Engine/Envelopes/AccentEnvelope.h"
#include "Engine/Binding.h"

namespace engine {

class DualOscillator;

/**
 * @class engine::Voice
 * @brief Represents a voice played by the raciderry juce::Synthesiser instance
 * 
 * Custom inheritance of the juce::SynthesiserVoice class to fit our needs
 * 
 * Handles waveforms oscillators and envelope generators
 */
class Voice : public juce::SynthesiserVoice
{
public:
    Voice(Bindings bindings);

//==============================================================================
    /**
     * @brief Returns a weak_ptr to the oscillator
     * @note This is needed in order to updates the oscillator when the buffer
     * size if changed, as the juce::dsp::Synthesiser class holding this voice
     * will only propagate samplerate changes.
     */
    std::weak_ptr<DualOscillator> getOscPtr() const;

//==============================================================================
     /**
     * @name juce::SynthesiserVoice overrides.
     */
    ///@{
    bool canPlaySound(juce::SynthesiserSound*) override { return true; };
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound *sound, 
                   int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
    void setCurrentPlaybackSampleRate(double newRate) override;
    ///@}

private:
//==============================================================================
    VCAEnvelope                                     m_ampEnvelope;
    AccentEnvelope                                  m_accEnvelope;
    std::shared_ptr<DualOscillator>                 m_osc;
    juce::Atomic<bool>                              m_noteStarted;
};

}//namespace engine