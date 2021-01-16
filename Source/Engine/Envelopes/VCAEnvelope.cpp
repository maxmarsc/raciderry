/*
  ==============================================================================

    AmplitudeEnvelope.cpp
    Created: 15 Sep 2020 5:45:39pm
    Author:  maxime

  ==============================================================================
*/

#include "VCAEnvelope.h"

#include "Engine/Envelopes/Utils.h"
#include "Engine/SignalBus.h"

#include "Utils/Identifiers.h"


namespace engine {

// Empirical values
constexpr double ATTACK_RATIO = 0.3;
constexpr double DECAY_RATIO = 0.0001;
constexpr double RELEASE_RATIO = 0.0001;

VCAEnvelope::VCAEnvelope(Bindings bindings)
    : m_attack(),
      m_decay(),
      m_sustain(),
      m_release(),
      m_state(State::idle),
      m_signalBus(bindings.r_signalBus)
{
    // bind to controllable parameters
    auto parameterMap = bindings.m_parameterMap.lock();
    jassert(parameterMap != nullptr);

    m_attack = (*parameterMap)[identifiers::controls::ATTACK];
    m_decay = (*parameterMap)[identifiers::controls::DECAY];
    m_sustain = (*parameterMap)[identifiers::controls::SUSTAIN];
    m_release = (*parameterMap)[identifiers::controls::RELEASE];
    jassert(m_attack.isValid());
    jassert(m_decay.isValid());
    jassert(m_sustain.isValid());
    jassert(m_release.isValid());
    m_attack.addListener(this);
    m_decay.addListener(this);
    m_release.addListener(this);
    m_sustain.addListener(this);
    updateAttack();
    updateDecay();
    updateRelease();
    updateSustain();
}

//==============================================================================
void VCAEnvelope::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (m_attack == source)
    {
        updateAttack();
    }
    else if (m_decay == source)
    {
        updateDecay();
    }
    else if (m_release ==  source)
    {
        updateRelease();
    }
    else if (m_sustain == source)
    {
        updateSustain();
    }
}

//==============================================================================
void VCAEnvelope::setSampleRate(double sampleRate)
{
    jassert(sampleRate > 0.);

    if (sampleRate != m_sampleRate.get())
    {
        m_sampleRate.set(sampleRate);

        // Recompute the rates by calling the setters
        updateAttack();
        updateDecay();
        updateRelease();
    }
}

void VCAEnvelope::reset()
{
    m_state = State::idle;
    m_lastEnvValue.set(0);
}

void VCAEnvelope::noteOn()
{
    // custom made kindof legato
    if (m_state.get() == State::idle || m_state.get() == State::release) 
    {
        m_state.set(State::attack);
    }
}

void VCAEnvelope::noteOff()
{
    if (m_state.get() != State::idle)
    {
        m_state.set(State::release);
    }
}

void VCAEnvelope::applyAmpEnvelopeToBuffer(juce::AudioBuffer<float>& buffer, 
        int startSample, int numSamples)
{
    jassert(numSamples > 0);

    auto sum = float(0);
    auto count = numSamples;
    auto* data = buffer.getWritePointer(0) + startSample;

    while(numSamples--)
    {
        computeNextEnvValue();
        *data *= m_lastEnvValue.get();
        sum += m_lastEnvValue.get();
        data++;
    }
    
    // We send the mean value to others units (filter, ...)
    m_signalBus.updateSignal(SignalBus::SignalId::VEG, sum/count);
}

void VCAEnvelope::updateAttack()
{
    // DBG("New attack : " + juce::String(attack));
    jassert(m_attack.getCurrentValue() > 0.);
    auto atkNumSamples = int(m_attack.getCurrentValue() * m_sampleRate.get());

    // Settings these two consecutively without mutex could lead to a 
    // computation error on one sample if the read performs between the two sets
    // However the computation error is bounded, inaudible, and allows lock-free
    // updates
    m_attackCoeff.set(computeExpEnvCoeff(atkNumSamples, ATTACK_RATIO));
    m_attackBase.set((1.0 + ATTACK_RATIO) * (1.0 - m_attackCoeff.get()));
}

void VCAEnvelope::updateDecay()
{
    jassert(m_decay.getCurrentValue() > 0.);
    // DBG("New decay : " + juce::String(decay));
    auto decNumSamples = int(m_decay.getCurrentValue() * m_sampleRate.get());

    // Settings these two consecutively without mutex could lead to a 
    // computation error on one sample if the read performs between the two sets
    // However the computation error is bounded, inaudible, and allows lock-free
    // updates
    m_decayCoeff.set(computeExpEnvCoeff(decNumSamples, DECAY_RATIO));
    m_decayBase.set((m_sustain.getCurrentValue() - DECAY_RATIO) * (1.0 - m_decayCoeff.get()));
}

void VCAEnvelope::updateRelease()
{
    jassert(m_release.getCurrentValue() > 0.);
    // DBG("New release : " + juce::String(release));
    auto relNumSamples = int(m_release.getCurrentValue() * m_sampleRate.get());

    // Settings these two consecutively without mutex could lead to a 
    // computation error on one sample if the read performs between the two sets
    // However the computation error is bounded, inaudible, and allows lock-free
    // updates
    m_releaseCoeff.set(computeExpEnvCoeff(relNumSamples, RELEASE_RATIO));
    m_releaseBase.set( - RELEASE_RATIO * (1.0 - m_releaseCoeff.get()));
}

void VCAEnvelope::updateSustain()
{
    jassert(m_sustain.getCurrentValue() >= 0. && m_sustain.getCurrentValue() <= 1.0);
    // DBG("New sustain : " + juce::String(sustain));
    m_decayBase.set((m_sustain.getCurrentValue() - DECAY_RATIO) * (1.0 - m_decayCoeff.get()));
}

void VCAEnvelope::computeNextEnvValue()
{
    float newValue;

    switch(m_state.get())
    {
        case State::idle:
            break;

        case State::attack:
            newValue = m_attackBase.get() + m_lastEnvValue.get() * m_attackCoeff.get();
            if (newValue >= 1.0)
            {
                newValue = 1.0;
                m_state.set(State::decay);
            }
            m_lastEnvValue.set(newValue);
            break;

        case State::decay:
            newValue = m_decayBase.get() + m_lastEnvValue.get() * m_decayCoeff.get();
            if (newValue <= m_sustain.getCurrentValue())
            {
                newValue = m_sustain.getCurrentValue();
                m_state.set(State::sustain);
            }
            m_lastEnvValue.set(newValue);
            break;

        case State::sustain:
            break;
        
        case State::release:
            newValue = m_releaseBase.get() + m_lastEnvValue.get() * m_releaseCoeff.get();
            if (newValue <= 0.0)
            {
                newValue = 0.0;
                m_state.set(State::idle);
            }
            m_lastEnvValue.set(newValue);
            break;
    }
}

}//namespace engine