//
//  ViewController.m
//  Seaboard Demo
//
//  Created by Christopher Fonseka on 03/09/2014.
//  Copyright (c) 2014 ROLI. All rights reserved.
//

#import "ViewController.h"
#import "Seaboard.h"

@interface ViewController()

@property (weak, nonatomic) IBOutlet UITextView *logger;

@end

@implementation ViewController

- (void)appendToLog:(NSString *)message
{
	NSString *newMessage = message;
	newMessage = [newMessage stringByAppendingString:@"\n"];
	newMessage = [newMessage stringByAppendingString:[self.logger text]];
	
	[self.logger setText:newMessage];
}

- (IBAction)midiButtonPressed:(id)sender
{
	
}
- (IBAction)verifyMidiButton:(id)sender
{
	
}

- (void)viewDidLoad
{
    [super viewDidLoad];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

@end
