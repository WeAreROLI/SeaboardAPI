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
//
#if defined( linux )
#warning "Using MIDI LINUX"
	#define __LINUX_ALSASEQ__
#elif defined(WIN32)
//#warning "Using MIDI MSW"
	#define __WINDOWS_MM__
#elif defined(__APPLE__)
#warning "Using MIDI MAC"
	#define __MACOSX_CORE__
#endif


// channel info
enum MidiStatus {

    MIDI_UNKNOWN			= 0x00,

    // channel voice messages
    MIDI_NOTE_OFF			= 0x80,
    MIDI_NOTE_ON			= 0x90,
    MIDI_CONTROL_CHANGE		= 0xB0,
    MIDI_PROGRAM_CHANGE		= 0xC0,
    MIDI_PITCH_BEND			= 0xE0,
    MIDI_AFTERTOUCH			= 0xD0,	// aka channel pressure
    MIDI_POLY_AFTERTOUCH	= 0xA0,	// aka key pressure

    // system messages
    MIDI_SYSEX				= 0xF0,
    MIDI_TIME_CODE			= 0xF1,
    MIDI_SONG_POS_POINTER	= 0xF2,
    MIDI_SONG_SELECT		= 0xF3,
    MIDI_TUNE_REQUEST		= 0xF6,
    MIDI_SYSEX_END			= 0xF7,
    MIDI_TIME_CLOCK			= 0xF8,
    MIDI_START				= 0xFA,
    MIDI_CONTINUE			= 0xFB,
    MIDI_STOP				= 0xFC,
    MIDI_ACTIVE_SENSING		= 0xFE,
    MIDI_SYSTEM_RESET		= 0xFF
};