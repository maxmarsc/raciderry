/*
  ==============================================================================

    EngineSignalBusTestUnit.cpp
    Created: 31 Jan 2021 6:40:18pm
    Author:  maxime

  ==============================================================================
*/

#include "Tests/CustomTestUnit.h"
#include "Tests/Utils.h"
#include "Engine/SignalBus.h"

namespace tests
{

class SignalBusTestUnit : public CustomTestUnit
{
public:
    SignalBusTestUnit() : CustomTestUnit("Signal Bus testing", category::engine::bindings) {};

    void initialise() override
    {
        rng = juce::UnitTest::getRandom();
    }

    void runTest() override
    {
    TEST("Constructor", [=]{
        auto signalBus = engine::SignalBus();
    });

    TEST("Signal Read - Valid Ids", [=]{
        auto signalBus = engine::SignalBus();

        // Check if reading expected values raise an assert/error
        for (auto id = 0; id < engine::SignalBus::MAX; ++id)
        {
            auto value = signalBus.readSignal(engine::SignalBus::SignalId(id));
        }
    });

    TEST("Signal Write - Valid Ids", [=]{
        auto signalBus = engine::SignalBus();

        // Check if writing expected values raises an assert/error
        for (auto id = 0; id < engine::SignalBus::MAX; ++id)
        {
            signalBus.updateSignal(engine::SignalBus::SignalId(id), rng.nextFloat());
        }
    });

    TEST("Signal R/W - Invalid Id", [=] {
        auto signalBus = engine::SignalBus();

        // Check if writing/reading unexpected value raises an assert/error (ie: shouldn't)
        signalBus.updateSignal(engine::SignalBus::MAX, rng.nextFloat());
        expect(signalBus.readSignal(engine::SignalBus::MAX) == 0.);
        signalBus.updateSignal(engine::SignalBus::SignalId(-1), rng.nextFloat());
        expect(signalBus.readSignal(engine::SignalBus::SignalId(-1)) == 0.);
    });

    TEST("Signal read over write", [=]{
        auto signalBus = engine::SignalBus();
        int testCount = 0;

        for (auto id = 0; id < engine::SignalBus::MAX; ++id)
        {
            for (auto y = 0; y < 10; ++y)
            {
                auto newValue = rng.nextFloat();
                auto iterId = engine::SignalBus::SignalId(id);

                signalBus.updateSignal(iterId, newValue);
                expect(signalBus.readSignal(iterId) == newValue);
                testCount++;
            }
        }

        expect(testCount > 0);
    });

    }

private:
    juce::Random    rng;

};

static SignalBusTestUnit SIGNAL_BUS_TEST;

} // namespace tests

