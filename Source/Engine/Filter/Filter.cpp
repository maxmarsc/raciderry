/*
  ==============================================================================

    Filter.cpp
    Created: 4 Sep 2020 5:01:38pm
    Author:  maxime

  ==============================================================================
*/

#include "Filter.h"

#include "Engine/SignalBus.h"

#include "Control/MidiBroker.h"

#include "Utils/Identifiers.h"

namespace engine
{

constexpr float ENV_MOD_RATIO_AMMOUNT = 0.5;
constexpr float ACCENT_RATIO_AMMOUNT = 0.25;

Filter::Filter()
    : m_oberheimFilter(nullptr),
      m_cutoffFreq(),
      m_resonance(),
      m_drive(),
      m_envMod(),
      m_accent()
{
    auto* midiBroker = control::MidiBroker::getInstance();
    m_cutoffFreq = midiBroker->getParameter(identifiers::controls::CUTOFF);
    m_resonance = midiBroker->getParameter(identifiers::controls::RESONANCE);
    m_drive = midiBroker->getParameter(identifiers::controls::DRIVE);
    m_envMod = midiBroker->getParameter(identifiers::controls::ENV_MOD);
    m_accent = midiBroker->getParameter(identifiers::controls::ACCENT);
}

//==============================================================================
void Filter::prepare(float sampleRate, int blockSize) noexcept
{
    m_oberheimFilter = std::make_unique<OberheimVariationMoog>(sampleRate);
}

void Filter::reset()
{
    m_oberheimFilter.reset();
}

void Filter::process(juce::dsp::ProcessContextReplacing<float>& context)
{
    // Get the modulation signals
    auto* signalBus = SignalBus::getInstanceWithoutCreating();
    float megValue = 0.0;
    float accent = 0.0;

    if (signalBus != nullptr)
    {
        megValue = signalBus->readSignal(SignalBus::SignalId::VEG);
        accent = signalBus->readSignal(SignalBus::SignalId::AEG);
    }

    // Compute the mod ratio
    auto envModRatio = (megValue * m_envMod.getCurrentValue() * ENV_MOD_RATIO_AMMOUNT);

    // Compute the accent ratio
    auto accentRatio = (accent * m_accent.getCurrentValue() * ACCENT_RATIO_AMMOUNT);

    auto cutoffRatio =  + m_cutoffFreq.getUnscaledRatioForCurrentValue()
            + envModRatio + accentRatio;

    if (cutoffRatio > 1.0)
    {
        cutoffRatio = 1.0;
    }
    
    auto envModCutoff = m_cutoffFreq.getScaledValueForUnscaledRatio(cutoffRatio);

    // Update the parameters
    m_oberheimFilter->SetCutoff(envModCutoff);
    m_oberheimFilter->SetResonance(m_resonance.getCurrentValue());
    m_oberheimFilter->SetSaturation(m_drive.getCurrentValue());

    // Process
    auto outputBlock = context.getOutputBlock();
    m_oberheimFilter->Process(outputBlock.getChannelPointer(0), 
            outputBlock.getNumSamples());
}

} // namespace engine
