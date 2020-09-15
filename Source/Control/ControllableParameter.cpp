/*
  ==============================================================================

    ControllableParameter.cpp
    Created: 30 Aug 2020 5:51:48pm
    Author:  maxime

  ==============================================================================
*/

#include "ControllableParameter.h"

namespace control
{

//==============================================================================
/**
 * @brief Pimpl idiom implementation
 * 
 */
struct ControllableParameter::Impl : public juce::ChangeBroadcaster
{
    Impl(float initValue, float minValue, 
            float maxValue, ScaleType scale, int discretRange)
        : m_discretRange(discretRange),
          m_precomputedValues(),
          m_maxValue(maxValue),
          m_minValue(minValue),
          m_currentDiscretValue(0)
    {
        jassert(discretRange > 0);

        if (initValue < m_minValue || initValue > m_maxValue)
        {
            initValue = m_minValue;
        }

        switch (scale)
        {
        case ScaleType::exponential:
            m_currentDiscretValue.set(precomputeExpValues(initValue));
            break;
        
        case ScaleType::logarithmic:
            m_currentDiscretValue.set(precomputeLogValues(initValue));
            break;

        default:
            m_currentDiscretValue.set(precomputeLinearValues(initValue));
            break;
        }
    }

    int precomputeLinearValues(double initValue)
    {
        jassert(m_precomputedValues.empty());
        m_precomputedValues.resize(m_discretRange);
        m_precomputedValues[0] = m_minValue;
        m_precomputedValues[m_discretRange - 1] = m_maxValue;

        auto step = (m_maxValue - m_minValue) / m_discretRange;
        auto base = m_minValue;
        int discretInitValue = 0;

        for (auto i = 1; i < m_discretRange-1; ++i)
        {
            base += step;
            m_precomputedValues[i] = base;
            if (base <= initValue)
            {
                ++discretInitValue;
            }
        }

        if (initValue == m_maxValue)
        {
            return m_discretRange - 1;
        }
        else
        {
            return discretInitValue;
        }
    }

    int precomputeLogValues(double initValue)
    {
        jassert(m_precomputedValues.empty());
        m_precomputedValues.resize(m_discretRange);
        m_precomputedValues[0] = m_minValue;
        m_precomputedValues[m_discretRange - 1] = m_maxValue;
        int discretInitValue = 0;

        if (initValue == m_maxValue)
        {
            discretInitValue = m_discretRange - 1;
        }

        auto upperLogBound = log2(m_maxValue);
        auto lowerLogBound = log2(m_minValue);
        auto diffLogBound = upperLogBound - lowerLogBound; 

        // Using recursive multiplication could introduce more error than wanted
        // This algorithm may be re-rewritten for efficiency if the current
        // trade-off efficiency/accuracy was not properly estimated
        auto step = diffLogBound / m_discretRange;
        auto base = lowerLogBound;

        for (auto i = 1; i < m_discretRange-1; ++i)
        {
            base += step;
            m_precomputedValues[i] = pow(2, base);

            if (m_precomputedValues[i] <= initValue)
            {
                ++discretInitValue;
            }
        }
        
        return discretInitValue;
    }
    int precomputeExpValues(double initValue)
    {
        jassert(m_precomputedValues.empty());
        m_precomputedValues.resize(m_discretRange);
        m_precomputedValues[0] = m_minValue;
        m_precomputedValues[m_discretRange - 1] = m_maxValue;
        int discretInitValue = 0;

        if (initValue == m_maxValue)
        {
            discretInitValue = m_discretRange - 1;
        }

        auto upperExpBound = pow(2, m_maxValue);
        auto lowerExpBound = pow(2, m_minValue);
        auto diffExpBound = upperExpBound - lowerExpBound; 

        // Using recursive multiplication could introduce more error than wanted
        // This algorithm may be re-rewritten for efficiency if the current
        // trade-off efficiency/accuracy was not properly estimated
        auto step = diffExpBound / m_discretRange;
        auto base = lowerExpBound;

        for (auto i = 1; i < m_discretRange-1; ++i)
        {
            base += step;
            m_precomputedValues[i] = log2(base);

            if (m_precomputedValues[i] <= initValue)
            {
                ++discretInitValue;
            }
        }

        return discretInitValue;
    }

    // Unmutable members
    int                                 m_discretRange;
    std::vector<float>                  m_precomputedValues;
    float                               m_maxValue;
    float                               m_minValue;

    // Mutable members
    juce::Atomic<int>                   m_currentDiscretValue;
    juce::Atomic<float>                 m_currentRatio;
    juce::CriticalSection               m_discretValueWritingSection;
};

//==============================================================================
ControllableParameter::ControllableParameter(float initValue, 
        float minValue, float maxValue, ScaleType scale, int discretRange)
    : m_impl(std::make_shared<Impl>(initValue, minValue, maxValue, scale, discretRange))
{
    // Nothing to do here
}

ControllableParameter::ControllableParameter()
    : m_impl(nullptr)
{
    // Nothing to do here
}

//==============================================================================
bool ControllableParameter::operator==(juce::ChangeBroadcaster* source)
{
    return dynamic_cast<Impl*>(source) == m_impl.get();
}

bool ControllableParameter::isValid()
{
    return m_impl != nullptr;
}

float ControllableParameter::getCurrentValue()
{
    jassert(m_impl != nullptr);

    if (m_impl != nullptr)
    {
        return m_impl->m_precomputedValues[m_impl->m_currentDiscretValue.get()];
    }

    return -1.0; // To avoid potential division by zero
}

float ControllableParameter::getUnscaledRatioForCurrentValue()
{
    jassert(m_impl != nullptr);

    if (m_impl != nullptr)
    {
        return float(m_impl->m_currentDiscretValue.get()) / (m_impl->m_discretRange - 1);
    }

    return 0.0;
}

int ControllableParameter::getDiscretRange()
{
    jassert(m_impl != nullptr);

    if (m_impl != nullptr)
    {
        return m_impl->m_discretRange;
    }

    return 0;
}

float ControllableParameter::getScaledValueForUnscaledRatio(float ratio)
{
    jassert(m_impl != nullptr);
    jassert(ratio >= 0.0 && ratio <= 1.0);

    if (m_impl != nullptr && ratio >= 0.0 && ratio <= 1.0)
    {
        int discretIndex = (m_impl->m_discretRange - 1) * ratio;
        return m_impl->m_precomputedValues[discretIndex];
    }

    return -1.0; // To avoid potential division by zero
}

//==============================================================================
void ControllableParameter::updateCurrentDiscretValue(int delta)
{
    jassert(m_impl != nullptr);

    if (m_impl != nullptr && delta != 0)
    {
        auto newDiscretValue = m_impl->m_currentDiscretValue.get() + delta;

        if (newDiscretValue > m_impl->m_discretRange - 1)
        {
            newDiscretValue = m_impl->m_discretRange - 1;
        }
        else if (newDiscretValue < 0)
        {
            newDiscretValue = 0;
        }

        m_impl->m_currentDiscretValue.set(newDiscretValue);
        m_impl->sendChangeMessage();
    }
}

void ControllableParameter::addListener(juce::ChangeListener* listener)
{
    jassert(m_impl != nullptr);
    
    if (m_impl != nullptr)
    {
        juce::MessageManagerLock lock;
        m_impl->addChangeListener(listener);
    }
}

} // namespace control
