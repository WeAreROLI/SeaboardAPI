/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-12 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#include "JuceHeader.h"
#include "Seaboard.h"
#include "BinaryData.h"
#include "SeaboardVoice.cpp"
//==============================================================================
/** We overwrite the JUCE Synthesiser class to allow for polyphonic pitch bend and aftertouch control **/

struct SeaboardSynth : public Synthesiser
{
	void stopVoice (SynthesiserVoice* voice, const bool allowTailOff)
	{
		jassert (voice != nullptr);
		
		voice->stopNote (allowTailOff);
		
		// the subclass MUST call clearCurrentNote() if it's not tailing off! RTFM for stopNote()!
		jassert (allowTailOff || (voice->getCurrentlyPlayingNote() < 0 && voice->getCurrentlyPlayingSound() == 0));
	}
	void handlePitchWheel (const int midiChannel, const int wheelValue)
	{
		const ScopedLock sl (lock);
		
		for (int i = voices.size(); --i >= 0;)
		{
			SeaboardVoice* const voice = (SeaboardVoice*)voices.getUnchecked (i);
			
			if ((midiChannel <= 0 || voice->getCurrentChannel()==midiChannel))// voice->isPlayingChannel (midiChannel)
				voice->pitchWheelMoved (wheelValue);
		}
	}
	void handleAftertouch (int midiChannel, int midiNoteNumber, int aftertouchValue)
	{
		const ScopedLock sl (lock);
		
		for (int i = voices.size(); --i >= 0;)
		{
			SeaboardVoice* const voice = (SeaboardVoice*)voices.getUnchecked (i);
			
			if (voice->getCurrentlyPlayingNote() == midiNoteNumber
				&& (midiChannel <= 0 || voice->getCurrentChannel()==midiChannel))
				voice->aftertouchChanged (aftertouchValue);
		}
	}
	void noteOn (const int midiChannel,
							  const int midiNoteNumber,
							  const float velocity)
	{
		const ScopedLock sl (lock);
		
		for (int i = sounds.size(); --i >= 0;)
		{
			SynthesiserSound* const sound = sounds.getUnchecked(i);
			
			if (sound->appliesToNote (midiNoteNumber)
				&& sound->appliesToChannel (midiChannel))
			{
				// If hitting a note that's still ringing, stop it first (it could be
				// still playing because of the sustain or sostenuto pedal).
				for (int j = voices.size(); --j >= 0;)
				{
					SynthesiserVoice* const voice = voices.getUnchecked (j);
					
					if (voice->getCurrentlyPlayingNote() == midiNoteNumber
						&& voice->isPlayingChannel (midiChannel))
						stopVoice (voice, true);
				}
				SeaboardVoice* voice = (SeaboardVoice*)findFreeVoice(sound, true);
				voice->assignToChannel(midiChannel);
				startVoice (voice,
							sound, midiChannel, midiNoteNumber, velocity);//shouldStealNotes
			}
		}
	}
};
//==============================================================================
// This is an audio source that streams the output of our demo synth.
struct SynthAudioSource  : public AudioSource
{
    SynthAudioSource ()
    {
        // add a sound for this audio source
        setUsingSynthesisSound();
		
		//In order to play the sound we need to add some voices
		//using the addNewVoice method
    }

    void setUsingSynthesisSound()
    {
        synth.clearSounds();
        synth.addSound (new SeaboardSound());
    }

	void setUsingSampledSound()
    {
        WavAudioFormat wavFormat;
		
        ScopedPointer<AudioFormatReader> audioReader (wavFormat.createReaderFor (new MemoryInputStream (BinaryData::cello_wav,
                                                                                                        BinaryData::cello_wavSize,
                                                                                                        false),
                                                                                 true));
		
        BigInteger allNotes;
        allNotes.setRange (0, 128, true);
		
        synth.clearSounds();
        synth.addSound (new SamplerSound ("demo sound",
                                          *audioReader,
                                          allNotes,
                                          74,   // root midi note
                                          0.1,  // attack time
                                          0.1,  // release time
                                          10.0  // maximum sample length
                                          ));
    }
	
	void addNewVoice(SynthesiserVoice* newVoice)
	{
		synth.addVoice(newVoice);
	}
	
    void prepareToPlay (int /*samplesPerBlockExpected*/, double sampleRate) override
    {
        midiCollector.reset (sampleRate);

        synth.setCurrentPlaybackSampleRate (sampleRate);
    }

    void releaseResources() override
    {
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        // the synth always adds its output to the audio buffer, so we have to clear it
        // first..
        bufferToFill.clearActiveBufferRegion();

        // fill a midi buffer with incoming messages from the midi input.
        MidiBuffer incomingMidi;
        midiCollector.removeNextBlockOfMessages (incomingMidi, bufferToFill.numSamples);

        // and now get the synth to process the midi events and generate its output.
        synth.renderNextBlock (*bufferToFill.buffer, incomingMidi, 0, bufferToFill.numSamples);
    }

    //==============================================================================
    // this collects real-time midi messages from the midi input device, and
    // turns them into blocks that we can process in our audio callback
    MidiMessageCollector midiCollector;

    // the synth itself!
    SeaboardSynth synth;
	// the number of independent voices (polyphony number).
};

//==============================================================================
class SeaboardPlayer : public Seaboard::Listener
{
public:
    SeaboardPlayer()
    {		
		deviceManager = new AudioDeviceManager();
        deviceManager->initialise (2, 2, 0, true, String::empty, 0);
        deviceManager->addAudioCallback (&audioSourcePlayer);
		
		synthAudioSource = new SynthAudioSource();
		audioSourcePlayer.setSource (synthAudioSource);
		deviceManager->addMidiInputCallback (String::empty, &(synthAudioSource->midiCollector));
    }

    ~SeaboardPlayer()
    {
        audioSourcePlayer.setSource (nullptr);
        deviceManager->removeMidiInputCallback (String::empty, &(synthAudioSource->midiCollector));
        deviceManager->removeAudioCallback (&audioSourcePlayer);
    }
	void addNewVoice(SeaboardVoice* newVoice)
	{
		synthAudioSource->addNewVoice(newVoice);
	}
	void addMessageToQueue(const juce::MidiMessage &message)
	{
		synthAudioSource->midiCollector.addMessageToQueue(message);
	}
    //==============================================================================
	// Seaboard responders
	void seaboardDidGetNoteOn(const juce::MidiMessage &message)
	{
		addMessageToQueue(message);
	}
	void seaboardDidGetNoteOff(const juce::MidiMessage &message)
	{
		addMessageToQueue(message);
	}
	void seaboardDidGetPitchBend(const juce::MidiMessage & message)
	{
		addMessageToQueue(message);
	}
	void seaboardDidGetAftertouch(const juce::MidiMessage & message)
	{
		addMessageToQueue(message);
	}

private:
	
    ScopedPointer<AudioDeviceManager> deviceManager;
    AudioSourcePlayer audioSourcePlayer;
    ScopedPointer<SynthAudioSource> synthAudioSource;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SeaboardPlayer)
};
