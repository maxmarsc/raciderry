/*
  ==============================================================================

    AmplitudeEnvelope.h
    Created: 15 Sep 2020 5:45:39pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Engine/Binding.h"

namespace engine
{

/**
 * @class  engine::VCAEnvelope
 * @brief  The ADSR envelope generator for the VCA
 * 
 * Based on the juce::ADSR design
 * 
 * ADSR exponential envelope with controllable values. It also internally 
 * updates its value in the SignalBus with the mean value of each block
 */
class VCAEnvelope : public juce::ChangeListener
{
public:
    VCAEnvelope(Bindings bindings);

//==============================================================================
    /**
     * @name juce::ChangeListener overrides.
     */
    ///@{
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    ///@}

//==============================================================================
    /**
     * @brief Returns true if the env is in an active stage
     */
    bool isActive() { return m_state.get() != State::idle; }
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
     * @brief Indicates the Envelope Generator a new note has started
     */
    void noteOn();
    /**
     * @brief Indicates the Envelope Generator a note has ended
     */
    void noteOff();

    /**
     * @brief Apply the amplitude ADSR envelope to the output buffer
     * 
     * @param buffer The buffer to reshape
     * @param startSample The index where to start in the buffer
     * @param numSamples The number of samples to process from the start point
     */
    void applyAmpEnvelopeToBuffer(juce::AudioBuffer<float>& buffer, int startSample, int numSamples);

private:
    enum class State {idle, attack, decay, sustain, release};

    void updateAttack();
    void updateDecay();
    void updateRelease();
    void updateSustain();

    forcedinline void computeNextEnvValue();
    // forcedinline double computeEnvCoeff(int rateInSample, double targetRatio);

//==============================================================================

    // Parameters the user can control
    control::ControllableParameter          m_attack;
    control::ControllableParameter          m_decay;
    control::ControllableParameter          m_release;
    control::ControllableParameter          m_sustain;

    /// Real Time properties
    // ADSR
    juce::Atomic<float>     m_attackCoeff;
    juce::Atomic<float>     m_attackBase;
    juce::Atomic<float>     m_decayCoeff;
    juce::Atomic<float>     m_decayBase;
    juce::Atomic<float>     m_releaseCoeff;
    juce::Atomic<float>     m_releaseBase;

    // Misc
    juce::Atomic<float>     m_sampleRate;
    juce::Atomic<float>     m_lastEnvValue;
    juce::Atomic<State>     m_state;
    SignalBus&              m_signalBus;
};

} // namespace engine