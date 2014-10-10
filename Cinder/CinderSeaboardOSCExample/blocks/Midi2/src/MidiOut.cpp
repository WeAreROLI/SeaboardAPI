//
//  MidiOut.cpp
//
//  Created by Tim Murray-Browne on 26/01/2013. See licence and credits in MidiOut.h.
//
//

#include <string>
#include <vector>
#include <sstream>
#include <assert.h>
#include "MidiOut.h"


using namespace cinder::midi;
using namespace std;

bool MidiOut::sVerboseLogging = false;

/// Set the output client name (optional).
MidiOut::MidiOut(std::string const& name)
: mName(name)
, mRtMidiOut(new RtMidiOut())
//, mRtMidiOut(new RtMidiOut(name))
, mPortNumber(-1)
, mIsVirtual(false)
, mBytes(3)
{}

MidiOut::~MidiOut()
{
	closePort();
}

/// Get a list of output port names.
/// The vector index corresponds with the name's port number.
/// Note: this order may change when new devices are added/removed
///		  from the system.
std::vector<std::string> MidiOut::getPortList() const
{
	vector<string> portList;
	for(unsigned int i = 0; i < mRtMidiOut->getPortCount(); ++i)
	{
		portList.push_back(mRtMidiOut->getPortName(i));
	}
	return portList;
}

/// Get the number of output ports
int MidiOut::getNumPorts() const
{
		return mRtMidiOut->getPortCount();
}

/// Get the name of an output port by it's number
/// \return "" if number is invalid
std::string MidiOut::getPortName(unsigned int portNumber) const
{
	return mRtMidiOut->getPortName(portNumber);
}

/// \section Connection

/// Connect to an output port.
/// Setting port = 0 will open the first available
bool MidiOut::openPort(unsigned int portNumber)
{
	// handle rtmidi exceptions
	try
	{
		closePort();
        std::stringstream ss;
        ss << mName << "Output " << portNumber;
		mRtMidiOut->openPort( portNumber, ss.str() );
	}
	catch(RtError& err)
	{
        std::cout << "[ERROR ci::midi::MidiOut::openPort] couldn't open port " << portNumber << " " << err.getMessage() << std::endl;
		return false;
	}
	mPortNumber = portNumber;
	mPortName = mRtMidiOut->getPortName(portNumber);
	if (sVerboseLogging)
		std::cout << "[VERBOSE ci::midi::MidiOut::openPort] opened port " << portNumber << " " << mPortName << std::endl;
	return true;
}

/// Create and connect to a virtual output port (MacOS and Linux ALSA only).
/// allows for connections between software
/// note: a connected virtual port has a portNum = -1
///	note: an open virtual port ofxMidiOut object cannot see it's virtual
///       own virtual port when listing ports
///
bool MidiOut::openVirtualPort(std::string const& portName)
{
	// handle rtmidi exceptions
	try
	{
		closePort();
		mRtMidiOut->openVirtualPort(portName);
	}
	catch(RtError& err) {
        std::cout << "[ERROR ci::midi::MidiOut::openVirtualPort] couldn't open virtual port " << portName << " " << err.getMessage() << std::endl;
		return false;
	}
	
	mPortName = portName;
	mIsVirtual = true;
	if (sVerboseLogging)
		std::cout << "[VERBOSE ci::midi::MidiOut::openVirtualPort] opened virtual port " << portName << std::endl;
	return true;
}

/// Close the port connection
void MidiOut::closePort()
{
	if (sVerboseLogging)
	{
		if(mIsVirtual)
		{
			assert(mPortNumber == -1); // ensure invariant is valid
            std::cout << "[VERBOSE ci::midi::MidiOut::closePort] closed virtual port " << mPortName << std::endl;
		}
		else if(mPortNumber > -1)
		{
            std::cout << "[VERBOSE ci::midi::MidiOut::closePort] closed port " << mPortNumber << ": " << mPortName << std::endl;
		}
	}
	mRtMidiOut->closePort();
	mPortNumber = -1;
	mPortName = "";
	mIsVirtual = false;
}

/// Get the port number if connected.
/// \return -1 if not connected or this is a virtual port
int MidiOut::getPort() const
{
	return mPortNumber;
}

/// Get the connected output port name
/// \return "" if not connected
std::string MidiOut::getName() const
{
	return mPortName;
}

/// \return true if connected
bool MidiOut::isOpen() const
{
	return mPortNumber>-1 || mIsVirtual;
}

/// \return true if this is a virtual port
bool MidiOut::isVirtual() const
{
	return mIsVirtual;
}

/// \section Sending

///
/// midi events
///
/// number ranges:
///		channel			1 - 16
///		pitch			0 - 127
///		velocity		0 - 127
///		control value	0 - 127
///		program value	0 - 127
///		bend value		0 - 16383
///		touch value		0 - 127
///
/// note:
///		- a noteon with vel = 0 is equivalent to a noteoff
///		- send velocity = 64 if not using velocity values
///		- most synths don't use the velocity value in a noteoff
///		- the lsb & msb for raw pitch bend bytes are 7 bit
///
/// references:
///		http://www.srm.com/qtma/davidsmidispec.html
///
void MidiOut::sendMessage(unsigned char status, unsigned char byteOne, unsigned char byteTwo)
{
	assert(mBytes.size() == 3);
	mBytes[0] = status;
	mBytes[1] = byteOne;
	mBytes[2] = byteTwo;
	sendMessage(mBytes);
}

void MidiOut::sendMessage(unsigned char status, unsigned char byteOne)
{
	assert(mBytes.size() == 3);
	mBytes.resize(2);
	mBytes[0] = status;
	mBytes[1] = byteOne;
	sendMessage(mBytes);
	mBytes.resize(3); // restore invariant
}

void MidiOut::sendMessage(std::vector<unsigned char>& bytes)
{
	mRtMidiOut->sendMessage(&bytes);
}

void MidiOut::sendNoteOn(int channel, int pitch, int velocity)
{
	sendMessage(MIDI_NOTE_ON+channel-1, pitch, velocity);
}
void MidiOut::sendNoteOff(int channel, int pitch, int velocity)
{
	sendMessage(MIDI_NOTE_OFF+channel-1, pitch, velocity);
}
void MidiOut::sendControlChange(int channel, int control, int value)
{
	sendMessage(MIDI_CONTROL_CHANGE+channel-1, control, value);
}
void MidiOut::sendProgramChange(int channel, int value)
{
	sendMessage(MIDI_PROGRAM_CHANGE+channel-1, value);
}
void MidiOut::sendPitchBend(int channel, int value)
{
	if (value >>14 != 0)
	{
        std::cout << "[ERROR ci::midi::MidiOut::sendPitchBend] Pitch bend values must be less than " << (1<<14) << std::endl;
	}
	// least significant 7 bits, most significant 7 bits (assuming 14 bit value)
	sendPitchBend(channel, value & 0x7F, (value>>7) & 0x7F);
}
void MidiOut::sendPitchBend(int channel, unsigned char lsb, unsigned char msb)
{
	sendMessage(MIDI_PITCH_BEND, lsb, msb);
}
void MidiOut::sendAftertouch(int channel, int value)
{
	sendMessage(MIDI_AFTERTOUCH+channel-1, value);
}
void MidiOut::sendPolyAftertouch(int channel, int pitch, int value)
{
	sendMessage(MIDI_POLY_AFTERTOUCH+channel-1, pitch, value);
}
