//
//  Seaboard.m
//  Seaboard Demo
//
//  Created by Christopher Fonseka on 03/09/2014.
//  Copyright (c) 2014 ROLI. All rights reserved.
//

#import "Seaboard.h"
#import <CoreMIDI/CoreMIDI.h>
#import "MIDIMessage.h"

#define kMIDI_COMMAND_NOTE_ON 0x09
#define kMIDI_COMMAND_NOTE_OFF 0x08
#define kMIDI_COMMAND_POLY_AFTERTOUCH 0x0A
#define kMIDI_COMMAND_PITCH_BEND 0x0E

static void	MIDIMessageCallback(const MIDIPacketList *pktlist, void *refCon, void *connRefCon);
void MIDINotifyCallback(const MIDINotification *message, void *refCon);

@implementation Seaboard

- (id)init
{
	self = [super init];
	if (self)
	{
	}
	return self;
}

- (void)scanForDevices
{
	MIDIClientRef client;
	MIDIClientCreate(CFSTR("Core MIDI to System Sounds Demo"), MIDINotifyCallback, (__bridge void*)self, &client);
	
	MIDIPortRef inPort;
	MIDIInputPortCreate(client, CFSTR("Input port"), MIDIMessageCallback, (__bridge void*)self, &inPort);
	
	unsigned long sourceCount = MIDIGetNumberOfSources();
	[self printToDelegate:[NSString stringWithFormat:@"%ld sources\n", sourceCount]];
	for (int i = 0; i < sourceCount; ++i) {
		MIDIEndpointRef src = MIDIGetSource(i);
		CFStringRef endpointName = NULL;
		OSStatus nameErr = MIDIObjectGetStringProperty(src, kMIDIPropertyName, &endpointName);
		if (noErr == nameErr)
		{
			[self printToDelegate:[NSString stringWithFormat:@"  source %d: %@\n", i, endpointName]];
		}
		MIDIPortConnectSource(inPort, src, NULL);
	}

}

- (void)printToDelegate:(NSString*)message
{
	if ([self.delegate respondsToSelector:@selector(seabaordDidSendMessage:)])
	{
		[self.delegate seabaordDidSendMessage:message];
	}
}

- (void)processMidiDevices
{
}

- (void)verifyMidiDevices
{
	[self scanForDevices];
}

static void	MIDIMessageCallback(const MIDIPacketList *pktlist, void *refCon, void *connRefCon)
{
	
	Seaboard *sb = (__bridge Seaboard*)refCon;
	[sb printToDelegate:@"MidiREad Haddpened"];
	
	MIDIPacket *packet = (MIDIPacket *)pktlist->packet;
	Byte midiCommand = packet->data[0] >> 4;
	NSString *messageOutput = [NSString new];
	
	switch (midiCommand)
	{
		case kMIDI_COMMAND_NOTE_ON:
		{
			Byte note = packet->data[1] & 0x7F;
			Byte veolocity = packet->data[2] & 0x7F;
			messageOutput = [NSString stringWithFormat:@"Note ON. Note=%d, Velocity=%d", note, veolocity];
		}
			break;
			
		case kMIDI_COMMAND_NOTE_OFF:
			messageOutput = @"Note OFF";
			break;
			
		case kMIDI_COMMAND_POLY_AFTERTOUCH:
			messageOutput = @"Poly Aftertouch";
			break;
			
		case kMIDI_COMMAND_PITCH_BEND:
			messageOutput = @"Poly Pitch Bend";
			break;
			
		default:
			messageOutput = @"Unknown Message";
			break;
	}
	
	[sb printToDelegate:messageOutput];
}


void MIDINotifyCallback (const MIDINotification  *message, void *refCon)
{
	Seaboard *sb = (__bridge Seaboard*)refCon;
	[sb printToDelegate:@"MIDI Notify"];
}

@end
