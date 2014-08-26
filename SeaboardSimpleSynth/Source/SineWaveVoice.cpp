/*
  ==============================================================================

    SineWaveVoice.cpp
    Created: 22 Aug 2014 10:30:50am
    Author:  Sean Soraghan

  ==============================================================================
*/

#include "JuceHeader.h"
#include "../SeaboardLibraryCode/SeaboardPlayer.cpp"

//==============================================================================
/*
 This file implements a simple structure for a sine wave voice. 
 We inherit the SeaboardVoice class, which provides methods for 
 starting/stopping notes, and responding to incoming midi messages.
 
 The renderNextBlock() function is where we render our audio blocks,
 and is the key function for audio synthesis.
 */

struct SineWaveVoice  : public SeaboardVoice
{
    SineWaveVoice()   : currentAngle (0), angleDelta (0), level (0), tailOff (0)
    {
		
    }
	
	//This function is called when note on messages are received
    void startNote (int midiNoteNumber, float velocity,
                    SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
        currentAngle = 0.0; //Set sinusoidal signal to the beginning.
		level = velocity * 0.15; //Velocity of key strike sets the initial volume.
        tailOff = 0.0; //Release set to 0.
		
        double cyclesPerSecond = MidiMessage::getMidiNoteInHertz (midiNoteNumber); //Set the base frequency according to the midi note number.
        double cyclesPerSample = cyclesPerSecond / getSampleRate(); //Calculate cycles per sample
		
        angleDelta = cyclesPerSample * 2.0 * double_Pi; //Set the base angular frequency.
    }
	
	//This function is called when note off messages are received.
    void stopNote (bool allowTailOff) override
    {
        if (allowTailOff)
        {
            // start a tail-off by setting this flag. The render callback will pick up on
            // this and do a fade out, calling clearCurrentNote() when it's finished.
			
            if (tailOff == 0.0) // we only need to begin a tail-off if it's not already doing so - the
				// stopNote method could be called more than once.
                tailOff = 1.0;
        }
        else
        {
            // we're being told to stop playing immediately, so reset everything..
			
            clearCurrentNote();
            angleDelta = 0.0;
        }
    }

	// This function continuously renders our audio blocks.
	// AudioSampleBuffer& outputBuffer = the audio buffer to be filled.
	// startSample = the beginning sample of the audio buffer.
	// numSamples = the number of samples in the buffer.
	// This example implements a sinusoid signal.
    void renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
        if (angleDelta != 0.0)
        {
            if (tailOff > 0)
            {
                while (--numSamples >= 0)
                {
                    const float currentSample = (float) (sin (currentAngle) * level * tailOff);
					
                    for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample (i, startSample, currentSample);
					
                    currentAngle += angleDelta;
                    ++startSample;
					
                    tailOff *= 0.99;
					
                    if (tailOff <= 0.005)
                    {
                        clearCurrentNote();
						
                        angleDelta = 0.0;
                        break;
                    }
                }
            }
            else
            {
                while (--numSamples >= 0)
                {
                    const float currentSample = (float) (sin (currentAngle) * level);
					
                    for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                        outputBuffer.addSample (i, startSample, currentSample);
					
                    currentAngle += angleDelta;
                    ++startSample;
                }
            }
        }
    }
	
private:
    double currentAngle; //The current angle of a sinusoidal cycle.
	double angleDelta; //The augmented angular frequency (with pitch bend).
	double level; //The volume.
	double tailOff; //The release length.
};
