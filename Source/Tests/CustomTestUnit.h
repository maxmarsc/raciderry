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

class CustomTestUnit : juce::UnitTest
{
public:
  virtual void singleTestInit() {};
  virtual void singleTestShutdown() {};
  void singleTest(juce::String name, std::function<void(void)> function)
  {
    singleTestInit();
    beginTest(name);

    function();

    singleTestShutdown();
  }


private:

};

} //namespace Tests