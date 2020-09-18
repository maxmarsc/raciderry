/*
  ==============================================================================

    AccentEnvelope.cpp
    Created: 15 Sep 2020 6:13:02pm
    Author:  maxime

  ==============================================================================
*/

#include "AccentEnvelope.h"

#include "Engine/Envelopes/Utils.h"
#include "Engine/SignalBus.h"

#include "Control/MidiBroker.h"

#include "Utils/Identifiers.h"

namespace engine
{

// Empirical values
constexpr double ATTACK_RATIO = 0.3;
constexpr double DECAY_RATIO = 0.0001;
constexpr double ATTACK_S = 0.005;

AccentEnvelope::AccentEnvelope()
    : m_decay(),
      m_crtMax(1.0),
      m_state(State::idle)
{
    // bind to midi broker
    auto* midiBroker = control::MidiBroker::getInstance();
    m_decay = midiBroker->getParameter(identifiers::controls::ACCENT_DECAY);
    jassert(m_decay.isValid());
    m_decay.addListener(this);
    updateAttack();
    updateDecay();
}

//==============================================================================
void AccentEnvelope::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if (m_decay == source)
    {
        updateDecay();
    }
}

void AccentEnvelope::setSampleRate(double sampleRate)
{
    jassert(sampleRate > 0.);

    if (sampleRate != m_sampleRate.get())
    {
        m_sampleRate.set(sampleRate);

        // Recompute the rates by calling the setters
        updateAttack();
        updateDecay();
    }
}

void AccentEnvelope::reset()
{
    m_state = State::idle;
    m_lastEnvValue.set(0);
}

void AccentEnvelope::noteOn()
{
    auto state = m_state.get();

    if (state != State::idle)
    {
        auto newMax = m_crtMax.get() + 1.0 / (10.0 * m_crtMax.get());
        
        if (newMax > 2.0)
        {
            newMax = 2.0;
        }

        m_crtMax.set(newMax);
    }

    m_state.set(State::attack);
}

void AccentEnvelope::noteOff()
{
    // Nothing to do here
}

void AccentEnvelope::nextValue(int numSamples)
{
    jassert(numSamples > 0);

    auto sum = float(0.);
    auto count = numSamples;

    while(numSamples--)
    {
        computeNextEnvValue();
        sum += m_lastEnvValue.get();
    }

    // We send the mean value in the signal bus
    auto* signalBus = SignalBus::getInstanceWithoutCreating();

    if (signalBus != nullptr)
    {
        signalBus->updateSignal(SignalBus::SignalId::AEG, sum/count);
    }
}

void AccentEnvelope::updateAttack()
{
    auto atkNumSamples = int(ATTACK_S * m_sampleRate.get());
    m_attackCoeff = computeExpEnvCoeff(atkNumSamples, ATTACK_RATIO);
    m_attackBase = (1.0 + ATTACK_RATIO) * (1.0 - m_attackCoeff);
}

void AccentEnvelope::updateDecay()
{
    jassert(m_decay.getCurrentValue() > 0.);
    // DBG("New decay : " + juce::String(decay));
    auto decNumSamples = int(m_decay.getCurrentValue() * m_sampleRate.get());

    // Settings these two consecutively without mutex could lead to a 
    // computation error on one sample if the read performs between the two sets
    // However the computation error is bounded, inaudible, and allows lock-free
    // updates
    m_decayCoeff.set(computeExpEnvCoeff(decNumSamples, DECAY_RATIO));
    m_decayBase.set(( - DECAY_RATIO ) * (1.0 - m_decayCoeff.get()));
}

void AccentEnvelope::computeNextEnvValue()
{
    float newValue;

    switch(m_state.get())
    {
        case State::idle:
            break;

        case State::attack:
            newValue = m_attackBase + m_lastEnvValue.get() * m_attackCoeff;
            if (newValue >= m_crtMax.get())
            {
                newValue = m_crtMax.get();
                m_state.set(State::decay);
            }
            m_lastEnvValue.set(newValue);
            break;

        case State::decay:
            newValue = m_decayBase.get() + m_lastEnvValue.get() * m_decayCoeff.get();
            if (newValue <= 0)
            {
                newValue = 0;
                m_state.set(State::idle);
            }
            m_lastEnvValue.set(newValue);
            break;
    }
}


} // namespace engine