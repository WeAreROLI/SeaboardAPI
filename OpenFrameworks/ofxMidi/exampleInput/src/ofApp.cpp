/*
 * Copyright (c) 2013 Dan Wilcox <danomatika@gmail.com>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/danomatika/ofxMidi for documentation
 *
 */
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetVerticalSync(true);
	ofBackground(255, 255, 255);
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	// print input ports to console
	midiIn.listPorts(); // via instance
	//ofxMidiIn::listPorts(); // via static as well
	
	// open port by number (you may need to change this)
	midiIn.openPort(2);
	//midiIn.openPort("IAC Pure Data In");	// by name
	//midiIn.openVirtualPort("ofxMidiIn Input"); // open a virtual port
	
	// don't ignore sysex, timing, & active sense messages,
	// these are ignored by default
	midiIn.ignoreTypes(false, false, false);
	
	// add ofApp as a listener
	midiIn.addListener(this);
	
	// print received messages to the console
	midiIn.setVerbose(false);
    
    // handles the size of the keyboard variable
    if (numNotes == 25) startNote = 60;
    if (numNotes == 63) startNote = 35;
    if (numNotes == 88) startNote = 0;
    
    for (int i = 0; i < numNotes; ++i)
    {
        noteOns[i] = 0.0f;
        noteOffs[i] = 0.0f;
        pitchBends[i] = 0.0f;
        afterTouch[i] = 0.0f;
        cc[i] = 0.0f;
        
    }
    
    for (int i = 0; i < 10; i++)
    {
        lastChannelNotes[i] = 0;
    }
    
    currentNum = 0;
}

//--------------------------------------------------------------
void ofApp::update() {
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofSetColor(0);
    
    float step = ofGetWidth() / (float)numNotes;
    
    for (int i = 0; i < numNotes; ++i)
    {
        float x = i*step + pitchBends[i]/683.0f * step;
        float y = ofGetHeight() * 0.5f;
        
        ofFill();
        ofDrawEllipse(i*step, y, 5.0f, 5.0f);
        ofSetLineWidth(2.0f);
        ofDrawLine(i*step, y, x,  y + afterTouch[i]);
        ofDrawEllipse(x, y + afterTouch[i], noteOns[i] * 0.5f, noteOns[i] * 0.5f);
    }
	
}

//--------------------------------------------------------------
void ofApp::exit() {
	
	// clean up
	midiIn.closePort();
	midiIn.removeListener(this);
}

//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& msg) {
    
    int noteNum = msg.pitch;
    
    currentNum = noteNum;
    
    switch (msg.status)
    {
        case MIDI_NOTE_ON:
            lastChannelNotes[msg.channel] = noteArrayIndex(noteNum);
            noteOns[noteArrayIndex(noteNum)] = msg.velocity;
            
            break;
            
        case MIDI_NOTE_OFF:
            noteOffs[noteArrayIndex(noteNum)] = msg.velocity;
            noteOns[noteArrayIndex(noteNum)] = 0.0f;
            afterTouch[noteArrayIndex(noteNum)] = 0.0f;
            pitchBends[noteArrayIndex(noteNum)] = 0.0f;
            
            break;
            
        case MIDI_POLY_AFTERTOUCH:
            afterTouch[noteArrayIndex(noteNum)] = msg.value;
            
            break;
            
        case MIDI_PITCH_BEND:
            // find index of latest channel
            pitchBends[ lastChannelNotes[msg.channel] ] = msg.value - 8192;
            
            break;
            
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	switch(key) {
		case 'l':
			midiIn.listPorts();
			break;
	}
}

int ofApp::noteArrayIndex( int n )
{
    int idx = 0;
    
    if ((n >= 0 && n < startNote) || n > startNote+numNotes-1 || n < 0) idx = 0;
    else idx = n - startNote;
    
    assert (idx < numNotes && idx >= 0);
    
    return idx;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mouseReleased() {
}
