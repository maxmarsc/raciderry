/*
  ==============================================================================

    CallDispatcher.h
    Created: 4 Feb 2021 5:25:16pm
    Author:  maxime

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

#include <map>

namespace tests
{

class CallDispatcher {
public:
    using CallUID = int;

    CallDispatcher(int numberOfThread=1);
    ~CallDispatcher();

    CallUID registerRecurrentCall(const std::function< void()>&, 
                                  int timeRangeBetweenCalls);

    bool    unregisterRecurrentCall(CallUID uid);

private:
    class RecurrentCall : public juce::ThreadPoolJob {
    public: 
        RecurrentCall(const std::function< void()>& call, 
                      int timeMsRangeBetweenCalls,
                      juce::Random& rng);

        virtual juce::ThreadPoolJob::JobStatus runJob() override;
    private:
        std::function< void()>          m_call;
        int                             m_sleepTimeMs;
        juce::Random&                   r_rng;
    };

    int                                 m_numberOfThread;
    CallUID                             m_nextUID;
    juce::ThreadPool                    m_threadPool;
    juce::Random                        m_rng;
    std::map<CallUID, std::unique_ptr<RecurrentCall>>    m_callMap;
    
};

} // namespace tests
