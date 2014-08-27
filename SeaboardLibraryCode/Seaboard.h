/*
  ==============================================================================

    Seaboard.h
    Created: 1 Jul 2014 10:46:40am
    Author:  Christopher Fonseka

  ==============================================================================
*/

/**
 Seaboard Class connects to the Seaboard as a MIDIDevice, and converts
 incoming messages into a series of Seaboard::Listener callbacks
 
 To use, subclass Seboard::Listener, and implement the virutal methods
 relevant to you. You then need to register your class to the Seaboard
 objects ListenerList using the addListener() method
 */

#ifndef SEABOARD_H_INCLUDED
#define SEABOARD_H_INCLUDED

#include "JuceHeader.h"

class Seaboard : public MidiInputCallback
{
public:
	Seaboard();
	~Seaboard();
	
	/** This class is used to receive input data from the seaboard. Subclasses should implement the virtual methods that are relevant to them.*/
	class Listener
	{
	public:
		virtual ~Listener() {};
		virtual void seaboardDidGetNoteOn(const MidiMessage&) {};
		virtual void seaboardDidGetNoteOff(const MidiMessage&) {};
		virtual void seaboardDidGetAftertouch(const MidiMessage&) {};
		virtual void seaboardDidGetPitchBend(const MidiMessage&) {};
		virtual void seaboardDidGetMessage(const MidiMessage&) {};
	};
	void addListener(Listener *listener);
	void removeListener(Listener *listener);
	
private:
	/**
	 This function parses an incoming midi message from the seaboard and passes it to all of its listeners.
	 */
	void handleIncomingMidiMessage(MidiInput* source, const MidiMessage& message);

	void findSeaboard();

	/**A list of Seaboard::Listener objects that receive midi data from this Seaboard object.*/
	ListenerList<Listener> listeners;
	MidiInput *theSeaboardMIDIInput;
};

#endif  // SEABOARD_H_INCLUDED
