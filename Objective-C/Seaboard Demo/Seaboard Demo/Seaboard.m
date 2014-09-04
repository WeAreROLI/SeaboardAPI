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
	MIDIEndpointRef src;
}

@end

@implementation Seaboard

- (id)init
{
	self = [super init];
	if (self)
	{
		
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
//	Seaboard *thisClass = (__bridge Seaboard*)procRef;
	MIDIPacket *packet = (MIDIPacket *)list->packet;
    
	Byte midiCommand = packet->data[0] >> 4;
    NSInteger command = midiCommand;
	
    Byte noteByte = packet->data[1] & 0x7F;
    NSInteger note = noteByte;
    
    Byte velocityByte = packet->data[2] & 0x7F;
    NSInteger velocity = velocityByte;
		
	if (command == 9 &&
        velocity > 0) { // "Note On" Event
        
        NSMutableDictionary* info = [[NSMutableDictionary alloc] init];
        [info setObject:[NSNumber numberWithInteger:note] forKey:kSeaboardI_NoteKey];
        [info setObject:[NSNumber numberWithInteger:velocity] forKey:kSeaboardI_VelocityKey];
        NSNotification* notification = [NSNotification notificationWithName:kSeaboardINoteOnNotification object:nil userInfo:info];
        [[NSNotificationCenter defaultCenter] postNotification:notification];
        
	}
    else if(command == 11)
    {
        if(velocity == 127) // Pedal On
        {
            NSMutableDictionary* info = [[NSMutableDictionary alloc] init];
            [info setObject:[NSNumber numberWithInteger:velocity] forKey:kSeaboardI_VelocityKey];
            NSNotification* notification = [NSNotification notificationWithName:kSeaboardIPedalOnNotification object:nil userInfo:info];
            [[NSNotificationCenter defaultCenter] postNotification:notification];
        }
        else { // Pedal off
            NSMutableDictionary* info = [[NSMutableDictionary alloc] init];
            [info setObject:[NSNumber numberWithInteger:velocity] forKey:kSeaboardI_VelocityKey];
            NSNotification* notification = [NSNotification notificationWithName:kSeaboardIPedalOffNotification object:nil userInfo:info];
            [[NSNotificationCenter defaultCenter] postNotification:notification];
        }
    }
    else if(velocity == 0 &&
            (command == 9 || command == 8)) { // Note Off
        
        NSMutableDictionary* info = [[NSMutableDictionary alloc] init];
        [info setObject:[NSNumber numberWithInteger:note] forKey:kSeaboardI_NoteKey];
        [info setObject:[NSNumber numberWithInteger:velocity] forKey:kSeaboardI_VelocityKey];
        NSNotification* notification = [NSNotification notificationWithName:kSeaboardINoteOffNotification object:nil userInfo:info];
        [[NSNotificationCenter defaultCenter] postNotification:notification];
        
    }
	
}

- (void)processMidiDevices
{
	MIDIClientRef	midiClient;
	MIDIPortRef		inputPort;
	OSStatus		status;
	
	status = MIDIClientCreate(CFSTR("MIDI Client"), NULL, NULL, &midiClient);
	if (status != noErr)
	{
		[self printToDelegate:@"ERROR Creating Midi CLient"];
		return;
	}
	else
	{
		[self printToDelegate:@"Midi Client Created"];
	}
	
	status = MIDIInputPortCreate(midiClient, CFSTR("MIDI Input"), midiInputCallback, (__bridge void *)(self), &inputPort);
	if (status != noErr)
	{
		[self printToDelegate:@"ERROR Creating Midi Input port"];
		return;
	}
	else
	{
		[self printToDelegate:@"Midi Input Port Created"];
	}
	
	ItemCount numOfDevices = MIDIGetNumberOfDevices();
	[self printToDelegate:@"Scanning for Midi Devices"];
	for (ItemCount i = 0; i < numOfDevices; i++) {
		MIDIEndpointRef midiDeviceEndpointRef = MIDIGetSource(i);
		[self printToDelegate:[NSString stringWithFormat:@"%lu - %@", i, getName(src)]];
		
		if ([getName(midiDeviceEndpointRef) isEqualToString:@"Seaboard"])
		{
			src = midiDeviceEndpointRef;
			
			MIDIPortConnectSource(inputPort, src, NULL);
			[self printToDelegate:@"-->CONNECTING TO THIS DEVICE"];
			break;
		}
	}
}

- (void)verifyMidiDevices
{
	// How many MIDI devices do we have?
	ItemCount deviceCount = MIDIGetNumberOfDevices();
	
	// Iterate through all MIDI devices
	for (ItemCount i = 0 ; i < deviceCount ; ++i) {
		
		// Grab a reference to current device
		MIDIDeviceRef device = MIDIGetDevice(i);
		[self printToDelegate:[NSString stringWithFormat:@"Device: %@", getName(device)]];
		
		// Is this device online? (Currently connected?)
		SInt32 isOffline = 0;
		MIDIObjectGetIntegerProperty(device, kMIDIPropertyOffline, &isOffline);
		[self printToDelegate:[NSString stringWithFormat:@"Device is online: %s", (isOffline ? "No" : "Yes")]];
		
		// How many entities do we have?
		ItemCount entityCount = MIDIDeviceGetNumberOfEntities(device);
		
		// Iterate through this device's entities
		for (ItemCount j = 0 ; j < entityCount ; ++j) {
			
			// Grab a reference to an entity
			MIDIEntityRef entity = MIDIDeviceGetEntity(device, j);
			[self printToDelegate:[NSString stringWithFormat:@"  Entity: %@", getName(entity)]];
			
			// Iterate through this device's source endpoints (MIDI In)
			ItemCount sourceCount = MIDIEntityGetNumberOfSources(entity);
			for (ItemCount k = 0 ; k < sourceCount ; ++k) {
				
				// Grab a reference to a source endpoint
				MIDIEndpointRef source = MIDIEntityGetSource(entity, k);
				[self printToDelegate:[NSString stringWithFormat:@"    Source: %@", getName(source)]];
			}
			
			// Iterate through this device's destination endpoints
			ItemCount destCount = MIDIEntityGetNumberOfDestinations(entity);
			for (ItemCount k = 0 ; k < destCount ; ++k) {
				
				// Grab a reference to a destination endpoint
				MIDIEndpointRef dest = MIDIEntityGetDestination(entity, k);
				[self printToDelegate:[NSString stringWithFormat:@"    Destination: %@", getName(dest)]];
			}
		}
		[self printToDelegate:@"------"];
	}
}

@end
