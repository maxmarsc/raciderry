/*
  ==============================================================================

    DualOscTestUnit.cpp
    Created: 20 Apr 2021 11:48:13am
    Author:  maxime

  ==============================================================================
*/

#include "Tests/CustomTestUnit.h"
#include "Tests/Utils.h"
#include "Tests/CallDispatcher.h"

#include "Engine/Binding.h"
#include "Engine/Oscillators/DualOscillator.h"
#include "Utils/Identifiers.h"

namespace tests
{

constexpr auto BLOCK_SIZE = 2048;

class DualOscTestUnit : public CustomTestUnit
{
public:
    DualOscTestUnit() : CustomTestUnit("Dual wt osc testing", 
            category::engine::oscillators),
            m_noiseGen(0.05),
            m_signalBus(),
            m_bindings{m_parameterMap, m_noiseGen, m_signalBus} {};

    void initialise() override
    {
        m_parameterMap = std::make_shared<control::ParameterMap>();
        m_bindings.m_parameterMap = m_parameterMap;
        (*m_parameterMap)[identifiers::controls::WAVEFORM_RATIO] = 
                control::ControllableParameter(0.5, 0.0, 1.0);
        (*m_parameterMap)[identifiers::controls::GLIDE] = 
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
        auto osc = engine::DualOscillator(m_bindings);
    });

    TEST("Test multiple processing context", [=] {
        auto osc = engine::DualOscillator(m_bindings);

        float samplerates[] = {44100, 48000, 96000, 192000};
        int blockSizes[] = {128, 256, 512, 1024, 2048};
        auto buffer = juce::AudioBuffer<float>(1, 2048);
        

        for (auto& samplerate : samplerates)
        {
            for (auto& blockSize : blockSizes)
            {
                osc.prepare(samplerate, blockSize);
                osc.process(buffer, 0, blockSize);
                osc.process(buffer, 0, blockSize);
                osc.process(buffer, 0, blockSize);
                osc.reset();
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

static DualOscTestUnit                         DUALOSC_UNIT;

} // namespace tests