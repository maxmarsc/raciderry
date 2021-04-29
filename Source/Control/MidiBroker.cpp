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
    : m_globalChannel(-1),
      m_savePatchCC(-1),
      m_readyToSavePreset(false)
{
    initControllableParameters();
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

std::weak_ptr<ParameterMap> MidiBroker::getIdToParameterMap()
{
    return m_idToParameterMap;
}

//==============================================================================
void MidiBroker::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& msg)
{
    // DBG(msg.getDescription());
    if (msg.getChannel() != m_globalChannel) { return; }
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
void MidiBroker::initControllableParameters()
{
    // We create and assign parameters to midi control signals
    m_idToParameterMap = std::make_shared<ParameterMap>();
    auto settingsMap = parameters::Parameter::loadParameters(
            m_globalChannel, m_savePatchCC);
    
    // Attack
    auto attackSettings = settingsMap[identifiers::controls::ATTACK];
    auto attack = ControllableParameter(attackSettings.m_default,
            attackSettings.m_min,
            attackSettings.m_max,
            ControllableParameter::ScaleType::logarithmic);
    m_midiCCToParameterMap[attackSettings.m_cc] = attack;
    (*m_idToParameterMap)[identifiers::controls::ATTACK] = attack;

    // Decay
    auto decaySettings = settingsMap[identifiers::controls::DECAY];
    auto decay = ControllableParameter(decaySettings.m_default,
            decaySettings.m_min,
            decaySettings.m_max,
            ControllableParameter::ScaleType::logarithmic);
    m_midiCCToParameterMap[decaySettings.m_cc] = decay;
    (*m_idToParameterMap)[identifiers::controls::DECAY] = decay;

    // Sustain
    auto sustainSettings = settingsMap[identifiers::controls::SUSTAIN];
    auto sustain = ControllableParameter(sustainSettings.m_default,
            sustainSettings.m_min,
            sustainSettings.m_max);
    m_midiCCToParameterMap[sustainSettings.m_cc] = sustain;
    (*m_idToParameterMap)[identifiers::controls::SUSTAIN] = sustain;

    // Release
    auto releaseSettings = settingsMap[identifiers::controls::RELEASE];
    auto release = ControllableParameter(releaseSettings.m_default,
            releaseSettings.m_min,
            releaseSettings.m_max,
            ControllableParameter::ScaleType::logarithmic);
    m_midiCCToParameterMap[releaseSettings.m_cc] = release;
    (*m_idToParameterMap)[identifiers::controls::RELEASE] = release;

    // Waveform ratio
    auto waveformSettings = settingsMap[identifiers::controls::WAVEFORM_RATIO];
    auto waveformRatio = ControllableParameter(
            waveformSettings.m_default,
            waveformSettings.m_min,
            waveformSettings.m_max);
    m_midiCCToParameterMap[waveformSettings.m_cc] = waveformRatio;
    (*m_idToParameterMap)[identifiers::controls::WAVEFORM_RATIO] = waveformRatio;

    // Glide
    auto glideSettings = settingsMap[identifiers::controls::GLIDE];
    auto glide = ControllableParameter(glideSettings.m_default,
            glideSettings.m_min,
            glideSettings.m_max,
            ControllableParameter::ScaleType::linear,
            256);
    m_midiCCToParameterMap[glideSettings.m_cc] = glide;
    (*m_idToParameterMap)[identifiers::controls::GLIDE] = glide;

    // Filter Cutoff Frequency
    auto cutoffSettings = settingsMap[identifiers::controls::CUTOFF];
    auto cutoff = ControllableParameter(cutoffSettings.m_default,
            cutoffSettings.m_min,
            cutoffSettings.m_max,
            ControllableParameter::ScaleType::logarithmic,
            512);
    m_midiCCToParameterMap[cutoffSettings.m_cc] = cutoff;
    (*m_idToParameterMap)[identifiers::controls::CUTOFF] = cutoff;

    // Filter Resonance
    auto resonanceSettings = settingsMap[identifiers::controls::RESONANCE];
    auto resonance = ControllableParameter(resonanceSettings.m_default,
            resonanceSettings.m_min,
            resonanceSettings.m_max);
    m_midiCCToParameterMap[resonanceSettings.m_cc] = resonance;
    (*m_idToParameterMap)[identifiers::controls::RESONANCE] = resonance;

    // Filter Mix
    auto filterSettings = settingsMap[identifiers::controls::FILTER_MIX];
    auto filterMix = ControllableParameter(filterSettings.m_default,
            filterSettings.m_min,
            filterSettings.m_max);
    m_midiCCToParameterMap[filterSettings.m_cc] = filterMix;
    (*m_idToParameterMap)[identifiers::controls::FILTER_MIX] = filterMix;

    // Envelope filter modulation
    auto modSettings = settingsMap[identifiers::controls::ENV_MOD];
    auto envMod = ControllableParameter(modSettings.m_default,
            modSettings.m_min,
            modSettings.m_max);
    m_midiCCToParameterMap[modSettings.m_cc] = envMod;
    (*m_idToParameterMap)[identifiers::controls::ENV_MOD] = envMod;

    // Accent amount
    auto accentConfig = settingsMap[identifiers::controls::ACCENT];
    auto accent = ControllableParameter(accentConfig.m_default,
            accentConfig.m_min,
            accentConfig.m_max);
    m_midiCCToParameterMap[accentConfig.m_cc] = accent;
    (*m_idToParameterMap)[identifiers::controls::ACCENT] = accent;

    // Accent decay
    auto accentDecConfig = settingsMap[identifiers::controls::ACCENT_DECAY];
    auto accentDec = ControllableParameter(accentDecConfig.m_default,
            accentDecConfig.m_min,
            accentDecConfig.m_max,
            ControllableParameter::ScaleType::logarithmic);
    m_midiCCToParameterMap[accentDecConfig.m_cc] = accentDec;
    (*m_idToParameterMap)[identifiers::controls::ACCENT_DECAY] = accentDec;
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
    // DBG(m_presets->toString());
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
        for (auto it = m_idToParameterMap->begin(); it != m_idToParameterMap->end(); ++it)
        {
            const auto& parameterId = it->first;
            auto& parameter = it->second;

            if (preset->hasAttribute(parameterId))
            {
                auto presetValue = preset->getIntAttribute(parameterId);
                parameter.setDiscretValue(presetValue);
            }
        }

        #ifndef TESTING
        // This is goind to spam your test output
        DBG(juce::String("Preset loaded : ") + juce::String(presetId));
        #endif
    }
    else
    {
        #ifndef TESTING
        // This is goind to spam your test output
        DBG(juce::String("No such preset ") + juce::String(presetId));
        #endif
    }
    
}

void MidiBroker::saveToPreset(int presetId)
{
    jassert(m_presets != nullptr);
    auto* newPresetXml = m_presets->createNewChildElement(presetPrefix + juce::String(presetId));

    for (auto it = m_idToParameterMap->begin(); it != m_idToParameterMap->end(); ++it)
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

    if (msg.isControllerOfType(m_savePatchCC))
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

}//namespace control