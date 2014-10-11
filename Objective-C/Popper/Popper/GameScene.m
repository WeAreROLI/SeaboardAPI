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

const int kMIDILeft = 20;
const int kMIDIRight = 90;

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
	sprite.physicsBody.velocity = CGVectorMake(0, 200);
	sprite.physicsBody.friction = 0;
	sprite.physicsBody.restitution = 1;
	sprite.physicsBody.linearDamping = 0;
	sprite.physicsBody.allowsRotation = true;
	sprite.physicsBody.usesPreciseCollisionDetection = false;
	
	SKLabelNode *label = [SKLabelNode node];
	label.name = @"NoteNoLabel";
	label.text = [MIDIMessage getNam];
	label.fontColor = [SKColor whiteColor];
	label.position = CGPointMake(CGRectGetMidX(sprite.frame), CGRectGetMidY(sprite.frame));
	[sprite addChild:label];
	
	[self addChild:sprite];
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
			if (factor > 1.9)
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

//======================================================================
#pragma mark Seaboard
//======================================================================

- (void)seaboardDidGetMIDIMessage:(MIDIMessage *)message
{
	if (message.messageType == MIDIMessageTypeAftertouch)
	{
		int aftertouch = message.pressure;
		float normAftertouch = aftertouch / 127.f;
		float scale = 1.0 + normAftertouch;
		
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
		if ([sprite.name hasPrefix:@"balloon"] && sprite.position.y >= (self.frame.size.height - sprite.size.height / 1.8))
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
	SKSpriteNode *balloon = [self addBalloon];

	int minX	= balloon.size.width / 2;
	int maxX	= self.frame.size.width - balloon.size.width / 2;
	int rangeX	= maxX - minX;
	int y		= balloon.size.height / 2;
	
	int x = minX + arc4random() % rangeX;
	
	balloon.position = CGPointMake(x, y);
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
