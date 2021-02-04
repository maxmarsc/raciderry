/*
  ==============================================================================

    TestRunner.cpp
    Created: 4 Feb 2021 2:14:01pm
    Author:  maxime

  ==============================================================================
*/

#include "TestRunner.h"

#include "Tests/Utils.h"


constexpr auto MSG_MGR_WAIT_ATTEMPTS = 5;


namespace tests
{

TestRunner::TestRunner() : juce::Thread("Main test thread"),
    m_state(TestRunner::CREATED)
{
    // Nothing to do here
}

void TestRunner::run()
{
    if ( waitForMessageManager() != 0)
    {
        DBG("Failed to start tests : MessageManager is not running");
        m_state = FAILED;
        return;
    }

    m_state = RUNNING;
    auto result = runTests();

    if (result == 0)
    {
        m_state = FINISHED;
    }
    else
    {
        m_state = FAILED;
    }

    // Tests are finished, tells the message manager to stop
    stopMessageManager();
}

int TestRunner::runTests()
{
    auto tester = juce::UnitTestRunner();
    tester.setAssertOnFailure(false);

    auto failed = 0;
    auto categories_to_test = juce::StringArray(
        category::control,
        category::engine::bindings,
        category::engine::oscillators,
        category::engine::envelopes,
        category::engine::filters,
        category::engine::synth
    );

    for (auto& category : categories_to_test)
    {
        tester.runTestsInCategory(category);

        auto num_test_unit = tester.getNumResults();
        for (int i = 0; i < num_test_unit; ++i) 
        {
            auto* result = tester.getResult(i);
            if (result->failures > 0) {
                ++failed;
            }
        }
    }

    return failed;
}

int TestRunner::waitForMessageManager()
{
    // Wait for the message manager to be started
    auto msg_mgr_wait_attempt = MSG_MGR_WAIT_ATTEMPTS;
    while (juce::MessageManager::getInstanceWithoutCreating() == nullptr 
        && msg_mgr_wait_attempt >0) {
      DBG("Test Thread waiting for the message manager");
      Thread::sleep(100);
      --msg_mgr_wait_attempt;
    }

    if (juce::MessageManager::getInstanceWithoutCreating() == nullptr)
    {
        return 1;
    }
    
    return 0;
}

void TestRunner::stopMessageManager()
{
    auto msgMgr = juce::MessageManager::getInstanceWithoutCreating();

    if (msgMgr != nullptr) {
        msgMgr->stopDispatchLoop();
    }
}

} // namespace tests
