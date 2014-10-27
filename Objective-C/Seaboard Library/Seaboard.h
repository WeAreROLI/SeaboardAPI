//
//  Seaboard.h
//  Seaboard Demo
//
//  Created by Christopher Fonseka on 03/09/2014.
//  Copyright (c) 2014 ROLI. All rights reserved.
//

#import <Foundation/Foundation.h>

@class MIDIMessage;

@protocol SeaboardDelegate <NSObject>

- (void)seaboardDidGetMIDIMessage:(MIDIMessage*)message;

@optional
- (void)seabaordDidSendMessage:(NSString*)message;

@end

@interface Seaboard : NSObject

@property (nonatomic, weak) id<SeaboardDelegate> delegate;

- (void)connect;

@end
