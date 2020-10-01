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

MidiBroker::MidiBroker()
{
    initParameters();
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
    if (msg.isNoteOnOrOff())
    {
        handleNoteMessage(msg);
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

    // Filter Drive
    auto drive = ControllableParameter(parameters::values::DRIVE_DEFAULT,
            parameters::values::DRIVE_MIN,
            parameters::values::DRIVE_MAX);
    m_midiCCToParameterMap[parameters::midiCC::FILTER_DRIVE] = drive;
    m_idToParameterMap[identifiers::controls::DRIVE] = drive;

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