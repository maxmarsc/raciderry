/*
  ==============================================================================

    ADSR.cpp
    Created: 21 Aug 2020 8:33:48pm
    Author:  maxime

  ==============================================================================
*/

#include "ADSR.h"

#include "Engine/SignalBus.h"

#include "Control/MidiBroker.h"

#include "Utils/Identifiers.h"


namespace engine {

constexpr double ATTACK_RATIO = 0.3;
constexpr double DECAY_RATIO = 0.0001;
constexpr double RELEASE_RATIO = 0.0001;

ADSR::ADSR()
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

void ADSR::changeListenerCallback(juce::ChangeBroadcaster* source)
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

void ADSR::setSampleRate(double sampleRate)
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

void ADSR::reset()
{
    m_state = State::idle;
    m_lastEnvValue.set(0);
}

void ADSR::noteOn()
{
    // custom made kindof legato
    if (m_state.get() == State::idle || m_state.get() == State::release) 
    {
        m_state.set(State::attack);
    }
}

void ADSR::noteOff()
{
    if (m_state.get() != State::idle)
    {
        m_state.set(State::release);
    }
}

bool ADSR::isActive()
{
    return m_state.get() != State::idle;
}

void ADSR::applyEnvelopeToBuffer(juce::AudioBuffer<float>& buffer, 
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

void ADSR::updateAttack()
{
    // DBG("New attack : " + juce::String(attack));
    jassert(m_attack.getCurrentValue() > 0.);
    m_attackNumSamples.set(m_attack.getCurrentValue() * m_sampleRate.get());
    m_attackCoeff.set(computeEnvCoeff(m_attackNumSamples.get(), ATTACK_RATIO));
    m_attackBase.set((1.0 + ATTACK_RATIO) * (1.0 - m_attackCoeff.get()));
}

void ADSR::updateDecay()
{
    jassert(m_decay.getCurrentValue() > 0.);
    // DBG("New decay : " + juce::String(decay));
    m_decayNumSamples.set(m_decay.getCurrentValue() * m_sampleRate.get());
    m_decayCoeff.set(computeEnvCoeff(m_decayNumSamples.get(), DECAY_RATIO));
    m_decayBase.set((m_sustain.getCurrentValue() - DECAY_RATIO) * (1.0 - m_decayCoeff.get()));
}

void ADSR::updateRelease()
{
    jassert(m_release.getCurrentValue() > 0.);
    // DBG("New release : " + juce::String(release));
    releaseNumSamples_.set(m_release.getCurrentValue() * m_sampleRate.get());
    m_releaseCoeff.set(computeEnvCoeff(releaseNumSamples_.get(), RELEASE_RATIO));
    m_releaseBase.set( - RELEASE_RATIO * (1.0 - m_releaseCoeff.get()));
}

void ADSR::updateSustain()
{
    jassert(m_sustain.getCurrentValue() >= 0. && m_sustain.getCurrentValue() <= 1.0);
    // DBG("New sustain : " + juce::String(sustain));
    m_decayBase.set((m_sustain.getCurrentValue() - DECAY_RATIO) * (1.0 - m_decayCoeff.get()));
}

void ADSR::computeNextEnvValue()
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

double ADSR::computeEnvCoeff(int rateInSample, double targetRatio)
{
    if (rateInSample > 0)
    {
        // May need some optimization if too cpu-hungry
        return exp(-log((1.0 + targetRatio) / targetRatio) / rateInSample);
    }
    return 0.;
}

}//namespace engine