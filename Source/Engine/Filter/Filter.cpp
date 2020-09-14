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

constexpr float ACCENT_RATIO_AMMOUNT = 0.5;

Filter::Filter()
    : m_ladderFilter(),
      m_oberheimFilter(nullptr),
      m_cutoffFreq(),
      m_resonance(),
      m_drive()
{
    m_ladderFilter.setEnabled(true);
    m_ladderFilter.setMode(juce::dsp::LadderFilter<float>::Mode::LPF24);

    auto* midiBroker = control::MidiBroker::getInstance();
    m_cutoffFreq = midiBroker->getParameter(identifiers::controls::CUTOFF);
    m_resonance = midiBroker->getParameter(identifiers::controls::RESONANCE);
    m_drive = midiBroker->getParameter(identifiers::controls::DRIVE);
    m_accent = midiBroker->getParameter(identifiers::controls::ACCENT);
}

void Filter::prepare(float sampleRate, int blockSize) noexcept
{
    // m_ladderFilter.prepare({sampleRate, juce::uint32(blockSize), 1});
    m_oberheimFilter = std::make_unique<OberheimVariationMoog>(sampleRate);
}

void Filter::reset()
{
    // m_ladderFilter.reset();
    m_oberheimFilter.reset();
}

void Filter::process(juce::dsp::ProcessContextReplacing<float>& context)
{
    /// Compute the accented cutoff frequency
    auto* signalBus = SignalBus::getInstanceWithoutCreating();
    float megValue = 0.0;
    if (signalBus != nullptr)
    {
        megValue = signalBus->readSignal(SignalBus::SignalId::MEG);
    }
    auto accentedRatio = (megValue * m_accent.getCurrentValue() * ACCENT_RATIO_AMMOUNT)
            + m_cutoffFreq.getCurrentRatio();
    if (accentedRatio > 1.0)
    {
        accentedRatio = 1.0;
    }
    auto accentedCutoff = m_cutoffFreq.getScaledValueForRatio(accentedRatio);

    // Update the parameters
    m_oberheimFilter->SetCutoff(accentedCutoff);
    m_oberheimFilter->SetResonance(m_resonance.getCurrentValue());
    m_oberheimFilter->SetSaturation(m_drive.getCurrentValue());

    auto outputBlock = context.getOutputBlock();
    m_oberheimFilter->Process(outputBlock.getChannelPointer(0), 
            outputBlock.getNumSamples());
}

} // namespace engine
