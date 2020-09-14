/*
  ==============================================================================

    ControllableParameter.h
    Created: 30 Aug 2020 5:51:48pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace control
{

class ControllableParameter
{
public:
    enum class ScaleType {linear, exponential, logarithmic};

    ControllableParameter();
    ControllableParameter(float initValue, float minValue, 
            float maxValue, ScaleType scale = ScaleType::linear, int discretRange = 128);

    bool operator==(juce::ChangeBroadcaster* source);       // Non atomic

    bool isValid();

    void addListener(juce::ChangeListener* listener);       // Non atomic

    float getCurrentValue();
    float getCurrentRatio();
    int getDiscretRange();
    float getScaledValueForRatio(float ratio);

    // Should not be called from the audio thread
    void updateCurrentDiscretValue(int delta);


private:
    class Impl;

    std::shared_ptr<Impl>           m_impl;
};

} // namespace control
