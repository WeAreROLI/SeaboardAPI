//
//  ViewController.m
//  Seaboard Demo
//
//  Created by Christopher Fonseka on 03/09/2014.
//  Copyright (c) 2014 ROLI. All rights reserved.
//

#import "ViewController.h"
#import "Seaboard.h"
#import "MIDIMessage.h"

@interface ViewController() <SeaboardDelegate>

@property (nonatomic, retain) Seaboard *seaboard;
@property (weak, nonatomic) IBOutlet UITextView *logger;

@end

@implementation ViewController

- (void)seaboardDidGetMIDIMessage:(MIDIMessage *)message
{
	[self appendToLog:[message description]];
}

- (void)seabaordDidSendMessage:(NSString *)message
{
	[self appendToLog:message];
}

- (void)midiNoteReceived
{
	[self appendToLog:@"New Note!"];
}

- (void)appendToLog:(NSString *)message
{
	dispatch_async(dispatch_get_main_queue(), ^{
		self.logger.text = [NSString stringWithFormat:@"%@%@\n",
							  self.logger.text, message];
		[self.logger scrollRangeToVisible:NSMakeRange(self.logger.text.length-1, 1)];
	});

}

- (IBAction)midiButtonPressed:(id)sender
{
	[self.seaboard verifyMidiDevices];
}

- (void)viewDidLoad
{
    [super viewDidLoad];
	
	self.seaboard = [[Seaboard alloc] init];
	[self.seaboard setDelegate:self];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

@end
