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

void Seaboard::addListener(Seaboard::Listener *listener)
{
	listeners.add(listener);
}

void Seaboard::removeListener(Seaboard::Listener *listener)
{
	listeners.remove(listener);
}

#pragma mark MIDIInputCallback Methods

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

