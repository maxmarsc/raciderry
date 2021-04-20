/*
  ==============================================================================

    FilterTestUnit.cpp
    Created: 20 Apr 2021 2:53:44pm
    Author:  maxime

  ==============================================================================
*/

#include "Tests/CustomTestUnit.h"
#include "Tests/Utils.h"
#include "Tests/CallDispatcher.h"

#include "Engine/Binding.h"
#include "Engine/Filter/Filter.h"
#include "Utils/Identifiers.h"

namespace tests
{


class FilterTestUnit : public CustomTestUnit
{
public:
    FilterTestUnit() : CustomTestUnit("Filter testing", 
            category::engine::oscillators),
            m_noiseGen(0.05),
            m_signalBus(),
            m_bindings{m_parameterMap, m_noiseGen, m_signalBus} {};

    void initialise() override
    {
        m_parameterMap = std::make_shared<control::ParameterMap>();
        m_bindings.m_parameterMap = m_parameterMap;
        (*m_parameterMap)[identifiers::controls::CUTOFF] = 
                control::ControllableParameter(5000, 200, 10000);
        (*m_parameterMap)[identifiers::controls::RESONANCE] = 
                control::ControllableParameter(0.5, 0.0, 1.0);
        (*m_parameterMap)[identifiers::controls::ENV_MOD] = 
                control::ControllableParameter(0.5, 0.0, 1.0);
        (*m_parameterMap)[identifiers::controls::FILTER_MIX] = 
                control::ControllableParameter(0.5, 0.0, 1.0);
    }

    void shutdown() override
    {
        m_parameterMap->clear();
    }

    void singleTestInit() override
    {
    }

    void singleTestShutdown() override
    {
    }

    void runTest() override
    {

    TEST("Constructor", [=] {
        auto filter = engine::Filter(m_bindings);
    });

    TEST("Test multiple processing context", [=] {
        auto filter = engine::Filter(m_bindings);

        float samplerates[] = {44100, 48000, 96000, 192000};
        int blockSizes[] = {128, 256, 512, 1024, 2048};

        for (auto& samplerate : samplerates)
        {
            for (auto& blockSize : blockSizes)
            {
                auto buffer = juce::AudioBuffer<float>(1, blockSize);
                auto block = juce::dsp::AudioBlock<float>(buffer);
                auto processingContext = juce::dsp::ProcessContextReplacing<float>(block);

                filter.prepare(samplerate, blockSize);
                filter.process(processingContext);
                filter.process(processingContext);
                filter.process(processingContext);
                filter.reset();
            }
        }
    });


    }

private:
    std::shared_ptr<control::ParameterMap>          m_parameterMap;
    engine::NoiseGenerator                          m_noiseGen;
    engine::SignalBus                               m_signalBus;
    engine::Bindings                                m_bindings;
    juce::Random                                    m_rng;
};

static FilterTestUnit                         FILTER_UNIT;

} // namespace tests
