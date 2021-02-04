/*
  ==============================================================================

    TestRunner.h
    Created: 4 Feb 2021 2:14:01pm
    Author:  maxime

  ==============================================================================
*/

#include <JuceHeader.h>

#pragma once

namespace tests
{

class TestRunner : public juce::Thread {
public:
    enum State {
        CREATED,
        RUNNING,
        FINISHED,
        FAILED
    };

    TestRunner();

    virtual void run() override;

    State getState() { return m_state; };
private:
    int  runTests();
    int  waitForMessageManager();
    void  stopMessageManager();


    juce::ThreadPool        m_asyncCallsThreadPool;
    State                   m_state;
};

} // namespace tests
