/*
  ==============================================================================

    RaciderryEngine.h
    Created: 20 Aug 2020 4:34:49pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Engine/Filter/Filter.h"
#include "Engine/NoiseGenerator.h"

#include "Control/MidiBroker.h"
#include "Control/ControllableParameter.h"

namespace engine {

class DualOscillator;

/**
 * @class engine::RaciderryEngine
 * @brief The main class of the Audio Engine, holds and connect the differents 
 * audio modules 
 * 
 * This engine manages all the audio modules, init and reset them when needed, 
 * and call them in the appropriate order to produce the audio output
 */
class RaciderryEngine :   public juce::AudioIODeviceCallback
{
public:
    RaciderryEngine(std::weak_ptr<control::ParameterMap> parameterMap);
    ~RaciderryEngine();

//==============================================================================
    /**
     * @name juce::AudioIODeviceCallback overrides.
     */
    ///@{
    void audioDeviceIOCallback (const float **inputChannelData, int numInputChannels, 
            float **outputChannelData, int numOutputChannels, int numSamples) override;
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;
    ///@}

private:
//==============================================================================
    NoiseGenerator                                  m_noiseGenerator;
    SignalBus                                       m_signalBus;
    std::unique_ptr<juce::Synthesiser>              m_synth;
    std::weak_ptr<DualOscillator>                   m_oscWeakPtr;
    juce::dsp::Limiter<float>                       m_limiter;
    Filter                                          m_filter;
    
    double                                          m_blockLength;
    double                                          m_sampleRate;
};

}//namespace engine
