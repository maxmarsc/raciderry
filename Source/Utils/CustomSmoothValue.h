/*
  ==============================================================================

    CustomSmoothValue.h
    Created: 8 Oct 2020 3:21:56pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace utils {

/**
 * @brief A custom implementation of the juce::SmoothedValue class, which allows
 * ramp length update without resetting the current value using the update()
 * methods()
 * 
 * @see JUCE/modules/juce_audio_basics/utilities/juce_SmoothedValue.h
 * 
 * Multiplicative smoothing is useful when you are dealing with
 * exponential/logarithmic values like volume in dB or frequency in Hz. For
 * example a 12 step ramp from 440.0 Hz (A4) to 880.0 Hz (A5) will increase the
 * frequency with an equal temperament tuning across the octave. A 10 step
 * smoothing from 1.0 (0 dB) to 3.16228 (10 dB) will increase the value in
 * increments of 1 dB.
 *
 * Note that when you are using multiplicative smoothing you cannot ever reach a
 * target value of zero!
 */
template <typename FloatType, typename SmoothingType = juce::ValueSmoothingTypes::Linear>
class CustomSmoothedValue   : public juce::SmoothedValueBase <CustomSmoothedValue <FloatType, SmoothingType>>
{
public:
    //==============================================================================
    /** Constructor. */
    CustomSmoothedValue() noexcept
        : CustomSmoothedValue ((FloatType) (std::is_same<SmoothingType, juce::ValueSmoothingTypes::Linear>::value ? 0 : 1))
    {
    }

    /** Constructor. */
    CustomSmoothedValue (FloatType initialValue) noexcept
    {
        // Multiplicative smoothed values cannot ever reach 0!
        jassert (! (std::is_same<SmoothingType, juce::ValueSmoothingTypes::Multiplicative>::value && initialValue == 0));

        // Visual Studio can't handle base class initialisation with CRTP
        this->currentValue = initialValue;
        this->target = this->currentValue;
    }

    //==============================================================================
    /** Reset to a new sample rate and ramp length.
        @param sampleRate           The sample rate
        @param rampLengthInSeconds  The duration of the ramp in seconds
    */
    void reset (double sampleRate, double rampLengthInSeconds) noexcept
    {
        jassert (sampleRate > 0 && rampLengthInSeconds >= 0);
        reset ((int) std::floor (rampLengthInSeconds * sampleRate));
    }

    /**
     * @brief Set a new ramp length, without resetting current smoothing
     * 
     * @param sampleRate            The sample rate
     * @param rampLengthInSeconds   The new duration of the ramp in seconds
     */
    void update (double sampleRate, double rampLengthInSeconds) noexcept
    {
        jassert (sampleRate > 0 && rampLengthInSeconds >= 0);
        update ((int) std::floor (rampLengthInSeconds * sampleRate));
    }

    /** Set a new ramp length directly in samples.
        @param numSteps     The number of samples over which the ramp should be active
    */
    void reset (int numSteps) noexcept
    {
        stepsToTarget = numSteps;
        this->setCurrentAndTargetValue (this->target);
    }

    /**
     * @brief Set a new ramp length directly in samples, without resetting the
     * current one
     * 
     * @param numSteps      The number of samples over which the ramp should be active
     */
    void update (int numSteps) noexcept
    {
        if (! this->isSmoothing())
        {
            // Not smoothing, no need to update the step, the next target change 
            // will do it anyway
            stepsToTarget = numSteps;
            return;
        }
        else
        {
            // Smoothing, we may need to update the step
            this->countdown += (numSteps - stepsToTarget);
            stepsToTarget = numSteps;

            if (this->countdown  <= 0)
            {
                this->countdown = 0;
                return;
            }

            setStepSize();
        }
    }

    //==============================================================================
    /** Set the next value to ramp towards.
        @param newValue     The new target value
    */
    void setTargetValue (FloatType newValue) noexcept
    {
        if (newValue == this->target)
            return;

        if (stepsToTarget <= 0)
        {
            this->setCurrentAndTargetValue (newValue);
            return;
        }

        // Multiplicative smoothed values cannot ever reach 0!
        jassert (! (std::is_same<SmoothingType, juce::ValueSmoothingTypes::Multiplicative>::value && newValue == 0));

        this->target = newValue;
        this->countdown = stepsToTarget;

        setStepSize();
    }

    //==============================================================================
    /** Compute the next value.
        @returns Smoothed value
    */
    FloatType getNextValue() noexcept
    {
        if (! this->isSmoothing())
            return this->target;

        --(this->countdown);

        if (this->isSmoothing())
            setNextValue();
        else
            this->currentValue = this->target;

        return this->currentValue;
    }

    //==============================================================================
    /** Skip the next numSamples samples.
        This is identical to calling getNextValue numSamples times. It returns
        the new current value.
        @see getNextValue
    */
    FloatType skip (int numSamples) noexcept
    {
        if (numSamples >= this->countdown)
        {
            this->setCurrentAndTargetValue (this->target);
            return this->target;
        }

        skipCurrentValue (numSamples);

        this->countdown -= numSamples;
        return this->currentValue;
    }

    /**
     * @brief Skip the current ramp and goes directly to the target value
     */
    void skipRamp () noexcept
    {
        skip(this->countdown);
    }

    //==============================================================================
    /** THIS FUNCTION IS DEPRECATED.

        Use `setTargetValue (float)` and `setCurrentAndTargetValue()` instead:

        lsv.setValue (x, false); -> lsv.setTargetValue (x);
        lsv.setValue (x, true);  -> lsv.setCurrentAndTargetValue (x);

        @param newValue     The new target value
        @param force        If true, the value will be set immediately, bypassing the ramp
    */
    JUCE_DEPRECATED_WITH_BODY (void setValue (FloatType newValue, bool force = false) noexcept,
    {
        if (force)
        {
            this->setCurrentAndTargetValue (newValue);
            return;
        }

        setTargetValue (newValue);
    })

private:
    //==============================================================================
    template <typename T>
    using LinearVoid = typename std::enable_if <std::is_same <T, juce::ValueSmoothingTypes::Linear>::value, void>::type;

    template <typename T>
    using MultiplicativeVoid = typename std::enable_if <std::is_same <T, juce::ValueSmoothingTypes::Multiplicative>::value, void>::type;

    //==============================================================================
    template <typename T = SmoothingType>
    LinearVoid<T> setStepSize() noexcept
    {
        step = (this->target - this->currentValue) / (FloatType) this->countdown;
    }

    template <typename T = SmoothingType>
    MultiplicativeVoid<T> setStepSize()
    {
        step = std::exp ((std::log (std::abs (this->target)) - std::log (std::abs (this->currentValue))) / (FloatType) this->countdown);
    }

    //==============================================================================
    template <typename T = SmoothingType>
    LinearVoid<T> setNextValue() noexcept
    {
        this->currentValue += step;
    }

    template <typename T = SmoothingType>
    MultiplicativeVoid<T> setNextValue() noexcept
    {
        this->currentValue *= step;
    }

    //==============================================================================
    template <typename T = SmoothingType>
    LinearVoid<T> skipCurrentValue (int numSamples) noexcept
    {
        this->currentValue += step * (FloatType) numSamples;
    }

    template <typename T = SmoothingType>
    MultiplicativeVoid<T> skipCurrentValue (int numSamples)
    {
        this->currentValue *= (FloatType) std::pow (step, numSamples);
    }

    //==============================================================================
    FloatType step = FloatType();
    int stepsToTarget = 0;
};

} // namespace utils

