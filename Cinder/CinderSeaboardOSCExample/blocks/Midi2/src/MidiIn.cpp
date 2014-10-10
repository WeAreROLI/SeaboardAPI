/*
*  MidiIn.cpp
*  glitches
*
*  Created by hec on 5/20/10.
*  Copyright 2010 aer studio. All rights reserved.
*
*/

#include "MidiIn.h"

namespace cinder { namespace midi {

	void MidiInCallback(double deltatime, std::vector<unsigned char> *message, void *userData){
		((Input*)userData)->processMessage(deltatime, message);
	}


	Input::Input(){
		mMidiIn = new RtMidiIn();
		mNumPorts = mMidiIn->getPortCount();
		mMidiIn->getCurrentApi();
	}

	Input::~Input(){
		closePort();
	}

	void Input::listPorts(){
		std::cout << "MidiIn: " << mNumPorts << " available." << std::endl;
		for (size_t i = 0; i < mNumPorts; ++i){
			std::cout << i << ": " << mMidiIn->getPortName(i).c_str() << std::endl;
			mPortNames.push_back(mMidiIn->getPortName(i));
		}
	}

	void Input::ignoreTypes(bool sysex, bool time, bool midisense){
		mMidiIn->ignoreTypes(sysex, time, midisense);

	}

	std::string Input::getPortName(int number){

		return mPortNames.at(number);
	}

	void Input::openPort(unsigned int port){
		if (mNumPorts == 0){
			throw MidiExcNoPortsAvailable();
		}

		if (port + 1 > mNumPorts){
			throw MidiExcPortNotAvailable();
		}

		mPort = port;
		mName = mMidiIn->getPortName(port);

		mMidiIn->openPort(mPort);

		mMidiIn->setCallback(&MidiInCallback, this);

		mMidiIn->ignoreTypes(false, false, false);
	}

	void Input::closePort(){
		mMidiIn->closePort();
	}

	void Input::processMessage(double deltatime, std::vector<unsigned char> *message){
		unsigned int numBytes = message->size();

		// solution for proper reading anything above MIDI_TIME_CODE goes to miguelvb
		// http://forum.openframeworks.cc/t/incorrect-handling-of-midiin-messages-in-ofxmidi-solved/8719
		

			Message msg;
			msg.port = mPort;
			if((message->at(0)) >= MIDI_SYSEX) {
				msg.status = (MidiStatus)(message->at(0) & 0xFF);
				msg.channel = 0;
			} else {
				msg.status = (MidiStatus) (message->at(0) & 0xF0);
				msg.channel = (int) (message->at(0) & 0x0F)+1;
			}


			msg.port = mPort;

			switch(msg.status) {
			case MIDI_NOTE_ON :
			case MIDI_NOTE_OFF:
				msg.pitch = (int) message->at(1);
				msg.velocity = (int) message->at(2);
				break;
			case MIDI_CONTROL_CHANGE:
				msg.control = (int) message->at(1);
				msg.value = (int) message->at(2);
				break;
			case MIDI_PROGRAM_CHANGE:
			case MIDI_AFTERTOUCH:
				msg.value = (int) message->at(1);
				break;
			case MIDI_PITCH_BEND:
				msg.value = (int) (message->at(2) << 7) +
					(int) message->at(1); // msb + lsb
				break;
			case MIDI_POLY_AFTERTOUCH:
				msg.pitch = (int) message->at(1);
				msg.value = (int) message->at(2);
				break;
			default:
				break;
			}
			midiSignal(msg);
		}

		// bool Input::hasWaitingMessages(){
		// 	int queue_length = (int)mMessages.size();
		// 	return queue_length > 0;
		// }

		// bool Input::getNextMessage(Message* message){
		// 	if (mMessages.size() == 0){
		// 		return false;
		// 	}

		// 	Message* src_message = mMessages.front();
		// 	message->copy(*src_message);
		// 	delete src_message;
		// 	mMessages.pop_front();

		// 	return true;
		// }

		unsigned int Input::getPort()const{
			return mPort;

		}


	} // namespace midi
} // namespace cinder