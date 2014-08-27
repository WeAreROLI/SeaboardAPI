/*
  ==============================================================================

    Seaboard.h
    Created: 1 Jul 2014 10:46:40am
    Author:  Christopher Fonseka
 
	Seaboard Class connects to the Seaboard as a MIDIDevice, and converts 
	incoming messages into a series of Seaboard::Listener callbacks
 
	To use, subclass Seboard::Listener, and implement the virutal methods
	relevant to you. You then need to register your class to the Seaboard
	objects ListenerList using the addListener() method
 
	see MainComponent.cpp

  ==============================================================================
*/

#ifndef SEABOARD_H_INCLUDED
#define SEABOARD_H_INCLUDED

#include "JuceHeader.h"

class Seaboard : public MidiInputCallback
{
public:
	Seaboard();
	~Seaboard();

	class Listener
	{
	public:
		/*
		 The following functions should be overwritten by subclasses of Seaboard::Listener in order to respond to midi data from the Seaboard.
		 */
		virtual ~Listener() {};
		virtual void seaboardDidGetNoteOn(const MidiMessage&) {};
		virtual void seaboardDidGetNoteOff(const MidiMessage&) {};
		virtual void seaboardDidGetAftertouch(const MidiMessage&) {};
		virtual void seaboardDidGetPitchBend(const MidiMessage&) {};
		virtual void seaboardDidGetMessage(const MidiMessage&) {};
	};
	/*
	 These functions are used to add/remove listener objects to Seaboard objects.
	 */
	void addListener(Listener *listener);
	void removeListener(Listener *listener);
	
private:
	void handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message);
	void findSeaboard();
	
	ListenerList<Listener> listeners; //A list of Seaboard::Listener objects that receive midi data from this Seaboard object.
	MidiInput *theSeaboardMIDIInput;
};

#endif  // SEABOARD_H_INCLUDED
