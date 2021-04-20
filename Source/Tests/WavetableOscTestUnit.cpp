/*
  ==============================================================================

    WavetableOscTestUnit.cpp
    Created: 19 Apr 2021 5:55:46pm
    Author:  maxime

  ==============================================================================
*/


#include "Tests/CustomTestUnit.h"
#include "Tests/Utils.h"
#include "Tests/CallDispatcher.h"

#include "Engine/Binding.h"
#include "Engine/Oscillators/WavetableOscillator.h"

namespace tests
{

constexpr auto BLOCK_SIZE = 2048;

class WavetableOscTestUnit : public CustomTestUnit
{
public:
    WavetableOscTestUnit() : CustomTestUnit("Wavetable Osc testing", 
            category::engine::oscillators),
            m_waveform(1, 2048),
            m_noiseGen(0.05),
            m_signalBus(),
            m_bindings{m_parameterMap, m_noiseGen, m_signalBus} {};

    void initialise() override
    {
        m_parameterMap = std::make_shared<control::ParameterMap>();
        m_bindings.m_parameterMap = m_parameterMap;
    }

    void shutdown() override
    {
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
        auto osc = engine::WavetableOscillator(m_waveform, m_bindings);
    });

    TEST("Test multiple processing context", [=] {
        auto osc = engine::WavetableOscillator(m_waveform, m_bindings);

        float samplerates[] = {44100, 48000, 96000, 192000};
        int blockSizes[] = {128, 256, 512, 1024, 2048};
        

        for (auto& samplerate : samplerates)
        {
            for (auto& blockSize : blockSizes)
            {
                auto buffer = juce::AudioBuffer<float>(1, blockSize);
                auto block = juce::dsp::AudioBlock<float>(buffer);
                auto processingContext = juce::dsp::ProcessContextReplacing<float>(block);

                osc.prepare(samplerate, blockSize);
                osc.setGlide(m_rng.nextFloat()*1.5);
                osc.process(processingContext);
                osc.setGlide(m_rng.nextFloat()*1.5);
                osc.process(processingContext);
                osc.setGlide(m_rng.nextFloat()*1.5);
                osc.process(processingContext);
                osc.reset();
            }
        }
    });


    }

private:
    juce::AudioSampleBuffer                         m_waveform;
    std::shared_ptr<control::ParameterMap>          m_parameterMap;
    engine::NoiseGenerator                          m_noiseGen;
    engine::SignalBus                               m_signalBus;
    engine::Bindings                                m_bindings;
    juce::Random                                    m_rng;
};

static WavetableOscTestUnit                         WAVETABLEOSC_UNIT;

} // namespace tests
