/*
  ==============================================================================

    ADSR.h
    Created: 21 Aug 2020 8:33:48pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Control/ControllableParameter.h"

namespace engine {

class ADSR : public juce::ChangeListener
{
public:
    ADSR();

    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    void setSampleRate(double sampleRate);
    void reset();
    void noteOn();
    void noteOff();

    bool isActive();
    void applyEnvelopeToBuffer(juce::AudioBuffer<float>& buffer, int startSample, int numSamples);

private:
    enum class State {idle, attack, decay, sustain, release};

    void updateAttack();
    void updateDecay();
    void updateRelease();
    void updateSustain();

    forcedinline void computeNextEnvValue();
    forcedinline double computeEnvCoeff(int rateInSample, double targetRatio);

    // Parameters the user can control
    control::ControllableParameter          m_attack;
    control::ControllableParameter          m_decay;
    control::ControllableParameter          m_release;
    control::ControllableParameter          m_sustain;

    /// Real Time properties
    // ADSR
    juce::Atomic<int>       m_attackNumSamples;
    juce::Atomic<float>     m_attackCoeff;
    juce::Atomic<float>     m_attackBase;
    juce::Atomic<int>       m_decayNumSamples;
    juce::Atomic<float>     m_decayCoeff;
    juce::Atomic<float>     m_decayBase;
    juce::Atomic<int>       releaseNumSamples_;
    juce::Atomic<float>     m_releaseCoeff;
    juce::Atomic<float>     m_releaseBase;

    // Misc
    juce::Atomic<float>     m_sampleRate;
    juce::Atomic<float>     m_lastEnvValue;
    juce::Atomic<State>     m_state;
};

}//namespace engine