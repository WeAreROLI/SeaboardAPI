/*
  ==============================================================================

    SineWaveVoice.cpp
    Created: 22 Aug 2014 10:30:50am
    Author:  Sean Soraghan

  ==============================================================================
*/

#include "JuceHeader.h"
#include "../../Library/SeaboardLibraryCode/SeaboardPlayer.cpp"

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
		
        baseAngleDelta = cyclesPerSample * 2.0 * double_Pi; //Set the base angular frequency.
		angleDelta = baseAngleDelta; //Set the current angular frequency to the base angular frequency (no pitch bend yet).
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
	
	// This is called when pitch bend messages are received.
	// Currently pitch bend messages augment the frequency (cyclesPerSecond).
    void pitchWheelMoved (int bend) override
    {
		double cyclesPerSecond = (baseAngleDelta/(2.0 * double_Pi))*getSampleRate();
		double frac = (bend-8192)/8192.0; //This maps the incoming pitch bend message from -1 (minus one octave) to 1 (plus one octave).
		cyclesPerSecond += frac*cyclesPerSecond; //Augment the frequency by the pitch bend amount.
		angleDelta = (cyclesPerSecond/getSampleRate())*2.0*double_Pi; //Convert back to angular frequency.
    }
	
	// This is called when aftertouch messages are received from the Seaboard.
	// Currently aftertouch is directly mapped to volume (level).
	void aftertouchChanged (int aftertouch) override
	{
		//Set the volume level to the normalised aftertouch value multiplied by the maximum volume.
		level = (aftertouch/127.0)*0.15;
	}
	
    void controllerMoved (int /*controllerNumber*/, int /*newValue*/) override
    {
        // not interested in controllers in this case.
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
	double baseAngleDelta; //The base angular frequency (without pitch bend).
	double angleDelta; //The augmented angular frequency (with pitch bend).
	double level; //The volume.
	double tailOff; //The release length.
};
