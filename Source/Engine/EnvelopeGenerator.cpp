/*
  ==============================================================================

    ADSR.cpp
    Created: 21 Aug 2020 8:33:48pm
    Author:  maxime

  ==============================================================================
*/

#include "EnvelopeGenerator.h"

#include "Engine/SignalBus.h"

#include "Control/MidiBroker.h"

#include "Utils/Identifiers.h"


namespace engine {

// Empirical values
constexpr double ATTACK_RATIO = 0.3;
constexpr double DECAY_RATIO = 0.0001;
constexpr double RELEASE_RATIO = 0.0001;

EnvelopeGenerator::EnvelopeGenerator()
    : m_attack(),
      m_decay(),
      m_sustain(),
      m_release(),
      m_state(State::idle)
{
    // bind to midi broker
    auto* midiBroker = control::MidiBroker::getInstance();
    m_attack = midiBroker->getParameter(identifiers::controls::ATTACK);
    m_decay = midiBroker->getParameter(identifiers::controls::DECAY);
    m_sustain = midiBroker->getParameter(identifiers::controls::SUSTAIN);
    m_release = midiBroker->getParameter(identifiers::controls::RELEASE);
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

void EnvelopeGenerator::changeListenerCallback(juce::ChangeBroadcaster* source)
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

void EnvelopeGenerator::setSampleRate(double sampleRate)
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

void EnvelopeGenerator::reset()
{
    m_state = State::idle;
    m_lastEnvValue.set(0);
}

void EnvelopeGenerator::noteOn()
{
    // custom made kindof legato
    if (m_state.get() == State::idle || m_state.get() == State::release) 
    {
        m_state.set(State::attack);
    }
}

void EnvelopeGenerator::noteOff()
{
    if (m_state.get() != State::idle)
    {
        m_state.set(State::release);
    }
}

void EnvelopeGenerator::applyAmpEnvelopeToBuffer(juce::AudioBuffer<float>& buffer, 
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
    auto* signalBus = SignalBus::getInstanceWithoutCreating();
    if (signalBus != nullptr)
    {
        signalBus->updateSignal(SignalBus::SignalId::VEG, sum/count);
    }
}

void EnvelopeGenerator::updateAttack()
{
    // DBG("New attack : " + juce::String(attack));
    jassert(m_attack.getCurrentValue() > 0.);
    auto atkNumSamples = int(m_attack.getCurrentValue() * m_sampleRate.get());

    // Settings these two consecutively without mutex could lead to a 
    // computation error on one sample if the read performs between the two sets
    // However the computation error is bounded, inaudible, and allows lock-free
    // updates
    m_attackCoeff.set(computeEnvCoeff(atkNumSamples, ATTACK_RATIO));
    m_attackBase.set((1.0 + ATTACK_RATIO) * (1.0 - m_attackCoeff.get()));
}

void EnvelopeGenerator::updateDecay()
{
    jassert(m_decay.getCurrentValue() > 0.);
    // DBG("New decay : " + juce::String(decay));
    auto decNumSamples = int(m_decay.getCurrentValue() * m_sampleRate.get());

    // Settings these two consecutively without mutex could lead to a 
    // computation error on one sample if the read performs between the two sets
    // However the computation error is bounded, inaudible, and allows lock-free
    // updates
    m_decayCoeff.set(computeEnvCoeff(decNumSamples, DECAY_RATIO));
    m_decayBase.set((m_sustain.getCurrentValue() - DECAY_RATIO) * (1.0 - m_decayCoeff.get()));
}

void EnvelopeGenerator::updateRelease()
{
    jassert(m_release.getCurrentValue() > 0.);
    // DBG("New release : " + juce::String(release));
    auto relNumSamples = int(m_release.getCurrentValue() * m_sampleRate.get());

    // Settings these two consecutively without mutex could lead to a 
    // computation error on one sample if the read performs between the two sets
    // However the computation error is bounded, inaudible, and allows lock-free
    // updates
    m_releaseCoeff.set(computeEnvCoeff(relNumSamples, RELEASE_RATIO));
    m_releaseBase.set( - RELEASE_RATIO * (1.0 - m_releaseCoeff.get()));
}

void EnvelopeGenerator::updateSustain()
{
    jassert(m_sustain.getCurrentValue() >= 0. && m_sustain.getCurrentValue() <= 1.0);
    // DBG("New sustain : " + juce::String(sustain));
    m_decayBase.set((m_sustain.getCurrentValue() - DECAY_RATIO) * (1.0 - m_decayCoeff.get()));
}

void EnvelopeGenerator::computeNextEnvValue()
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

double EnvelopeGenerator::computeEnvCoeff(int rateInSample, double targetRatio)
{
    if (rateInSample > 0)
    {
        // May need some optimization if too cpu-hungry
        return exp(-log((1.0 + targetRatio) / targetRatio) / rateInSample);
    }
    return 0.;
}

}//namespace engine