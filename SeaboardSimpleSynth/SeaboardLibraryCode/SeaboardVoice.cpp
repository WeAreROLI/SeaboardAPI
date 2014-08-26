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
/** We overwrite the SynthesiserVoice class to implement polyphonic control */
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
	
    void startNote (int midiNoteNumber, float velocity,
                    SynthesiserSound*, int /*currentPitchWheelPosition*/) override
    {
		
    }
	
    void stopNote (bool allowTailOff) override
    {
		
    }
	
    void pitchWheelMoved (int bend) override
    {
		
    }
	
	void aftertouchChanged (int aftertouch) override
	{
	}
	
    void controllerMoved (int /*controllerNumber*/, int /*newValue*/) override
    {
    }
	
    void renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
    {
		
    }
	
private:
	int channel;
};