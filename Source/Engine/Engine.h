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

#include "Control/MidiBroker.h"

namespace engine {

class DualOscillator;

class RaciderryEngine :   public juce::AudioIODeviceCallback
{
public:
    RaciderryEngine();
    ~RaciderryEngine();

    void audioDeviceIOCallback (const float **inputChannelData, int numInputChannels, 
            float **outputChannelData, int numOutputChannels, int numSamples) override;
    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

private:
    std::unique_ptr<juce::Synthesiser>              m_synth;
    std::weak_ptr<DualOscillator>                   m_oscWeakPtr;
    juce::dsp::Limiter<float>                       m_limiter;
    Filter                                          m_filter;
    
    double                                          m_blockLength;
    double                                          m_sampleRate;
};

}//namespace engine
