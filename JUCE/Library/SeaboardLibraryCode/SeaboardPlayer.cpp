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
/** A Seaboard Synth object that facilitates polyphonic parameter control. This class is used by SeaboardPlayer objects. 
	In order to produce sound from seaboard input you should create a SeaboardPlayer object and add it as a listener to a
	Seaboard object. Custom voices can be added to a SeaboardSynth through SeaboardPlayer, by using the SeaboardPlayer::addNewVoice() function.
	The SeaboardVoice::renderNextBlock() function will be called by the parent class of SeaboardSynth, juce::Synthesiser.
 **/
struct SeaboardSynth : public Synthesiser
{
	/** Calls the stopNote() function for a given SeaboardVoice */
	void stopVoice (SynthesiserVoice* voice, const bool allowTailOff)
	{
		jassert (voice != nullptr);
		
		voice->stopNote (allowTailOff);
		
		// the subclass MUST call clearCurrentNote() if it's not tailing off! RTFM for stopNote()!
		jassert (allowTailOff || (voice->getCurrentlyPlayingNote() < 0 && voice->getCurrentlyPlayingSound() == 0));
	}
	/** Calls the pitchWheelMoved() function for a given SeaboardVoice */
	void handlePitchWheel (const int midiChannel, const int wheelValue)
	{
		const ScopedLock sl (lock);
		
		for (int i = voices.size(); --i >= 0;)
		{
			SeaboardVoice* const voice = (SeaboardVoice*)voices.getUnchecked (i);
			
			if ((midiChannel <= 0 || voice->getCurrentChannel()==midiChannel))
				voice->pitchWheelMoved (wheelValue);
		}
	}
	/** Calls the afterTouchChanged() function for a given SeaboardVoice */
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
	/** Checks whether a given note is still ringing, and stops it if it is. Then calls the startVoice() function for a given SeaboardVoice */
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
							sound, midiChannel, midiNoteNumber, velocity);
			}
		}
	}
};
//==============================================================================
/** This is an audio source that streams the output of our demo synth. */
struct SynthAudioSource  : public AudioSource
{
    SynthAudioSource ()
    {
        // add a sound for this audio source
        setUsingSynthesisSound();
		
		//In order to play the sound we need to add some voices
		//using the addNewVoice method
    }
	/** Adds a new SeaboardSound to the SeaboardSynth */
    void setUsingSynthesisSound()
    {
        synth.clearSounds();
        synth.addSound (new SeaboardSound());
    }
	/** Adds a SynthesiserVoice to the SeaboardSynth. Note that a SeaboardSound must be added using setUsingSynthesisSound() before the SeaboardSynth can play sounds. */
	void addNewVoice(SynthesiserVoice* newVoice)
	{
		synth.addVoice(newVoice);
	}
	/** Adds a new Sampler sound to the SeaboardVoice */
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
	
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        midiCollector.reset (sampleRate);

        synth.setCurrentPlaybackSampleRate (sampleRate);
    }

    void releaseResources() override
    {
    }
	
	/** Process incoming midi messages from the midi input, then call the renderNextBlock() function of the SeaboardSynth (inherited from juce::Synthesiser). */
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
    /// this collects real-time midi messages from the midi input device, and
    /// turns them into blocks that we can process in our audio callback
    MidiMessageCollector midiCollector;

    // the synth itself!
    SeaboardSynth synth;
};

//==============================================================================
/** The SeaboardPlayer is used to play sounds in response to incoming data from the seaboard. 
	In order to produce sound from seaboard input you should create a SeaboardPlayer and add it
	as a listener to a Seaboad object (using the addListener() function). 
 
	In order to play sound you can then add custom voices using addNewVoice().
	These custom voices should inherit SeaboardVoice.
 */
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
	/** Adds a SeaboardVoice object to the audio source. */
	void addNewVoice(SeaboardVoice* newVoice)
	{
		synthAudioSource->addNewVoice(newVoice);
	}
	/** Add an incoming midi message to the queue. This is called by the various Seaboard responder functions.*/
	void addMessageToQueue(const juce::MidiMessage &message)
	{
		synthAudioSource->midiCollector.addMessageToQueue(message);
	}
    //==============================================================================
	/** Seaboard Note On responder. */
	void seaboardDidGetNoteOn(const juce::MidiMessage &message)
	{
		addMessageToQueue(message);
	}
	/** Seaboard Note Off responder. */
	void seaboardDidGetNoteOff(const juce::MidiMessage &message)
	{
		addMessageToQueue(message);
	}
	/** Seaboard Pitch Bend responder. */
	void seaboardDidGetPitchBend(const juce::MidiMessage & message)
	{
		addMessageToQueue(message);
	}
	/** Seaboard Aftertouch responder. */
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
