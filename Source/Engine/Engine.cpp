/*
  ==============================================================================

    RaciderryEngine.cpp
    Created: 20 Aug 2020 4:34:49pm
    Author:  maxime

  ==============================================================================
*/

#include "Engine.h"

#include "Engine/Sound.h"
#include "Engine/Voice.h"
#include "Engine/Oscillators/DualOscillator.h"
#include "Engine/SignalBus.h"
#include "Engine/Binding.h"

#include "Utils/Parameters.h"

namespace engine {

RaciderryEngine::RaciderryEngine(control::MidiBroker& midiBroker)
    : r_midiBroker(midiBroker),
      m_noiseGenerator(0.03),
      m_signalBus(),
      m_synth(std::make_unique<juce::Synthesiser>()),
      m_oscWeakPtr(),
      m_limiter(),
      m_filter({midiBroker.getIdToParameterMap(), m_noiseGenerator, m_signalBus}),
      m_blockLength(0),
      m_sampleRate(0.)
{
    // Init the Voice and Sound for the synth
    auto voice = std::make_unique<Voice>(Bindings({
            midiBroker.getIdToParameterMap(), 
            m_noiseGenerator, 
            m_signalBus}));
    auto sound = std::make_unique<Sound>();

    // Get a weak pointer to the osc to update its samplerate/blocksize
    m_oscWeakPtr = voice->getOscPtr();

    // Gives the Voice and Sound to the Synth
    m_synth->addVoice(voice.release());
    m_synth->addSound(sound.release());
    m_synth->setNoteStealingEnabled(true);

    // Set the limiter
    m_limiter.setRelease(parameters::values::LIMITER_RELEASE_MS);
    m_limiter.setThreshold(parameters::values::LIMITER_THRESHOLD_DB);
}

RaciderryEngine::~RaciderryEngine()
{
}

//==============================================================================
void RaciderryEngine::audioDeviceIOCallback(const float **inputChannelData, 
                                              int numInputChannels, 
                                              float **outputChannelData, 
                                              int numOutputChannels, 
                                              int numSamples)
{
    /* 
    * This engine is a mono synth, but the pisound has a stereo output. In order
    * to be compatible with both stereo and mono cables, we need to duplicate
    * our mono signal on both channel with -3dB attenuation on each.
    */
    jassert(numInputChannels == 0);
    jassert(numOutputChannels == 2);

    // These three structures points toward the same memory block
    auto outputBuffer = juce::AudioBuffer<float>(outputChannelData, 1, numSamples);
    auto outputBlock = juce::dsp::AudioBlock<float>(
        outputBuffer.getArrayOfWritePointers(),
        outputBuffer.getNumChannels(),
        0,
        numSamples
    );
    auto outputContext = juce::dsp::ProcessContextReplacing(outputBlock);

    // 1. The synth produces the main output
    m_synth->renderNextBlock(outputBuffer, r_midiBroker.getNoteMidiBuffer(), 0, numSamples);

    // 2. We apply the filter on the synth output
    m_filter.process(outputContext);

    // 3. We use the limiter to prevent the audio from saturating
    m_limiter.process(outputContext);


    // 4. We duplicate and attenuate the signal to produce stereo output from
    // our mono signal
    outputBuffer.applyGain(0.5);
    memcpy(outputChannelData[1], outputChannelData[0], numSamples * sizeof(float));
}

void RaciderryEngine::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    m_sampleRate = device->getCurrentSampleRate();
    auto numSamples = juce::uint32(device->getCurrentBufferSizeSamples());
    m_blockLength = numSamples / m_sampleRate;
    DBG(juce::String("About to start : ") + juce::String(m_sampleRate) + " : "  + juce::String(numSamples));

    m_synth->setCurrentPlaybackSampleRate(m_sampleRate);
    m_limiter.prepare({m_sampleRate, numSamples, 1});
    m_filter.prepare(m_sampleRate, numSamples);

    if (auto safePtr = m_oscWeakPtr.lock())
    {
        safePtr->prepare(m_sampleRate, numSamples);
    }
}

void RaciderryEngine::audioDeviceStopped()
{
    m_blockLength = 0.;
    m_sampleRate = 0.;

    m_limiter.reset();
    m_filter.reset();
    
    if (auto safePtr = m_oscWeakPtr.lock())
    {
        safePtr->reset();
    }
}


}//namespace engine