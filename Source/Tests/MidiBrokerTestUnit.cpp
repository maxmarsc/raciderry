/*
  ==============================================================================

    MidiBrokerTestUnit.cpp
    Created: 20 Dec 2020 4:43:22pm
    Author:  maxime

  ==============================================================================
*/

#include "Tests/CustomTestUnit.h"
#include "Tests/Utils.h"
#include "Tests/CallDispatcher.h"

#include "Control/MidiBroker.h"

#include "Utils/Identifiers.h"
#include "Utils/Parameters.h"

namespace tests
{

class MidiBrokerTestUnit : public CustomTestUnit
{
friend class control::MidiBroker;

public:
    MidiBrokerTestUnit() : CustomTestUnit("Midi Broker testing", category::control) {};

    void initialise() override
    {
        m_rng = getRandom();
    }

    void shutdown() override
    {
    }

    void singleTestInit() override
    {
    }

    void singleTestShutdown() override
    {
    }

    void runTest() override
    {
    TEST("Constructor", [=] {
        bool error = false;

        try
        {
            control::MidiBroker();
        }
        catch (const std::exception& err)
        {
            error = true;
        }

        expect(error != true);
    });

    TEST("ParameterMap", [=] {
        auto broker = control::MidiBroker();
        auto parameterMap = broker.getIdToParameterMap().lock();
        
        expect(parameterMap != nullptr);
        if (parameterMap == nullptr) { return; }

        expect(parameterMap->size() != 0);

        for (auto& pair : *parameterMap)
        {
          auto parameter = std::get<1>(pair);
          expect(parameter.isValid());
        } 

    });

    TEST("Note MIDI buffer - Single Thread", [=] {
        auto broker = control::MidiBroker();

        auto buffer = broker.getNoteMidiBuffer();
        expect(buffer.isEmpty());

        // Generate random midi messages for the broker
        auto bufferRef = juce::MidiBuffer();
        for (auto i = 0; i < 200; ++i) {
            // Add random message
            auto msg = GenerateRandomMidiMessage(m_rng);

            if (msg.isNoteOnOrOff() && msg.getChannel() == parameters::midiCC::GLOBAL_CHANNEL) {
                bufferRef.addEvent(msg, i);
            }
            broker.handleIncomingMidiMessage(nullptr, msg);
        }

        // Get the buffer after the broker treated it
        buffer = broker.getNoteMidiBuffer();
        expect(! buffer.isEmpty());
        auto bufferIt = buffer.begin();
        auto refIt = bufferRef.begin();

        while (refIt != bufferRef.end()) {
            auto msg = (*bufferIt).getMessage();
            auto refMsg = (*refIt).getMessage();

            expect(msg.isNoteOnOrOff());
            expect(refMsg.isNoteOnOrOff());
            expect(msg.getDescription() == refMsg.getDescription());
            refIt++;
            bufferIt++;
        }

    });

    TEST("Note MIDI buffer - Multi Thread stress test", [=] {
        auto callDispatcher = CallDispatcher();
        auto broker = control::MidiBroker();
        auto bufferRef = juce::MidiBuffer();
        constexpr auto postIntervalMs = 10;

        auto postMsgCall = [&] {
            auto msg = GenerateRandomMidiMessage(m_rng);
            broker.handleIncomingMidiMessage(nullptr, msg);
            msg = GenerateRandomMidiMessage(m_rng);
            broker.handleIncomingMidiMessage(nullptr, msg);
            msg = GenerateRandomMidiMessage(m_rng);
            broker.handleIncomingMidiMessage(nullptr, msg);
        };

        auto buffer = broker.getNoteMidiBuffer();
        expect(buffer.isEmpty());

        auto call_uid = callDispatcher.registerRecurrentCall(postMsgCall, postIntervalMs);

        auto count = 0;
        for (auto i=0; i<10; ++i) {
            juce::Thread::sleep(postIntervalMs*20);
            buffer = broker.getNoteMidiBuffer();

            for (auto msgIt : buffer) {
                ++count;
                auto msg = msgIt.getMessage();
                expect(msg.isNoteOnOrOff(), 
                    juce::String("Unexpected msg type : ") + msg.getDescription());
                expect(msg.getChannel() == parameters::midiCC::GLOBAL_CHANNEL,
                    juce::String("Unexpected channel : ") + msg.getDescription());
            }
        }

        callDispatcher.unregisterRecurrentCall(call_uid);

        expect(count > 20);

    });
    
    }

private:
    // CallDispatcher                                    m_callDispatcher;
    juce::Random                                      m_rng;
};

static MidiBrokerTestUnit MIDI_BROKER_TEST;

} // namespace tests

