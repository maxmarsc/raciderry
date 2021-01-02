/*
  ==============================================================================

    MidiBrokerTestUnit.cpp
    Created: 20 Dec 2020 4:43:22pm
    Author:  maxime

  ==============================================================================
*/

#include "Tests/CustomTestUnit.h"
#include "Tests/Utils.h"

#include "Control/MidiBroker.h"

#include "Utils/Identifiers.h"

namespace tests
{

class MidiBrokerTestUnit : public CustomTestUnit
{
friend class control::MidiBroker;

public:
    MidiBrokerTestUnit() : CustomTestUnit("Midi Broker testing", category::control) {};

    void resetBroker()
    {
        auto* broker = control::MidiBroker::getInstanceWithoutCreating();

        if (broker != nullptr)
        {
            broker->clearSingletonInstance();
        }
    }

    void initialise() override
    {
        resetBroker();
    }

    void shutdown() override
    {
        resetBroker();
    }

    void singleTestInit() override
    {
        resetBroker();
    }

    void singleTestShutdown() override
    {
        resetBroker();
    }

    void runTest() override
    {
    TEST("Singleton constructor", [=] {
        auto* broker = control::MidiBroker::getInstance();

        expect(broker != nullptr);
    });

    TEST("Parameter Getter", [=] {
        auto* broker = control::MidiBroker::getInstance();
        jassert(broker != nullptr);
        
        // We only test basic parameters, some could change over time and we don't
        // wanna recode the entire test every time
        {
            auto param = broker->getParameter(identifiers::controls::ACCENT);
            expect(param.isValid());
        }
        {
            auto param = broker->getParameter(identifiers::controls::ACCENT_DECAY);
            expect(param.isValid());
        }
        {
            auto param = broker->getParameter(identifiers::controls::ATTACK);
            expect(param.isValid());
        }
        {
            auto param = broker->getParameter(identifiers::controls::DECAY);
            expect(param.isValid());
        }
        {
            auto param = broker->getParameter(identifiers::controls::RELEASE);
            expect(param.isValid());
        }
        {
            auto param = broker->getParameter(identifiers::controls::SUSTAIN);
            expect(param.isValid());
        }
        {
            auto param = broker->getParameter(identifiers::controls::CUTOFF);
            expect(param.isValid());
        }
        {
            auto param = broker->getParameter(identifiers::controls::RESONANCE);
            expect(param.isValid());
        }
        {
            auto param = broker->getParameter(identifiers::controls::WAVEFORM_RATIO);
            expect(param.isValid());
        }
        {
            // Test it twice on purpose
            auto param = broker->getParameter(identifiers::controls::ACCENT);
            expect(param.isValid());
        }

    });
    
    }

};

static MidiBrokerTestUnit MIDI_BROKER_TEST;

} // namespace tests

