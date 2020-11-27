/*
  ==============================================================================

    MidiBroker.cpp
    Created: 24 Aug 2020 5:03:40pm
    Author:  maxime

  ==============================================================================
*/

#include "MidiBroker.h"

#include "Utils/Parameters.h"
#include "Utils/Identifiers.h"

namespace control
{

const auto presetPrefix = juce::String("preset_");

MidiBroker::MidiBroker()
    : m_readyToSavePreset(false)
{
    initParameters();
    initPresets();
}

juce::MidiBuffer MidiBroker::getNoteMidiBuffer() noexcept
{
    // We swap the buffers to make sure we don't block the other thread
    // on the spinlock for too long
    // It might be interesting to go for a lock-free FIFO implementation to avoid
    // introducing some latency to the notes sometimes
    auto newBuffer = juce::MidiBuffer();

    {
        auto tryLock = juce::SpinLock::ScopedTryLockType(m_spinLock);
        if (tryLock.isLocked())
        {
            newBuffer.swapWith(m_midiBuffer);
        }
    }

    return newBuffer;
}

ControllableParameter MidiBroker::getParameter(const juce::Identifier& id) const
{
    if (m_idToParameterMap.count(id) == 1)
    {
        return ControllableParameter(m_idToParameterMap.at(id));
    }

    return ControllableParameter();
}

//==============================================================================
void MidiBroker::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& msg)
{
    // DBG(msg.getDescription());
    if (msg.getChannel() != parameters::midiCC::GLOBAL_CHANNEL) { return; }
    if (msg.isNoteOnOrOff())
    {
        handleNoteMessage(msg);
    }
    else if (msg.isProgramChange())
    {
        if (m_readyToSavePreset)
        {
            saveToPreset(msg.getProgramChangeNumber());
        }
        else
        {
            loadPreset(msg.getProgramChangeNumber());
        }
        
    }
    else if (msg.isController())
    {
        handleControllerMessage(msg);
    }
}

//==============================================================================
void MidiBroker::initParameters()
{
    // We create and assign parameters to midi control signals
    
    // Attack
    auto attack = ControllableParameter(parameters::values::ATTACK_DEFAULT,
            parameters::values::ATTACK_MIN,
            parameters::values::ATTACK_MAX,
            ControllableParameter::ScaleType::logarithmic);
    m_midiCCToParameterMap[parameters::midiCC::ATTACK] = attack;
    m_idToParameterMap[identifiers::controls::ATTACK] = attack;

    // Decay
    auto decay = ControllableParameter(parameters::values::DECAY_DEFAULT,
            parameters::values::DECAY_MIN,
            parameters::values::DECAY_MAX,
            ControllableParameter::ScaleType::logarithmic);
    m_midiCCToParameterMap[parameters::midiCC::DECAY] = decay;
    m_idToParameterMap[identifiers::controls::DECAY] = decay;

    // Sustain
    auto sustain = ControllableParameter(parameters::values::SUSTAIN_DEFAULT,
            parameters::values::SUSTAIN_MIN,
            parameters::values::SUSTAIN_MAX);
    m_midiCCToParameterMap[parameters::midiCC::SUSTAIN] = sustain;
    m_idToParameterMap[identifiers::controls::SUSTAIN] = sustain;

    // Release
    auto release = ControllableParameter(parameters::values::RELEASE_DEFAULT,
            parameters::values::RELEASE_MIN,
            parameters::values::RELEASE_MAX,
            ControllableParameter::ScaleType::logarithmic);
    m_midiCCToParameterMap[parameters::midiCC::RELEASE] = release;
    m_idToParameterMap[identifiers::controls::RELEASE] = release;

    // Waveform ratio
    auto waveformRatio = ControllableParameter(
            parameters::values::WAFEFORM_RATIO_DEFAULT,
            parameters::values::WAFEFORM_RATIO_MIN,
            parameters::values::WAFEFORM_RATIO_MAX);
    m_midiCCToParameterMap[parameters::midiCC::WAVEFORM_RATIO] = waveformRatio;
    m_idToParameterMap[identifiers::controls::WAVEFORM_RATIO] = waveformRatio;

    // Glide
    auto glide = ControllableParameter(parameters::values::GLIDE_DEFAULT,
            parameters::values::GLIDE_MIN,
            parameters::values::GLIDE_MAX,
            ControllableParameter::ScaleType::linear,
            256);
    m_midiCCToParameterMap[parameters::midiCC::GLIDE] = glide;
    m_idToParameterMap[identifiers::controls::GLIDE] = glide;

    // Filter Cutoff Frequency
    auto cutoff = ControllableParameter(parameters::values::CUTOFF_DEFAULT,
            parameters::values::CUTOFF_MIN,
            parameters::values::CUTOFF_MAX,
            ControllableParameter::ScaleType::logarithmic,
            512);
    m_midiCCToParameterMap[parameters::midiCC::CUTOFF] = cutoff;
    m_idToParameterMap[identifiers::controls::CUTOFF] = cutoff;

    // Filter Resonance
    auto resonance = ControllableParameter(parameters::values::RESONANCE_DEFAULT,
            parameters::values::RESONANCE_MIN,
            parameters::values::RESONANCE_MAX);
    m_midiCCToParameterMap[parameters::midiCC::RESONANCE] = resonance;
    m_idToParameterMap[identifiers::controls::RESONANCE] = resonance;

    // Filter Mix
    auto filterMix = ControllableParameter(parameters::values::MIX_DEFAULT,
            parameters::values::MIX_MIN,
            parameters::values::MIX_MAX);
    m_midiCCToParameterMap[parameters::midiCC::FILTER_MIX] = filterMix;
    m_idToParameterMap[identifiers::controls::FILTER_MIX] = filterMix;

    // Envelope filter modulation
    auto envMod = ControllableParameter(parameters::values::ENV_MOD_DEFAULT,
            parameters::values::ENV_MOD_MIN,
            parameters::values::ENV_MOD_MAX);
    m_midiCCToParameterMap[parameters::midiCC::ENV_MOD] = envMod;
    m_idToParameterMap[identifiers::controls::ENV_MOD] = envMod;

    // Accent amount
    auto accent = ControllableParameter(parameters::values::ACCENT_DEFAULT,
            parameters::values::ACCENT_MIN,
            parameters::values::ACCENT_MAX);
    m_midiCCToParameterMap[parameters::midiCC::ACCENT] = accent;
    m_idToParameterMap[identifiers::controls::ACCENT] = accent;

    // Accent decay
    auto accentDec = ControllableParameter(parameters::values::ACC_DEC_DEFAULT,
            parameters::values::ACC_DEC_MIN,
            parameters::values::ACC_DEC_MAX,
            ControllableParameter::ScaleType::logarithmic);
    m_midiCCToParameterMap[parameters::midiCC::ACCENT_DECAY] = accentDec;
    m_idToParameterMap[identifiers::controls::ACCENT_DECAY] = accentDec;
}

void MidiBroker::initPresets()
{
    auto presetFile = juce::File::getCurrentWorkingDirectory().getChildFile(parameters::files::PRESETS_FILE);
    
    if (presetFile.existsAsFile())
    {
        m_presets = juce::XmlDocument::parse(presetFile);
    }

    if (! presetFile.existsAsFile() || m_presets == nullptr)
    {
        // If we cannot load a valid preset xml, we create a new one
        DBG("No valid XML preset file found");
        m_presets = std::make_unique<juce::XmlElement>(identifiers::miscellaneous::PRESETS);
    }  
}

void MidiBroker::serializePresets()
{
    jassert(m_presets != nullptr);
    DBG(m_presets->toString());
    auto presetFile = juce::File::getCurrentWorkingDirectory().getChildFile(parameters::files::PRESETS_FILE);
    auto success = m_presets->writeTo(presetFile);

    if (! success)
    {
        DBG("Could not save presets to preset file");
    }
}

void MidiBroker::loadPreset(int presetId)
{
    jassert(m_presets != nullptr);
    
    auto* preset = m_presets->getChildByName(presetPrefix + juce::String(presetId));

    if (preset != nullptr)
    {
        for (auto it = m_idToParameterMap.begin(); it != m_idToParameterMap.end(); ++it)
        {
            const auto& parameterId = it->first;
            auto& parameter = it->second;

            if (preset->hasAttribute(parameterId))
            {
                auto presetValue = preset->getIntAttribute(parameterId);
                parameter.setDiscretValue(presetValue);
            }
        }

        DBG(juce::String("Preset loaded : ") + juce::String(presetId));
    }
    else
    {
        DBG(juce::String("No such preset ") + juce::String(presetId));
    }
    
}

void MidiBroker::saveToPreset(int presetId)
{
    jassert(m_presets != nullptr);
    auto* newPresetXml = m_presets->createNewChildElement(presetPrefix + juce::String(presetId));

    for (auto it = m_idToParameterMap.begin(); it != m_idToParameterMap.end(); ++it)
    {
        const auto& parameterId = it->first;
        const auto& parameter = it->second;

        newPresetXml->setAttribute(parameterId, parameter.getCurrentDiscretValue());
    }

    serializePresets();
}

void MidiBroker::handleNoteMessage(const juce::MidiMessage& msg)
{
    auto tryLock = juce::SpinLock::ScopedTryLockType(m_spinLock);

    if (tryLock.isLocked())
    {
        m_midiBuffer.addEvent(msg, 0);
        
        for (auto msgIt = m_fallbackMidiBuffer.begin(); msgIt != m_fallbackMidiBuffer.end(); msgIt++)
        {
            m_midiBuffer.addEvent((*msgIt).getMessage(), 0);
        }
        
        m_fallbackMidiBuffer.clear();
    }
    else
    {
        m_fallbackMidiBuffer.addEvent(msg, 0);
    }
}

void MidiBroker::handleControllerMessage(const juce::MidiMessage& msg)
{
    auto controllerNumber = msg.getControllerNumber();

    if (msg.isControllerOfType(parameters::midiCC::SAVE_PATCH))
    {
        m_readyToSavePreset = ! m_readyToSavePreset;
        DBG("Switching");
        return;
    }

    if (m_midiCCToParameterMap.count(controllerNumber) == 1)
    {
        // If this controller is assigned

        // We use relative control values :
        // 64 : no movement
        // 58 -> 63 : negative movement, 58 the faster
        // 65 -> 70 : positive movement, 65 the faster
        auto controlDelta = msg.getControllerValue() - 64;

        if (controlDelta < -10 || controlDelta > 10)
        {
            DBG("Invalid midi control input detected");
            return;
        }

        m_midiCCToParameterMap[controllerNumber].updateCurrentDiscretValue(controlDelta);

        // If you want to implement Midi return to your controller, this should
        // probably be the place
    }
}

JUCE_IMPLEMENT_SINGLETON(MidiBroker)

}//namespace control