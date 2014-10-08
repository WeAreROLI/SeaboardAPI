//
//  MIDIMessage+Private.h
//  Seaboard Demo
//
//  Created by Christopher Fonseka on 08/10/2014.
//  Copyright (c) 2014 ROLI. All rights reserved.
//

#import "MIDIMessage.h"

@interface MIDIMessage (Private)

- (void)setNoteNo:(int)noteNo;

@end

@implementation MIDIMessage (Private)

- (void)setNoteNo:(int)noteNo
{
	self.noteNo = noteNo;
}

@end
