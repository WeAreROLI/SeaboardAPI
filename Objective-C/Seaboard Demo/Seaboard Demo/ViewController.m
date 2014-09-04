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

//======================================================================
#pragma mark SeaboardDelegate methods
//======================================================================

- (void)seaboardDidGetMIDIMessage:(MIDIMessage *)message
{
	[self appendToLog:[message description]];
}

- (void)seabaordDidSendMessage:(NSString *)message
{
	[self appendToLog:message];
}

//======================================================================
#pragma mark Message Log methods
//======================================================================

- (void)appendToLog:(NSString *)message
{
	dispatch_async(dispatch_get_main_queue(), ^{
		self.logger.text = [NSString stringWithFormat:@"%@%@",
							  self.logger.text, message];
		[self.logger scrollRangeToVisible:NSMakeRange(self.logger.text.length-1, 1)];
	});
}

//======================================================================
#pragma mark IBActions
//======================================================================

- (IBAction)connectToSeaboardButtonPressed:(id)sender
{
	[self.seaboard connect];
}

//======================================================================
#pragma mark View Controller methods
//======================================================================

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
