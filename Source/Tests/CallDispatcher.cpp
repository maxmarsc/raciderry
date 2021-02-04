/*
  ==============================================================================

    CallDispatcher.cpp
    Created: 4 Feb 2021 5:25:16pm
    Author:  maxime

  ==============================================================================
*/

#include "CallDispatcher.h"

namespace tests
{

CallDispatcher::CallDispatcher(int numberOfThread)
    : m_numberOfThread(numberOfThread),
      m_nextUID(0),
      m_threadPool(numberOfThread),
      m_rng(0)
{
    m_rng.setSeedRandomly();
}

CallDispatcher::CallUID CallDispatcher::registerRecurrentCall(
    const std::function< void()>& call, int timeRangeBetweenCalls)
{
    if (m_callMap.size() < m_numberOfThread)
    {
        auto uid = m_nextUID++;

        m_callMap.emplace(m_nextUID, 
            std::make_unique<CallDispatcher::RecurrentCall>(call, timeRangeBetweenCalls, m_rng));

        return uid;
    }

    return -1;
}

bool CallDispatcher::unregisterRecurrentCall(CallDispatcher::CallUID uid)
{
    if (m_callMap.count(uid) > 0)
    {
        auto& recurrentCall = m_callMap.at(uid);
        jassert(recurrentCall != nullptr);
        recurrentCall->signalJobShouldExit();

        while(recurrentCall->isRunning())
        {
            juce::Thread::sleep(25);
            DBG("Waiting for a recurrent call to end");
        }
        
        return (m_callMap.erase(uid) != 0);
    }

    return false;
}

CallDispatcher::RecurrentCall::RecurrentCall(
    const std::function< void()>& call, 
    int timeRangeBetweenCalls, 
    juce::Random& rng)
    : juce::ThreadPoolJob(""),
      m_call(call),
      m_sleepTimeMs(timeRangeBetweenCalls),
      r_rng(rng)
{
    // Nothing to do here
}

juce::ThreadPoolJob::JobStatus CallDispatcher::RecurrentCall::runJob()
{
    while (! shouldExit())
    {
        m_call();

        juce::Thread::sleep(r_rng.nextInt(m_sleepTimeMs));
    }

    return juce::ThreadPoolJob::jobHasFinished;
}


} // namespace tests
