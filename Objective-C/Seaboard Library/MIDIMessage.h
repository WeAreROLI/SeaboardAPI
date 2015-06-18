//
//  MIDINote.h
//  Seaboard Demo
//
//  Created by Christopher Fonseka on 04/09/2014.
//  Copyright (c) 2014 ROLI. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CoreMIDI/CoreMIDI.h>

typedef enum
{
	MIDIMessageTypeUnknown		= 0x00,
    MIDIMessageTypeNoteOn		= 0x09,
    MIDIMessageTypeNoteOff		= 0x08,
	MIDIMessageTypeAftertouch	= 0x0A,
	MIDIMessageTypePitchBend	= 0x0E
} MIDIMessageType;

@interface MIDIMessage : NSObject

@property (nonatomic, readonly) MIDIMessageType messageType;
@property (nonatomic, readonly) int noteNo;
@property (nonatomic, readonly) int channel;
@property (nonatomic, readonly) int velocity;
@property (nonatomic, readonly) int pressure;
@property (nonatomic, readonly) int pitchbend;

- (instancetype)initWithMidiPacket:(MIDIPacket*)packet;
- (NSString*)description;

+ (NSString*)getNameForNote:(int)note;

@end
