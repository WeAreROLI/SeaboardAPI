//
//  Seaboard.m
//  Seaboard Demo
//
//  Created by Christopher Fonseka on 03/09/2014.
//  Copyright (c) 2014 ROLI. All rights reserved.
//

#import "Seaboard.h"
#import <CoreMIDI/CoreMIDI.h>
#import "MIDIMessage+Private.h"

//======================================================================
#pragma mark Private Interface
//======================================================================
@interface Seaboard ()

@property (nonatomic, retain) NSMutableDictionary* channelDict;

@end

//======================================================================
#pragma mark Static Callback Methods
//======================================================================
static void	MIDIMessageCallback(const MIDIPacketList *pktlist, void *refCon, void *connRefCon);
void MIDINotifyCallback(const MIDINotification *message, void *refCon);

//======================================================================
#pragma mark Main Implementation
//======================================================================
@implementation Seaboard

- (id)init
{
	self = [super init];
	if (self)
	{
		_channelDict = [NSMutableDictionary dictionary];
	}
	return self;
}

- (void)connect
{
	[self scanForDevicesAndConnect];
}

- (void)scanForDevicesAndConnect
{
	MIDIClientRef client;
	MIDIClientCreate(CFSTR("Seaboard MIDI Client"), MIDINotifyCallback, (__bridge void*)self, &client);
	
	MIDIPortRef inPort;
	MIDIInputPortCreate(client, CFSTR("Input port"), MIDIMessageCallback, (__bridge void*)self, &inPort);
	
	unsigned long sourceCount = MIDIGetNumberOfSources();
	[self printToDelegate:[NSString stringWithFormat:@"Found: %ld sources\n", sourceCount]];
	for (int i = 0; i < sourceCount; ++i) {
		MIDIEndpointRef src = MIDIGetSource(i);
		CFStringRef endpointName = NULL;
		OSStatus nameErr = MIDIObjectGetStringProperty(src, kMIDIPropertyName, &endpointName);
		if (noErr == nameErr)
		{
			[self printToDelegate:[NSString stringWithFormat:@"- %d: %@\n", i, endpointName]];
		}
		MIDIPortConnectSource(inPort, src, NULL);
	}
}

//======================================================================
#pragma mark Delegate Handlers
//======================================================================
- (void)sendMessageToDelegates:(MIDIMessage*)message
{
	if ([self.delegate respondsToSelector:@selector(seaboardDidGetMIDIMessage:)])
	{
		[self.delegate seaboardDidGetMIDIMessage:message];
	}
}

- (void)printToDelegate:(NSString*)message
{
	if ([self.delegate respondsToSelector:@selector(seabaordDidSendMessage:)])
	{
		[self.delegate seabaordDidSendMessage:message];
	}
}

//======================================================================
#pragma mark Callback Methods for MIDI
//======================================================================
static void	MIDIMessageCallback(const MIDIPacketList *pktlist, void *refCon, void *connRefCon)
{
	MIDIPacket *packet = (MIDIPacket *)pktlist->packet;
	MIDIMessage *message = [[MIDIMessage alloc] initWithMidiPacket:packet];
	Seaboard *sb = (__bridge Seaboard*)refCon;
	[sb sendMessageToDelegates:message];
}


void MIDINotifyCallback (const MIDINotification  *message, void *refCon)
{
	Seaboard *sb = (__bridge Seaboard*)refCon;
	[sb printToDelegate:@"MIDI Notify"];
}

//======================================================================
#pragma mark NSNumbers are Annoying
//======================================================================
- (int)getNoteNumberForChannel:(int)channel
{
	return [[self.channelDict objectForKey:[NSNumber numberWithInt:channel]] intValue];
}

- (void)setNoteNumber:(int)noteNo ForChannel:(int)channel
{
	NSNumber *noteNum		= [NSNumber numberWithInt:noteNo];
	NSNumber *channelNum	= [NSNumber numberWithInt:channel];
	
	[self.channelDict setObject:noteNum forKey:channelNum];
}

//======================================================================
#pragma mark Polyphonic Pitch Bend Hacking
//======================================================================
- (void)applyPitchBendHack:(MIDIMessage*)message
{
	switch (message.messageType)
	{
		// If note on, mark channel array with note number
		case MIDIMessageTypeNoteOn:
			[self setNoteNumber:message.noteNo ForChannel:message.channel];
			break;
			
		// if note off, clear channel array
		case MIDIMessageTypeNoteOff:
			[self setNoteNumber:-1 ForChannel:message.channel];
			break;
		
		// if pitch bend, append channel number
		case MIDIMessageTypePitchBend:
			[message setNoteNo:[self getNoteNumberForChannel:message.channel]];
			break;
			
		default:
			break;
	}
	
}

@end
