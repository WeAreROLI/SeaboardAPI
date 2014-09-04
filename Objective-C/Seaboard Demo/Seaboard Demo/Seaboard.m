//
//  Seaboard.m
//  Seaboard Demo
//
//  Created by Christopher Fonseka on 03/09/2014.
//  Copyright (c) 2014 ROLI. All rights reserved.
//

#import "Seaboard.h"
#import <CoreMIDI/CoreMIDI.h>

@interface Seaboard ()
{
	MIDIClientRef	client;
	MIDIPortRef		inputPort;
	MIDIEndpointRef src;
}

@end

@implementation Seaboard

- (id)init
{
	self = [super init];
	if (self)
	{
		
		OSStatus
		
	}
	return self;
}

- (void)printToDelegate:(NSString*)message
{
	if ([self.delegate respondsToSelector:@selector(seabaordDidSendMessage:)])
	{
		[self.delegate seabaordDidSendMessage:message];
	}
}

NSString *getName(MIDIObjectRef object)
{
	// Returns the name of a given MIDIObjectRef as an NSString
	CFStringRef name = nil;
	if (noErr != MIDIObjectGetStringProperty(object, kMIDIPropertyName, &name))
		return nil;
	return (__bridge NSString *)name;
}

static void midiInputCallback(const MIDIPacketList *list, void *procRef, void *srcRef)
{
}

- (void)processMidiDevices
{
}

- (void)verifyMidiDevices
{
}

@end
