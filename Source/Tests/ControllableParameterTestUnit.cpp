/*
  ==============================================================================

    ControllableParameterTestUnit.cpp
    Created: 19 Apr 2021 3:22:59pm
    Author:  maxime

  ==============================================================================
*/

#include "Tests/CustomTestUnit.h"
#include "Tests/Utils.h"
#include "Tests/CallDispatcher.h"

#include "Control/ControllableParameter.h"

namespace tests
{

class ControllableParameterTestUnit : public CustomTestUnit
{
public:
    ControllableParameterTestUnit() : CustomTestUnit("ControllableParameter testing",
            category::control) {};

    void initialise() override
    {

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
        TEST("Empty Constructor", [=] {
            auto param = control::ControllableParameter();

            expect(! param.isValid());
        });

        TEST("Basic Linear parameter", [=] {
            auto constexpr vmin = 0.0f;
            auto constexpr vmax = 2.0f;
            auto constexpr vinit = 1.0f;
            auto param = control::ControllableParameter(
                vinit,
                vmin,
                vmax,
                control::ControllableParameter::ScaleType::linear
            );
            
            // Check init
            expect(param.isValid());
            expect(std::abs(param.getCurrentValue() - vinit) < 0.01f);
            expect(param.getDiscretRange() >= 128);
            auto initIdx = param.getCurrentDiscretValue();

            // Check movement around init value
            param.updateCurrentDiscretValue(-6);
            param.updateCurrentDiscretValue(-6);
            expect(param.getCurrentDiscretValue() < initIdx);
            expect(param.getCurrentValue() < vinit);
            param.updateCurrentDiscretValue(6);
            param.updateCurrentDiscretValue(6);
            param.updateCurrentDiscretValue(6);
            param.updateCurrentDiscretValue(6);
            expect(param.getCurrentDiscretValue() > initIdx);
            expect(param.getCurrentValue() > vinit);

            // Check extreme values
            auto idxMin = 0;
            auto idxMax = param.getDiscretRange() - 1;

            param.setDiscretValue(idxMin);
            expect(param.getCurrentDiscretValue() == idxMin);
            expect(param.getCurrentValue() == vmin);
            param.updateCurrentDiscretValue(-5);
            expect(param.getCurrentDiscretValue() == idxMin);
            expect(param.getCurrentValue() == vmin);

            param.setDiscretValue(idxMax);
            expect(param.getCurrentDiscretValue() == idxMax);
            expect(param.getCurrentValue() == vmax);
            param.updateCurrentDiscretValue(5);
            expect(param.getCurrentDiscretValue() == idxMax);
            expect(param.getCurrentValue() == vmax);
        });

        TEST("Basic Exp parameter", [=] {
            auto constexpr vmin = 0.01f;
            auto constexpr vmax = 2.0f;
            auto constexpr vinit = 1.0f;
            auto param = control::ControllableParameter(
                vinit,
                vmin,
                vmax,
                control::ControllableParameter::ScaleType::exponential
            );
            
            // Check init
            expect(param.isValid());
            expect(std::abs(param.getCurrentValue() - vinit) < 0.01f);
            expect(param.getDiscretRange() >= 128);
            auto initIdx = param.getCurrentDiscretValue();

            // Check movement around init value
            param.updateCurrentDiscretValue(-6);
            param.updateCurrentDiscretValue(-6);
            expect(param.getCurrentDiscretValue() < initIdx);
            expect(param.getCurrentValue() < vinit);
            param.updateCurrentDiscretValue(6);
            param.updateCurrentDiscretValue(6);
            param.updateCurrentDiscretValue(6);
            param.updateCurrentDiscretValue(6);
            expect(param.getCurrentDiscretValue() > initIdx);
            expect(param.getCurrentValue() > vinit);

            // Check extreme values
            auto idxMin = 0;
            auto idxMax = param.getDiscretRange() - 1;

            param.setDiscretValue(idxMin);
            expect(param.getCurrentDiscretValue() == idxMin);
            expect(param.getCurrentValue() == vmin);
            param.updateCurrentDiscretValue(-5);
            expect(param.getCurrentDiscretValue() == idxMin);
            expect(param.getCurrentValue() == vmin);

            param.setDiscretValue(idxMax);
            expect(param.getCurrentDiscretValue() == idxMax);
            expect(param.getCurrentValue() == vmax);
            param.updateCurrentDiscretValue(5);
            expect(param.getCurrentDiscretValue() == idxMax);
            expect(param.getCurrentValue() == vmax);
        });

        TEST("Basic Log parameter", [=] {
            auto constexpr vmin = 0.01f;
            auto constexpr vmax = 2.0f;
            auto constexpr vinit = 1.0f;
            auto param = control::ControllableParameter(
                vinit,
                vmin,
                vmax,
                control::ControllableParameter::ScaleType::logarithmic
            );
            
            // Check init
            expect(param.isValid());
            expect(std::abs(param.getCurrentValue() - vinit) < 0.02f);
            expect(param.getDiscretRange() >= 128);
            auto initIdx = param.getCurrentDiscretValue();

            // Check movement around init value
            param.updateCurrentDiscretValue(-6);
            param.updateCurrentDiscretValue(-6);
            expect(param.getCurrentDiscretValue() < initIdx);
            expect(param.getCurrentValue() < vinit);
            param.updateCurrentDiscretValue(6);
            param.updateCurrentDiscretValue(6);
            param.updateCurrentDiscretValue(6);
            param.updateCurrentDiscretValue(6);
            expect(param.getCurrentDiscretValue() > initIdx);
            expect(param.getCurrentValue() > vinit);

            // Check extreme values
            auto idxMin = 0;
            auto idxMax = param.getDiscretRange() - 1;

            param.setDiscretValue(idxMin);
            expect(param.getCurrentDiscretValue() == idxMin);
            expect(param.getCurrentValue() == vmin);
            param.updateCurrentDiscretValue(-5);
            expect(param.getCurrentDiscretValue() == idxMin);
            expect(param.getCurrentValue() == vmin);

            param.setDiscretValue(idxMax);
            expect(param.getCurrentDiscretValue() == idxMax);
            expect(param.getCurrentValue() == vmax);
            param.updateCurrentDiscretValue(5);
            expect(param.getCurrentDiscretValue() == idxMax);
            expect(param.getCurrentValue() == vmax);

            // Check non linearity
            constexpr auto a = 0.3f;
            constexpr auto b = 0.45f;
            constexpr auto c = 0.60f;
            auto ab_diff = param.getScaledValueForUnscaledRatio(b) -
                    param.getScaledValueForUnscaledRatio(a);
            auto bc_diff = param.getScaledValueForUnscaledRatio(c) -
                    param.getScaledValueForUnscaledRatio(b);

            expect(ab_diff * 1.01 < bc_diff);
        });

        TEST("Accessors and value computing", [=] {
            auto constexpr vmin = 0.0f;
            auto constexpr vmax = 2.0f;
            auto constexpr vinit = 1.0f;
            auto param = control::ControllableParameter(
                vinit,
                vmin,
                vmax
            );

            expect(param.isValid());

            auto crtIdx = param.getCurrentDiscretValue();
            expect(crtIdx >= 0 && crtIdx < param.getDiscretRange());

            auto unscaled_ratio = param.getUnscaledRatioForCurrentValue();
            expect(param.getCurrentValue() == param.getScaledValueForUnscaledRatio(unscaled_ratio));
        });

        TEST("Comparison and copy", [=] {
            auto constexpr vmin = 0.0f;
            auto constexpr vmax = 2.0f;
            auto constexpr vinit = 1.0f;
            auto param_1 = control::ControllableParameter(
                vinit,
                vmin,
                vmax
            );
            auto param_2 = control::ControllableParameter(
                vinit,
                vmin,
                vmax
            );

            auto param_1_copy = param_1;

            // Comparison
            expect(param_1.isValid());
            expect(param_2.isValid());
            expect(param_1_copy.isValid());
            expect(param_1.getCurrentValue() == param_2.getCurrentValue());
            
            // Change param 1 value
            param_1.updateCurrentDiscretValue(-5);
            expect(param_1.getCurrentDiscretValue() == param_1_copy.getCurrentDiscretValue());
            expect(param_2.getCurrentDiscretValue() != param_1_copy.getCurrentDiscretValue());
        });

        TEST("Multi thread access", [=] {
            auto constexpr minV = 0.0;
            auto constexpr maxV = 2.0;
            auto constexpr initV = 1.0;
            auto rng = juce::Random();
            auto param = control::ControllableParameter(initV, minV, maxV);
            auto callDispatcher = CallDispatcher();

            auto call = [&] {
                auto delta = (rng.nextInt() % 10) - 5;
                param.updateCurrentDiscretValue(delta);
            };

            auto callId = callDispatcher.registerRecurrentCall(call, 1);
            auto idxInit = param.getCurrentDiscretValue();
            auto countDiffFromInit = 0;

            for (auto i=0; i<30; ++i) {
                auto newIdx = param.getCurrentDiscretValue();
                auto newVal = param.getCurrentValue();
                if (newIdx != idxInit) {
                    ++countDiffFromInit;
                }
                
                expect(newVal <= maxV);
                expect(newVal >= minV);
                juce::Thread::sleep(5);
            }

            expect(countDiffFromInit > 0);


        });
    }

    juce::Random                                      m_rng;
};

static ControllableParameterTestUnit CONTROLLABLE_PARAMETER_TEST;

} // namespace tests

