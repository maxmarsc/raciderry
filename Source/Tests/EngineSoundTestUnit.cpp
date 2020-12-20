/*
  ==============================================================================

    EngineSoundTestUnit.cpp
    Created: 20 Dec 2020 12:54:33pm
    Author:  maxime

  ==============================================================================
*/

#include "Tests/CustomTestUnit.h"
#include "Engine/Sound.h"

namespace Tests
{

class SoundTestUnit : public CustomTestUnit
{
public:
    SoundTestUnit() : CustomTestUnit("Sound testing", "Sound Engine") {};

    void runTest() override
    {
    TEST("Constructor", [=] {
        auto sound = engine::Sound();
    });

    TEST("Basic methods", [=]{
        auto sound = engine::Sound();

        // Test channels
        for (auto i=1; i<=12; ++i)
        {
            sound.appliesToChannel(i);
        }

        // Test note
        for (auto i=0; i<128; ++i)
        {
            sound.appliesToNote(i);
        }
    });

    }


};

static SoundTestUnit SOUND_TEST;

} // namespace Tests
