//
//  MIDINote.m
//  Seaboard Demo
//
//  Created by Christopher Fonseka on 04/09/2014.
//  Copyright (c) 2014 ROLI. All rights reserved.
//

#import "MIDIMessage.h"

@implementation MIDIMessage

- (instancetype)initWithMidiPacket:(MIDIPacket *)packet
{
	self = [super init];
	if (self)
	{
		_messageType	= [self getTypeForPacket:packet];
		_noteNo			= [self getNoteNumberForPacket:packet];
		_channel		= [self getChannelForPacket:packet];
		_velocity		= [self getVelocityForPacket:packet];
		_pressure		= [self getPressureForPacket:packet];
		_pitchbend		= [self getPitchBendForPacket:packet];
	}
	return self;
}

- (NSString *)description
{
	switch (self.messageType)
	{
		case MIDIMessageTypeNoteOn:
			return [NSString stringWithFormat:@"[Note On] - Channel:%d, Note:%d, Velocity:%d", self.channel, self.noteNo, self.velocity];
			
		case MIDIMessageTypeNoteOff:
			return [NSString stringWithFormat:@"[Note Off] - Channel:%d, Note:%d, Velocity:%d", self.channel, self.noteNo, self.velocity];
			
		case MIDIMessageTypeAftertouch:
			return [NSString stringWithFormat:@"[Aftertouch] - Channel:%d, Note:%d, Pressure:%d", self.channel, self.noteNo, self.pressure];
		
		case MIDIMessageTypePitchBend:
			return [NSString stringWithFormat:@"[Pitch Bend] - Channel:%d, Bend:%d", self.channel, self.noteNo];
			
		default:
			break;
	}
	return @"";
}

- (MIDIMessageType)getTypeForPacket:(MIDIPacket *)packet
{
	Byte midiCommand = packet->data[0] >> 4;
	return (MIDIMessageType)midiCommand;
}

- (int)getNoteNumberForPacket:(MIDIPacket *)packet
{
	MIDIMessageType type = [self getTypeForPacket:packet];
	switch (type)
	{
		case MIDIMessageTypeNoteOn:
		case MIDIMessageTypeNoteOff:
		case MIDIMessageTypeAftertouch:
			return packet->data[1];
			
		default:
			break;
	}
	return -1;
}

- (int)getChannelForPacket:(MIDIPacket *)packet
{
	return (int)(packet->data[0] & 0x0F);
}

- (int)getVelocityForPacket:(MIDIPacket *)packet
{
	MIDIMessageType type = [self getTypeForPacket:packet];
	switch (type)
	{
		case MIDIMessageTypeNoteOn:
		case MIDIMessageTypeNoteOff:
			return packet->data[2];
		
		default:
			break;
	}
	return -1;
}

- (int)getPressureForPacket:(MIDIPacket *)packet
{
	MIDIMessageType type = [self getTypeForPacket:packet];
	switch (type)
	{
		case MIDIMessageTypeAftertouch:
			return packet->data[2];
		
		default:
			break;
	}
	return -1;
}

- (int)getPitchBendForPacket:(MIDIPacket *)packet
{
	MIDIMessageType type = [self getTypeForPacket:packet];
	if (type == MIDIMessageTypePitchBend)
	{
		Byte MSB = packet->data[2];
		Byte LSB = packet->data[1];

		return (int)(MSB << 7) + LSB;
	}
	return -1;
}

+ (NSString *)getNameForNote:(int)note
{
	NSArray *noteString = [NSArray arrayWithObjects:@"C", @"C#", @"D", @"D#", @"E", @"F", @"F#", @"G", @"G#", @"A", @"A#", @"B", nil];
	
	int octave = (note / 12) - 1;
	int noteIndex = (note % 12);
	
	return [NSString stringWithFormat:@"%@%d", noteString[noteIndex], octave];

}

@end
