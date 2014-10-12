//
//  GameScene.m
//  Popper
//
//  Created by Christopher Fonseka on 11/10/2014.
//  Copyright (c) 2014 ROLI. All rights reserved.
//

#import "GameScene.h"
#import "Seaboard.h"
#import "MIDIMessage.h"

@interface GameScene () <SeaboardDelegate>

@property (nonatomic, retain) NSTimer *timer;
@property (nonatomic, retain) Seaboard *seaboard;

@end

@implementation GameScene

const int kMIDILeft = 25;
const int kMIDIRight = 72;
const float kBalloonScaleFactor = 4.0;
const int kBalloonSpeedMax = 100;
const int kBalloonSpeedMin = 30;

//======================================================================
#pragma mark Balloons
//======================================================================

- (void)debugPath:(CGPathRef)path onNode:(SKNode*)node {
	SKShapeNode *yourline = [SKShapeNode node];
	yourline.name = @"yourline";
	yourline.path = path;
	[yourline setStrokeColor:[SKColor redColor]];
	[node addChild:yourline];
}

- (int)randomMIDINote
{
	int midiRange = kMIDIRight - kMIDILeft;
	return kMIDILeft + (arc4random() % midiRange);
}

- (SKSpriteNode*)addBalloon
{
	int note = [self randomMIDINote];
	
	SKSpriteNode *sprite = [SKSpriteNode spriteNodeWithImageNamed:@"balloon.png"];
	sprite.name = [NSString stringWithFormat:@"balloon%d", note];
	sprite.physicsBody = [SKPhysicsBody bodyWithRectangleOfSize:sprite.frame.size];
	sprite.physicsBody.affectedByGravity = false;
	int balloonSpeed = kBalloonSpeedMin + (arc4random() % (kBalloonSpeedMax - kBalloonSpeedMin));
	sprite.physicsBody.velocity = CGVectorMake(0, balloonSpeed);
	sprite.physicsBody.friction = 0;
	sprite.physicsBody.restitution = 1;
	sprite.physicsBody.linearDamping = 0;
	sprite.physicsBody.allowsRotation = true;
	sprite.physicsBody.usesPreciseCollisionDetection = false;
	
	sprite.color = [SKColor colorWithDeviceRed:((arc4random() % 100) / 100.0) green:((arc4random() % 100) / 100.0) blue:((arc4random() % 100) / 100.0) alpha:1.0];
	sprite.colorBlendFactor = 1.0;
	
	SKLabelNode *label = [SKLabelNode node];
	label.name = @"NoteNoLabel";
	label.text = [MIDIMessage getNameForNote:note];
	label.fontColor = [SKColor whiteColor];
	label.position = CGPointMake(CGRectGetMidX(sprite.frame), CGRectGetMidY(sprite.frame));
	[sprite addChild:label];
	
	[self addChild:sprite];
	sprite.position = [self getPositionForMIDINote:note];
	return sprite;
}

//======================================================================
#pragma mark GameMechanics
//======================================================================

- (void)setScaleForBalloon:(int)note withFactor:(float)factor
{
	for (SKSpriteNode *balloon in [self children])
	{
		int balloonNote = [[balloon.name stringByReplacingOccurrencesOfString:@"balloon" withString:@""] intValue];
		if (balloonNote == note)
		{
			if (factor > kBalloonScaleFactor - 0.1)
			{
				[balloon removeFromParent];
			}
			else
			{
				[balloon setScale:factor];
			}
		}
	}
}

- (CGPoint)getPositionForMIDINote:(float)note
{
	note = (note < kMIDILeft ? kMIDILeft : note);
	note = (note > kMIDIRight ? kMIDIRight : note);
	
	int noteCount = kMIDIRight - kMIDILeft;
	CGFloat normalisedPosition = (note - kMIDILeft) / (float)noteCount;
	
	CGFloat x = self.frame.size.width * normalisedPosition;
	CGFloat y = 50;
	
	return CGPointMake(x, y);
}

//======================================================================
#pragma mark Seaboard
//======================================================================

- (void)seaboardDidGetMIDIMessage:(MIDIMessage *)message
{
	if (message.messageType == MIDIMessageTypeAftertouch)
	{
		int aftertouch = message.pressure;
		float normAftertouch = aftertouch / 127.f;
		float scale = 1.0 + (normAftertouch * kBalloonScaleFactor);
		
		[self setScaleForBalloon:message.noteNo withFactor:scale];
	}
}

//======================================================================
#pragma mark Phyisics
//======================================================================

- (void)didSimulatePhysics
{
	[self popBalloonsAtTop];
}

- (void)popBalloonsAtTop
{
	for (SKSpriteNode *sprite in [self children])
	{
		if ([sprite.name hasPrefix:@"balloon"] && sprite.position.y >= (self.frame.size.height - sprite.size.height / 1.5))
		{
			[sprite removeFromParent];
		}
	}
}

//======================================================================
#pragma mark NSTimer Callback
//======================================================================

- (void)balloonGenerator
{
	[self addBalloon];
}

//======================================================================
#pragma mark GameScene Methods
//======================================================================

- (instancetype)initWithCoder:(NSCoder *)coder
{
	self = [super initWithCoder:coder];
	if (self) {
		_seaboard = [[Seaboard alloc] init];
		
		_seaboard.delegate = self;
		[_seaboard connect];

	}
	return self;
}

-(void)didMoveToView:(SKView *)view
{
    /* Setup your scene here */
	
	self.physicsBody = [SKPhysicsBody bodyWithEdgeLoopFromRect:self.frame];
	self.physicsBody.friction = 0.0;
	
	self.timer = [NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(balloonGenerator) userInfo:nil repeats:YES];
	[self.timer fire];
}

-(void)mouseDown:(NSEvent *)theEvent {
     /* Called when a mouse click occurs */
}

-(void)update:(CFTimeInterval)currentTime {
    /* Called before each frame is rendered */
}

@end
