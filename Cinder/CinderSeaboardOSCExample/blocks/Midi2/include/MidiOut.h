/*
 MidiBlock for Cinder developed by Bruce Lane, Martin Blasko. 
Original code by Hector Sanchez-Pajares(http://www.aerstudio.com).
MidiOut written by Tim Murray-Browne (http://timmb.com)
Midi parsing taken from openFrameworks addon ofxMidi by Theo Watson & Dan Wilcox

 This is a block for MIDI Integration for Cinder framework developed by The Barbarian Group, 2010
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this list of conditions and
 the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
 the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include "MidiHeaders.h"
#include "cinder/app/AppBasic.h"

/*#include "MidiConstants.h"
#include "RtMidi.h"
#include "MidiExceptions.h"
#include "MidiMessage.h"
*/

namespace cinder { namespace midi {
///
/// A midi output port
///
/// Create multiple instances to connect to multiple ports.
/// 
/// With thanks to the authors of ofxMidi <https://github.com/danomatika/ofxMidi/network>.
class MidiOut {
	
public:
	
	/// Set the output client name (optional).
	MidiOut(std::string const& name="Cinder-MIDI Client");
	virtual ~MidiOut();
	
	/// \section Global Port Info
	
	/// Get a list of output port names.
	/// The vector index corresponds with the name's port number.
	/// Note: this order may change when new devices are added/removed
	///		  from the system.
	std::vector<std::string> getPortList() const;
	
	/// Get the number of output ports
	int getNumPorts() const;
	
	/// Get the name of an output port by it's number
	/// \return "" if number is invalid
	std::string getPortName(unsigned int portNumber) const;
	
	/// \section Connection
	
	/// Connect to an output port.
	/// Setting port = 0 will open the first available
	bool openPort(unsigned int portNumber=0);
	
	/// Create and connect to a virtual output port (MacOS and Linux ALSA only).
	/// allows for connections between software
	/// note: a connected virtual port has a portNum = -1
	///	note: an open virtual port ofxMidiOut object cannot see it's virtual
	///       own virtual port when listing ports
	///
	bool openVirtualPort(std::string const& portName="ofxMidi Virtual Output");
	
	/// Close the port connection
	void closePort();
	
	/// Get the port number if connected.
	/// \return -1 if not connected or this is a virtual port
	int getPort() const;
	
	/// Get the connected output port name
	/// \return "" if not connected
	std::string getName() const;
	bool isOpen() const;
	bool isVirtual() const;
	
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
	void sendNoteOn(int channel, int pitch, int velocity=64);
	void sendNoteOff(int channel, int pitch, int velocity=64);
	void sendControlChange(int channel, int control, int value);
	void sendProgramChange(int channel, int value);
	void sendPitchBend(int channel, int value);
	void sendPitchBend(int channel, unsigned char lsb, unsigned char msb);
	void sendAftertouch(int channel, int value);
	void sendPolyAftertouch(int channel, int pitch, int value);
	
	/// Low level access
	void sendMessage(std::vector<unsigned char>& bytes);
	void sendMessage(unsigned char status, unsigned char byteOne);
	void sendMessage(unsigned char status, unsigned char byteOne, unsigned char byteTwo);
	
	static bool sVerboseLogging;
private:
	std::string mName;
	std::shared_ptr<RtMidiOut> mRtMidiOut;
	bool mIsVirtual; ///< mIsVirtual => port is open as virtual port
	std::string mPortName;
	int mPortNumber; ///< mPortNumber == -1 => port is not open or is open as virtual port
	// Invariant: mIsVirtual => mPortNumber == -1
	/// Vector used to create midi messages. This should be kept at a length of 3
	/// and may be wiped at any point by any function. It is used by sendMessage( , , )
	std::vector<unsigned char> mBytes;
	// Invariant: mBytes.size() == 3
};

}} // namespaces