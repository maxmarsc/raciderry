/*
  ==============================================================================

    CustomTestUnit.h
    Created: 20 Dec 2020 3:09:20pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace Tests {

class CustomTestUnit : public juce::UnitTest
{
public:
    CustomTestUnit(const juce::String& name, const juce::String& category)
        : UnitTest(name, category)
    {
      
    }

    virtual void singleTestInit() {};
    virtual void singleTestShutdown() {};
    void TEST(juce::String name, std::function<void(void)> function)
    {
        singleTestInit();
        beginTest(name);

        function();

        singleTestShutdown();
    }


private:

};

} //namespace Tests