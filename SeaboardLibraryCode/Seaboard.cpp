/*
  ==============================================================================

    Seaboard.cpp
    Created: 1 Jul 2014 10:46:40am
    Author:  Christopher Fonseka

  ==============================================================================
*/

#include "Seaboard.h"

#pragma mark Inits, Constructors and Destructors

Seaboard::Seaboard()
{
	findSeaboard();
}

/*
 Search through the available Midi input devices for one named Seaboard. If it is found, stop it, then delete theSeaboardMIDIInput.
 */
Seaboard::~Seaboard()
{
	int seaboardIndex = MidiInput::getDevices().indexOf("Seaboard");
	if (seaboardIndex > -1)
	{
		theSeaboardMIDIInput->stop();
		delete theSeaboardMIDIInput;
	}
}

#pragma mark Helper Methods
/*
 Search through the available Midi input devices for one named Seaboard. If it is found, open it and start it. If not, output a warning that it couldn't be found.
 */
void Seaboard::findSeaboard()
{
	int seaboardIndex = MidiInput::getDevices().indexOf("Seaboard");
	if (seaboardIndex > -1)
	{
		theSeaboardMIDIInput = MidiInput::openDevice(seaboardIndex, this);
		theSeaboardMIDIInput->start();
		return;
	}
	DBG("Unable to find Seaboard");
}

#pragma mark Listener Methods
/*
 This function adds an instance of Seaboard::Listener as a listener to this Seaboard object. Once a Seaboard::Listener object has been added as a listener to a Seaboard object, it can receive the call backs that the Seaboard object calls in response to Midi data from the Seaboard.
 */
void Seaboard::addListener(Seaboard::Listener *listener)
{
	listeners.add(listener);
}
/*
 This function removes a given Seaboard::Listener object from this object`s list of listeners.
 */
void Seaboard::removeListener(Seaboard::Listener *listener)
{
	listeners.remove(listener);
}

#pragma mark MIDIInputCallback Methods
/*
 This function is called whenever the Seaboard object receives some Midi data from the physical Seaboard. It sends out the relevant midi message to all of its listeners, who in return overwrite the midi message functions in order to implement the desired response behaviour.
 */
void Seaboard::handleIncomingMidiMessage(MidiInput *source, const MidiMessage &message)
{
	if (message.isNoteOn())
	{
		listeners.call(&Seaboard::Listener::seaboardDidGetNoteOn,message);
	}
	else if (message.isNoteOff())
	{
		listeners.call(&Seaboard::Listener::seaboardDidGetNoteOff,message);
	}
	else if (message.isAftertouch())
	{
		listeners.call(&Seaboard::Listener::seaboardDidGetAftertouch,message);
	}
	else if (message.isPitchWheel())
	{
		listeners.call(&Seaboard::Listener::seaboardDidGetPitchBend,message);
	}
	listeners.call(&Seaboard::Listener::seaboardDidGetMessage,message);
}

