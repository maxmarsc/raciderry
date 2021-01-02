/*
  ==============================================================================

    EngineVoiceTestUnit.cpp
    Created: 20 Dec 2020 1:23:51pm
    Author:  maxime

  ==============================================================================
*/

#include "Tests/CustomTestUnit.h"
#include "Tests/Utils.h"

#include "Engine/NoiseGenerator.h"
#include "Engine/Voice.h"

namespace tests
{

class VoiceTestUnit : public CustomTestUnit
{
public:
    VoiceTestUnit() : CustomTestUnit("Voice testing", category::engine::synth) {} ;

    void initialise() override
    {
        noiseGen = std::make_unique<engine::NoiseGenerator>(getRandom().nextFloat());
    }

    void runTest() override
    {
    TEST("Constructor", [=] {
        auto voice = engine::Voice(*noiseGen);
    });

    TEST("Getters", [=] {
        auto voice = engine::Voice(*noiseGen);

        // Test valid weak_ptr
        {
            auto oscPtr = voice.getOscPtr();

            expect(oscPtr.expired() == false);
            expect(oscPtr.use_count() == 1);
        }
        
        // Test invalid weak_ptr
        {
            auto scndVoice = std::make_unique<engine::Voice>(*noiseGen);
            auto oscPtr = scndVoice->getOscPtr();
            scndVoice.reset();

            expect(oscPtr.expired() == true);
        }
    });

    TEST("Basic methods", [=] {
        auto voice = engine::Voice(*noiseGen);

        ///TODO: 
    });

    }

private:
    std::unique_ptr<engine::NoiseGenerator>  noiseGen;

};

} // namespace Tests

