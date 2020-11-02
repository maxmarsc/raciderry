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
#include "Utils/Parameters.h"

namespace engine
{

constexpr float ENV_MOD_RATIO_AMMOUNT = 0.5;
constexpr float ACCENT_RATIO_AMMOUNT = 0.25;

Filter::Filter(NoiseGenerator& noiseGenerator)
    : m_oberheimFilter(nullptr),
      m_open303Filter(),
      m_noiseGenerator(noiseGenerator),
      m_mixBuffer(),
      m_cutoffFreq(),
      m_resonance(),
      m_drive(),
      m_envMod(),
      m_accent()
{
    auto* midiBroker = control::MidiBroker::getInstance();
    m_cutoffFreq = midiBroker->getParameter(identifiers::controls::CUTOFF);
    m_resonance = midiBroker->getParameter(identifiers::controls::RESONANCE);
    m_envMod = midiBroker->getParameter(identifiers::controls::ENV_MOD);
    m_accent = midiBroker->getParameter(identifiers::controls::ACCENT);
    m_filtersMix = midiBroker->getParameter(identifiers::controls::FILTER_MIX);

    // Set the filter to the proper mode
    m_open303Filter.setMode(rosic::TeeBeeFilter::TB_303);
    m_open303Filter.setFeedbackHighpassCutoff(180);
}

//==============================================================================
void Filter::prepare(float sampleRate, int blockSize)
{
    m_oberheimFilter = std::make_unique<OberheimVariationMoog>(sampleRate);
    m_open303Filter.setSampleRate(sampleRate);
    m_mixBuffer.setSize(1, blockSize);
}

void Filter::reset()
{
    m_oberheimFilter.reset();
    m_mixBuffer.clear();
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

    auto cutoffRatio = m_cutoffFreq.getUnscaledRatioForCurrentValue()
            + envModRatio + accentRatio;

    float modulatedCutoff;
    if (cutoffRatio > 1.0)
    {
        // Custom compute to allow accent note to go higher than the max of
        // the cutoff parameter
        auto pow = cutoffRatio * cutoffRatio;
        modulatedCutoff = pow * m_cutoffFreq.getScaledValueForUnscaledRatio(1.0);
    }
    else
    {
        modulatedCutoff = m_cutoffFreq.getScaledValueForUnscaledRatio(cutoffRatio);
    }

    // Update of both the filters
    m_oberheimFilter->SetCutoff(modulatedCutoff * m_noiseGenerator.getNoiseFactor());
    m_oberheimFilter->SetResonance(m_resonance.getCurrentValue() * m_noiseGenerator.getNoiseFactor());
    m_open303Filter.setCutoff(modulatedCutoff * m_noiseGenerator.getNoiseFactor());
    m_open303Filter.setResonance(m_resonance.getCurrentValue() * m_noiseGenerator.getNoiseFactor() * 100 / parameters::values::RESONANCE_MAX);

    // Prepare audio buffers for processing    
    auto outputBlock = context.getOutputBlock();
    auto* data1 = outputBlock.getChannelPointer(0);
    m_mixBuffer.copyFrom(0, 0, data1, outputBlock.getNumSamples());
    auto* data2 = m_mixBuffer.getWritePointer(0);
    auto mixRatio = m_filtersMix.getCurrentValue();

    // Process with Open303 filter
    for (auto i = 0; i < outputBlock.getNumSamples(); ++i)
    {
        // Directly applying the mix gain
        *data1 = mixRatio * m_open303Filter.getSample(*data1);
        ++data1;
    }

    // Process with Oberheim filter
    m_oberheimFilter->Process(data2, outputBlock.getNumSamples());

    // Mix the two filters outputs
    m_mixBuffer.applyGain(1.0 - mixRatio);
    outputBlock.add(juce::dsp::AudioBlock<float>(m_mixBuffer));
}

} // namespace engine
