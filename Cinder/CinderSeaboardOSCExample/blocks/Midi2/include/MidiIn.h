/*

MidiBlock for Cinder developed by Bruce Lane, Martin Blasko. 
Original code by Hector Sanchez-Pajares(http://www.aerstudio.com).
Midi parsing taken from openFrameworks addon ofxMidi by Theo Watson & Dan Wilcox
 
 All rights reserved.
 

 
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

#include <vector>
#include <string>
#include <iostream>
#include "MidiHeaders.h"
#include "MidiConstants.h"
 #include "boost/signals2.hpp"
/*
#include "MidiConstants.h"
#include "RtMidi.h"
#include <deque>
#include "MidiExceptions.h"
#include "MidiMessage.h"
*/


namespace cinder { namespace midi {

void MidiInCallback( double deltatime, std::vector< unsigned char > *message, void *userData );

class Input {
public:
	Input();
	virtual ~Input();
	
	void processMessage(double deltatime, std::vector<unsigned char> *message);
	void listPorts();
	void openPort(unsigned int port = 0);
	void closePort();
	
	unsigned int getNumPorts()const{ return mNumPorts; }
	unsigned int getPort()const;
	void ignoreTypes(bool sysex, bool time, bool midisense);
	
	std::vector<std::string> mPortNames;
	std::string getName()	{ return mName; };
	std::string getPortName(int number);
	boost::signals2::signal<void(Message)> midiSignal;	
	
	//typedef boost::signal<void (Message*)> signal_t;
	// bool hasWaitingMessages();
	//signal_t mSignal;
	// bool getNextMessage(Message*);

protected:
	
	RtMidiIn *mMidiIn;
	unsigned int mNumPorts;
	unsigned int mPort;
	std::string mName;
	//std::deque<Message*> mMessages;


};

} // namespace midi
} // namespace cinder