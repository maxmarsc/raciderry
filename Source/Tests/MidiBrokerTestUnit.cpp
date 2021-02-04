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

    TEST("Parameter Getter", [=] {
        auto broker = control::MidiBroker();
        
        // We only test basic parameters, some could change over time and we don't
        // wanna recode the entire test every time
        {
            auto param = broker.getParameter(identifiers::controls::ACCENT);
            expect(param.isValid());
        }
        {
            auto param = broker.getParameter(identifiers::controls::ACCENT_DECAY);
            expect(param.isValid());
        }
        {
            auto param = broker.getParameter(identifiers::controls::ATTACK);
            expect(param.isValid());
        }
        {
            auto param = broker.getParameter(identifiers::controls::DECAY);
            expect(param.isValid());
        }
        {
            auto param = broker.getParameter(identifiers::controls::RELEASE);
            expect(param.isValid());
        }
        {
            auto param = broker.getParameter(identifiers::controls::SUSTAIN);
            expect(param.isValid());
        }
        {
            auto param = broker.getParameter(identifiers::controls::CUTOFF);
            expect(param.isValid());
        }
        {
            auto param = broker.getParameter(identifiers::controls::RESONANCE);
            expect(param.isValid());
        }
        {
            auto param = broker.getParameter(identifiers::controls::WAVEFORM_RATIO);
            expect(param.isValid());
        }
        {
            // Test it twice on purpose
            auto param = broker.getParameter(identifiers::controls::ACCENT);
            expect(param.isValid());
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

    TEST("Get Id To ParameterMap", [=] {

    });
    
    }

private:
    CallDispatcher                                    m_callDispatcher;
    juce::Random                                      m_rng;
};

static MidiBrokerTestUnit MIDI_BROKER_TEST;

} // namespace tests

