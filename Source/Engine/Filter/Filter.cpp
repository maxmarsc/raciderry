/*
  ==============================================================================

    Filter.cpp
    Created: 4 Sep 2020 5:01:38pm
    Author:  maxime

  ==============================================================================
*/

#include "Filter.h"

#include "Engine/SignalBus.h"

// #include "Control/MidiBroker.h"

#include "Utils/Identifiers.h"
#include "Utils/Parameters.h"

namespace engine
{

constexpr float ENV_MOD_RATIO_AMMOUNT = 0.5;
constexpr float ACCENT_RATIO_AMMOUNT = 0.25;
constexpr float OBERHEIM_GAIN_REDUCTION = -9.0;

Filter::Filter(Bindings bindings)
    : m_oberheimFilter(nullptr),
      m_open303Filter(),
      r_noiseGenerator(bindings.r_noiseGenerator),
      r_signalBus(bindings.r_signalBus),
      m_mixBuffer(),
      m_cutoffFreq(),
      m_resonance(),
      m_drive(),
      m_envMod()
{
    // Bind to the controllable parameters
    // auto* midiBroker = control::MidiBroker::getInstance();
    auto parameterMap = bindings.m_parameterMap.lock();
    jassert(parameterMap != nullptr);

    m_cutoffFreq = (*parameterMap)[identifiers::controls::CUTOFF];
    m_resonance = (*parameterMap)[identifiers::controls::RESONANCE];
    m_envMod = (*parameterMap)[identifiers::controls::ENV_MOD];
    m_filtersMix = (*parameterMap)[identifiers::controls::FILTER_MIX];
    jassert(m_cutoffFreq.isValid());
    jassert(m_resonance.isValid());
    jassert(m_envMod.isValid());
    jassert(m_filtersMix.isValid());

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
    // auto* signalBus = SignalBus::getInstanceWithoutCreating();
    float megValue = 0.0;
    float accent = 0.0;


    megValue = r_signalBus.readSignal(SignalBus::SignalId::VEG);
    accent = r_signalBus.readSignal(SignalBus::SignalId::AEG);
    jassert(megValue >= 0.);
    jassert(accent >= 0.);

    // Compute the mod ratio
    auto envModRatio = (megValue * m_envMod.getCurrentValue() * ENV_MOD_RATIO_AMMOUNT);

    // Compute the accent ratio
    auto accentRatio = (accent * ACCENT_RATIO_AMMOUNT);

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
    m_oberheimFilter->SetCutoff(modulatedCutoff * r_noiseGenerator.getNoiseFactor());
    m_oberheimFilter->SetResonance(m_resonance.getCurrentValue() * r_noiseGenerator.getNoiseFactor());
    m_open303Filter.setCutoff(modulatedCutoff * r_noiseGenerator.getNoiseFactor());
    m_open303Filter.setResonance(m_resonance.getCurrentValue() * r_noiseGenerator.getNoiseFactor() * 100 / parameters::values::RESONANCE_MAX);

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
    // Apply general gain + custom gain reduction when resonance is high to
    // force the two filters on a same level range
    auto customGain = juce::Decibels::decibelsToGain<float>(OBERHEIM_GAIN_REDUCTION
            * m_resonance.getUnscaledRatioForCurrentValue());
    m_mixBuffer.applyGain(customGain * (1.0 - mixRatio));

    // Mix the two filters outputs
    outputBlock.add(juce::dsp::AudioBlock<float>(m_mixBuffer));
}

} // namespace engine
