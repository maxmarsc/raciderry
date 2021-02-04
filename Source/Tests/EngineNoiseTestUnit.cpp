/*
  ==============================================================================

    EngineSoundTestUnit.cpp
    Created: 20 Dec 2020 12:54:33pm
    Author:  maxime

  ==============================================================================
*/

#include "Tests/CustomTestUnit.h"
#include "Tests/Utils.h"

#include "Engine/NoiseGenerator.h"

namespace tests
{

class NoiseTestUnit : public CustomTestUnit
{
public:
    NoiseTestUnit() : CustomTestUnit("Noise Generator testing", category::engine::bindings) {};

    void initialise() override
    {
        rng = juce::UnitTest::getRandom();
    }

    void runTest() override
    {
    TEST("Constructor", [=] {
        float ranges[] = {0.f, 0.01f, -0.01f, 0.5f, 1.f};

        for (auto& range : ranges)
        {
            auto noiseGen = engine::NoiseGenerator(range);
        }
    });

    TEST("Basic methods", [=] {
        // Generate differents range to tests
        auto numRanges = 10;
        auto ranges = std::vector<float>(numRanges);
        auto testCount = 0;

        for (auto i = 0; i<numRanges; ++i)
        {
            ranges[i] = rng.nextFloat();
        }

        // For each range, check values are correctly bounded
        for (auto& range : ranges)
        {
            auto numCheck = 10;
            auto noiseGen = engine::NoiseGenerator(range);

            while(numCheck--)
            {
                auto factor = noiseGen.getNoiseFactor();

                expectGreaterThan(factor, 0.f, "negative noise factor");
                expectGreaterThan(factor, 1.f - range, "noise factor out of bound");
                expectLessThan(factor, 2.f, "noise factor greater than 2");
                expectLessThan(factor, 1.f + range, "noise factor out of range");
                testCount++;
            }
        }

        // Because we use loops we wanna make sure we tested enough times
        expectGreaterThan(testCount, 10);
    });
    }

private:
    juce::Random    rng;
};

static NoiseTestUnit NOISE_TEST;

} // namespace tests
