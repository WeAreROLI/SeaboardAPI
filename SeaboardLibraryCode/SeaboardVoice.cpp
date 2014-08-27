/*
 ==============================================================================
 
 PolyphonicVoice.cpp
 Created: 22 Aug 2014 11:14:25am
 Author:  Sean Soraghan
 
 ==============================================================================
 */

#include "JuceHeader.h"

//==============================================================================
/** A Seaboard-specific sound */

struct SeaboardSound : public SynthesiserSound
{
    SeaboardSound() {}
	
    bool appliesToNote (const int /*midiNoteNumber*/)           { return true; }
    bool appliesToChannel (const int /*midiChannel*/)           { return true; }
};


//==============================================================================
/** An abstract voice for the seaboard. Subclasses of this should overwrite the various midi response methods in order to implement the desired response behaviour. */
struct SeaboardVoice  : public SynthesiserVoice
{
    SeaboardVoice()
    {
		channel = -1;
    }
	
    bool canPlaySound (SynthesiserSound* sound) override
    {
        return dynamic_cast<SeaboardSound*> (sound) != nullptr;
    }
    void assignToChannel(int channelNum)
	{
		channel = channelNum;
	}
	int getCurrentChannel()
	{
		return channel;
	}
	
	/** Called when Midi Note On messages are received. Subclasses should overwrite this and implement the desired Note On behaviour. */
    void startNote (int midiNoteNumber, float velocity,
                    SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
		
    }
	/** Called when Midi Note Off messages are received. Subclasses should overwrite this and implement the desired Note Off behaviour. */
    void stopNote (bool allowTailOff) override
    {
		
    }
	/** Called when Midi Pitch Bend messages are received. Subclasses should overwrite this and implement the desired Pitch Bend behaviour. */
    void pitchWheelMoved (int bend) override
    {
		
    }
	/** Called when Midi Aftertouch messages are received. Subclasses should overwrite this and implement the desired Aftertouch behaviour. */
	void aftertouchChanged (int aftertouch) override
	{
	}
	/** Called when Midi Control Change messages are received. Subclasses should overwrite this and implement the desired Control Change behaviour. */
    void controllerMoved (int /*controllerNumber*/, int /*newValue*/) override
    {
    }
	
	/* Main Sound Generation Loop. Subclasses should overwrite this in order to implement their own signal generation. */
    void renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
		
    }
	
private:
	int channel;
};