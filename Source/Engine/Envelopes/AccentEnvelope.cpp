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

// #include "Control/MidiBroker.h"

#include "Utils/Identifiers.h"

namespace engine
{

// Empirical values
constexpr double ATTACK_RATIO = 0.3;
constexpr double DECAY_RATIO = 0.0001;
constexpr double ATTACK_S = 0.005;
constexpr float  AMOUNT_MIN = 0.2;

AccentEnvelope::AccentEnvelope(Bindings bindings)
    : m_decay(),
      m_crtMax(1.0),
      m_state(State::idle),
      m_noteAmount(0.0),
      r_signalBus(bindings.r_signalBus)
{
    // bind to controllable parameters
    // auto* midiBroker = control::MidiBroker::getInstance();
    auto parameterMap = bindings.m_parameterMap.lock();
    jassert(parameterMap != nullptr);

    m_decay = (*parameterMap)[identifiers::controls::ACCENT_DECAY];
    m_accent = (*parameterMap)[identifiers::controls::ACCENT];
    jassert(m_decay.isValid());
    jassert(m_accent.isValid());
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

void AccentEnvelope::noteOn(float velocity)
{
    auto state = m_state.get();
    m_noteAmount = velocity;

    if (state != State::idle)
    {
        // We change the max value to partially reproduce the behaviour of the
        // accent circuit when notes overlaps
        // See https://www.firstpr.com.au/rwi/dfish/303-unique.html
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
    if (m_state.get() == State::attack)
    {
        m_state.set(State::decay);
    }
}

void AccentEnvelope::nextValue(int numSamples)
{
    jassert(numSamples > 0);

    auto envBlockMax = float(0.);


    while(numSamples--)
    {
        computeNextEnvValue();
        if (m_lastEnvValue.get() > envBlockMax)
        {
            envBlockMax = m_lastEnvValue.get();
        }
    }

    // Compute the actual value once modulated
    auto envValue = m_crtMax.get() * envBlockMax;
    auto modulatedValue = envValue * (AMOUNT_MIN + m_noteAmount * m_accent.getCurrentValue());

    // We send the mean value in the signal bus
    // auto* signalBus = SignalBus::getInstanceWithoutCreating();

    // if (signalBus != nullptr)
    // {
    //     // We multiply the env value by the new max
    //     signalBus->updateSignal(SignalBus::SignalId::AEG, modulatedValue);
    // }
    r_signalBus.updateSignal(SignalBus::SignalId::AEG, modulatedValue);
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
            m_crtMax.set(1.0);
            break;

        case State::attack:
            newValue = m_attackBase + m_lastEnvValue.get() * m_attackCoeff;
            if (newValue >= 1.0)
            {
                newValue = 1.0;
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

    // DBG(juce::String(newValue));
}


} // namespace engine