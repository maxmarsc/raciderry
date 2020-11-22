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

/**
 * @class control::ControllableParameter
 * @brief An atomic thread safe structure which holds the logic and value of a 
 * parameter controlled by the user.
 * 
 * A ControllableParameter precomputes all the possibles values for the 
 * parameter and keeps track of the current value.
 * 
 * Any module can hold how many he needs, and can become an ChangeListener to
 * react to a value change by calling addListener()
 * 
 * This class uses pass by value semantic, each parameter should be creating 
 * only once and then passed and copied. Most methods are thread-safes.
 * 
 * A parameter has a default discret resolution of 128 values. These can be
 * distributed either linearly, exponantially or logarithmicly among the value
 * range provided in the constructor.
 * 
 * The default contructor should not be used explicitely and is only available
 * for conveniency.
 */
class ControllableParameter
{
public:
//==============================================================================
    /**
     * @brief Describes the distribution of the parameter values among the 
     * provided range
     */
    enum class ScaleType {linear, exponential, logarithmic};

//==============================================================================
    /**
     * @brief Construct an empty and invalid parameter object
     * 
     * @note Such an object only supports comparison and isValid() method calls
     * and must not be used
     */
    ControllableParameter();

    /**
     * @brief Construct a valid Controllable Parameter object
     * 
     * @param initValue The value to initialise the parameter to. Must match the
     * provided range. It will be casted to the nearest value in the precomputed
     * discret distribution
     * @param minValue  Minimum of the value range
     * @param maxValue  Maximum of the value range 
     * @param scale     Type of distribution scale, default to linear
     * @param discretRange The number of samples of the discret distribution
     */
    ControllableParameter(float initValue, float minValue, float maxValue, 
            ScaleType scale = ScaleType::linear, int discretRange = 128,
            float minPlusOneValue=0.0);

//==============================================================================
    /**
     * @brief Usefull to identify which parameters has changed
     */
    bool operator==(juce::ChangeBroadcaster* source) const;
    /**
     * @brief Test if the object is a valid or an empty one
     */
    bool isValid() const noexcept;
    /**
     * @brief Get the current value of the parameter
     */
    float getCurrentValue() const noexcept;
    /**
     * @brief Get the current discret value of the parameter
     */
    int getCurrentDiscretValue() const noexcept;
    /**
     * @brief Get the ratio corresponding to the current float value of the 
     * parameter
     * 
     * @return float Btw 0 and 1. Position in the precomputed scaled (linear/
     * logaritmic/exponential) range. 0 the min, 1 the max.
     */
    float getUnscaledRatioForCurrentValue() const noexcept;
    /**
     * @brief Get the number of possibles discret value in the range
     */
    int getDiscretRange() const noexcept;
    /**
     * @brief Get the precomputed value for the given ratio
     * 
     * @param ratio Btw 0 and 1. Position in the precomputed scaled (linear/
     * logaritmic/exponential) range. 0 the min, 1 the max.
     * @return float The corresponding value
     */
    float getScaledValueForUnscaledRatio(float ratio) const noexcept;

//==============================================================================
    /**
     * @brief Changes the current value
     * @note This function is NOT safe to call from the audio thread
     * 
     * @param delta The delta to apply to the discret position of the parameter
     */
    void updateCurrentDiscretValue(int delta);
    /**
     * @brief Set the current value
     * @note This function is NOT safe to call from the audio thread
     * 
     * @param newValue The new discret value, btw 0 and discret range
     */
    void setDiscretValue(int newValue);
    /**
     * @brief Add a listener to the parameter to be noticed when the value
     * changes
     * @note This method is NOT entirely thread-safe, avoid calling it on
     * message/audio thread without further investigation on 
     * juce::ChangeBroadcaster methods
     * 
     * @param listener The new listener to the parameter
     */
    void addListener(juce::ChangeListener* listener);


private:
//==============================================================================

    // Pimpl idiom with shared pointer to deal with multithreading
    class Impl;
    std::shared_ptr<Impl>           m_impl;
};

} // namespace control
