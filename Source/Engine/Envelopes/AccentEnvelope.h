/*
  ==============================================================================

    AccentEnvelope.h
    Created: 15 Sep 2020 6:13:02pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Control/ControllableParameter.h"

namespace engine
{

class AccentEnvelope : public juce::ChangeListener
{
public:
    AccentEnvelope();

//==============================================================================
    /**
     * @name juce::ChangeListener overrides.
     */
    ///@{
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    ///@}

//==============================================================================
    /**
     * @brief Updates the sample rate - thread safe
     * 
     * @param sampleRate The new sample rate
     */
    void setSampleRate(double sampleRate);
    /**
     * @brief Reset the state of the envelopes, not the parameters
     */
    void reset();
    /**
     * @brief Indicates the Accent Envelope a new note has started
     */
    void noteOn();
    /**
     * @brief Indicates the Accent Envelope a note has ended
     */
    void noteOff();

    /**
     * @brief Computes and updates the next value for the Accent envelope
     * 
     * @param numSamples The number of samples to the next point
     */
    void nextValue(int numSamples);

private:
    enum class State {idle, attack, decay};

    void updateAttack();
    void updateDecay();

    forcedinline void computeNextEnvValue();

//==============================================================================

    // Parameters the user can control
    control::ControllableParameter          m_decay;

    /// AD compute values
    // ADSR
    float                   m_attackCoeff;
    float                   m_attackBase;
    juce::Atomic<float>     m_crtMax;
    juce::Atomic<float>     m_decayCoeff;
    juce::Atomic<float>     m_decayBase;

    // Misc
    juce::Atomic<float>     m_sampleRate;
    juce::Atomic<float>     m_lastEnvValue;
    juce::Atomic<State>     m_state;
};

} // namespace engine