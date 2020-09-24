/*
  ==============================================================================

    RaciderryVoice.cpp
    Created: 20 Aug 2020 3:24:47pm
    Author:  maxime

  ==============================================================================
*/

#include "Voice.h"

#include "Utils/Utils.h"
#include "Engine/Oscillators/DualOscillator.h"

namespace engine {

Voice::Voice()
    : m_ampEnvelope(),
      m_accEnvelope(),
      m_osc(std::make_shared<DualOscillator>()),
      m_noteStarted(false)
{
    /// Nothing to do here
}

//==============================================================================
std::weak_ptr<DualOscillator> Voice::getOscPtr()
{
    return m_osc;
}

//==============================================================================
void Voice::startNote(int midiNoteNumber, float velocity, 
                               juce::SynthesiserSound *sound, 
                               int currentPitchWheelPosition)
{
    m_osc->setFrequency(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber), m_noteStarted.get());
    m_noteStarted.set(true);
    m_ampEnvelope.noteOn();
    m_accEnvelope.noteOn();
}

void Voice::stopNote(float velocity, bool allowTailOff)
{
    if (allowTailOff)
    {
        m_ampEnvelope.noteOff();
    }
    m_accEnvelope.noteOff();        // does nothing
}

void Voice::pitchWheelMoved(int newValue)
{
    // Nothing to do for now
}

void Voice::controllerMoved(int controllerNumber, int newValue)
{
    // Nothing to do for now
}

void Voice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, 
                                     int startSample, int numSamples)
{
    outputBuffer.clear();

    if (m_noteStarted.get())
    {
        jassert(m_osc != nullptr);
        jassert(outputBuffer.getNumChannels() == 1);

        // Write the osc output to the buffer
        m_osc->process(outputBuffer, startSample, numSamples);

        // Reshape the audio according to the EnvelopeGenerator
        m_ampEnvelope.applyAmpEnvelopeToBuffer(outputBuffer, startSample, numSamples);
    }
    m_accEnvelope.nextValue(numSamples);
}

void Voice::setCurrentPlaybackSampleRate(double newRate)
{
    juce::SynthesiserVoice::setCurrentPlaybackSampleRate(newRate);

    if (newRate != 0.)
    {
        m_ampEnvelope.setSampleRate(newRate);
        m_accEnvelope.setSampleRate(newRate);
    }
}

}//namespace engine