#include "Parameters.h"

#include "Utils/Identifiers.h"

namespace parameters
{

constexpr auto CC = "CC";
constexpr auto DEFAULT = "DEFAULT";
constexpr auto MIN = "MIN";
constexpr auto MAX = "MAX";
constexpr auto GLOBAL_CHANNEL = "GLOBAL_CHANNEL";
constexpr auto SAVE_PATCH_CC = "SAVE_PATCH_CC";

const std::map<juce::Identifier, Parameter>   Parameter::loadParameters(
        int& globalChannel, int& savePatchCC)
{
    auto parametersMap = std::map<juce::Identifier, Parameter>();
    auto userParameterFile = juce::File(files::PARAMETERS);
    auto defaultParameterData = juce::var();
    auto userParameterData = juce::var();

    // Try to load the user's parameters file
    if (userParameterFile.existsAsFile())
    {
        userParameterData = juce::JSON::parse(userParameterFile);
    }

    // Load the default parameters configuration
    if (defaultParameterData == juce::var())
    {
        auto block = juce::MemoryBlock(BinaryData::default_parameters_json, 
                BinaryData::default_parameters_jsonSize);
        defaultParameterData = juce::JSON::parse(block.toString());
        jassert(defaultParameterData != juce::var());
    }
    jassert(defaultParameterData.hasProperty("GLOBAL_CHANNEL"));
    jassert(defaultParameterData.hasProperty("ATTACK"));

    // Which controls do we want to read from the JSON
    const juce::Identifier* controlsToLoad[] = {
        &identifiers::controls::ATTACK,
        &identifiers::controls::DECAY,
        &identifiers::controls::SUSTAIN,
        &identifiers::controls::RELEASE,
        &identifiers::controls::WAVEFORM_RATIO,
        &identifiers::controls::GLIDE,
        &identifiers::controls::CUTOFF,
        &identifiers::controls::RESONANCE,
        &identifiers::controls::ENV_MOD,
        &identifiers::controls::FILTER_MIX,
        &identifiers::controls::ACCENT,
        &identifiers::controls::ACCENT_DECAY,
    };

    // Populate the map with the parameters
    for (auto paramId : controlsToLoad)
    {
        jassert(defaultParameterData.hasProperty(*paramId));
        auto var = juce::var();
        if (userParameterData.hasProperty(*paramId))
        {
            // Override with the user's value
            var = userParameterData[*paramId];
        }
        else
        {
            // Use the default value
            var = defaultParameterData[*paramId];
        }
        parametersMap[*paramId] = Parameter(var);
    }

    jassert(defaultParameterData.hasProperty(GLOBAL_CHANNEL));
    jassert(defaultParameterData.hasProperty(SAVE_PATCH_CC));
    globalChannel = defaultParameterData.getProperty(GLOBAL_CHANNEL, juce::var());
    savePatchCC = defaultParameterData.getProperty(SAVE_PATCH_CC, juce::var());

    return parametersMap;
}

Parameter::Parameter(const juce::var& data)
{
    jassert(data.hasProperty(CC));
    jassert(data.hasProperty(DEFAULT));
    jassert(data.hasProperty(MIN));
    jassert(data.hasProperty(MAX));

    m_cc = int(data.getProperty(CC, juce::var()));
    m_default = float(data.getProperty(DEFAULT, juce::var()));
    m_min = float(data.getProperty(MIN, juce::var()));
    m_max = float(data.getProperty(MAX, juce::var()));
}

} // namespace parameters
