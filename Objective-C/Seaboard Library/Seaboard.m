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
- (void)sendMessageToDelegates:(MIDIMessage *)message
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

@end
