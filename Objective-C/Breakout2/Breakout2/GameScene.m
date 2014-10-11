//
//  GameScene.m
//  Breakout2
//
//  Created by Christopher Fonseka on 08/10/2014.
//  Copyright (c) 2014 ChristmasHouse. All rights reserved.
//

#import "GameScene.h"
#import "Seaboard.h"
#import "MIDIMessage.h"

enum ColliderType
{
	ColliderCategoryBlock = 1,
	ColliderCategoryBall  = 2
};

@interface GameScene () <SKPhysicsContactDelegate, SeaboardDelegate>
{
	int noteValue;
	int pbOffset;
}

@property (nonatomic, retain) Seaboard* seaboard;

@end


@implementation GameScene

// Boundry Information
const int kMIDILeft  = 48;
const int kMIDIRight = 72;

// Block Info
const CGFloat kBlockHeight	= 20;
const CGFloat kBlockWidth	= 40;
const CGFloat kBlockMargin	= 16;

// Paddle Info
const CGFloat kPaddleWidth	= 120;
const CGFloat kPaddleHeight	= 14;
const CGFloat kPaddleY		= 40;

// Ball Info
const CGFloat kBallRadius	= 6;
const CGFloat kBallVelocityX= 600;
const CGFloat kBallVelocityY= 600;

//======================================================================
#pragma mark Blocks
//======================================================================

- (SKNode*)newBlock
{
	SKSpriteNode *block = [SKSpriteNode spriteNodeWithColor:[SKColor colorWithRed:57/255.0 green:127/255.0 blue:186/255.0 alpha:1.0] size:CGSizeMake(kBlockWidth, kBlockHeight)];
	block.name = @"Block";
	block.physicsBody = [SKPhysicsBody bodyWithRectangleOfSize:block.size];
	block.physicsBody.dynamic = false;
	block.physicsBody.categoryBitMask = ColliderCategoryBlock;
	
	[self addChild:block];
	return block;
}

- (void)addBlocks:(int)rows
{
	int columns = floor(CGRectGetWidth(self.frame) - kBlockMargin) / (kBlockWidth + kBlockMargin);
	CGFloat y = CGRectGetHeight(self.frame) - kBlockMargin - kBlockHeight / 2;
	
	for (int i = 0; i < rows; i++)
	{
		CGFloat x = kBlockMargin + kBlockWidth / 2.0;
		for (int j = 0; j < columns; j++)
		{
			SKNode *block = [self newBlock];
			block.position = CGPointMake(x, y);
			x += kBlockWidth + kBlockMargin;
		}
		y -= (kBlockHeight + kBlockMargin);
	}
}

- (void)blockTouched:(SKNode*) block
{
	[block removeFromParent];
}

//======================================================================
#pragma mark Paddle
//======================================================================

- (void)addPaddle
{
	SKSpriteNode *paddle = [SKSpriteNode spriteNodeWithColor:[SKColor greenColor] size:CGSizeMake(kPaddleWidth, kPaddleHeight)];
	paddle.name = @"Paddle";
	paddle.position = CGPointMake(CGRectGetMidX(self.frame), kPaddleY);
	paddle.physicsBody = [SKPhysicsBody bodyWithRectangleOfSize:paddle.size];
	paddle.physicsBody.dynamic = false;
	[self addChild:paddle];
}

- (SKNode*)paddleNode
{
	return [self childNodeWithName:@"Paddle"];
}

- (void)movePaddle
{
	float normalisedPitchBendOffset = ((pbOffset - 8192) / 8192.f) * 13;
	NSLog(@"PB Offset: %d", pbOffset);
	// Need to filter pb reset messages
	if (normalisedPitchBendOffset > 12 || normalisedPitchBendOffset < -12)
	{
		return;
	}
	
	float derivedNote = noteValue + normalisedPitchBendOffset;
	
	[self paddleNode].position = [self getPositionForMIDINote:derivedNote];
}

- (CGPoint)getPositionForMIDINote:(float)note
{
	
	NSLog(@"Note is : %f", note);
	
	note = (note < kMIDILeft ? kMIDILeft : note);
	note = (note > kMIDIRight ? kMIDIRight : note);
	
	int noteCount = kMIDIRight - kMIDILeft;
	CGFloat normalisedPosition = (note - kMIDILeft) / (float)noteCount;
	
	CGFloat paddleX = self.frame.size.width * normalisedPosition;
	
	return CGPointMake(paddleX, kPaddleY);
}

//======================================================================
#pragma mark Ball
//======================================================================

- (void)addBall
{
	SKShapeNode *ball = [SKShapeNode node];
	ball.name = @"Ball";
	ball.position = CGPointMake(CGRectGetMidX([self paddleNode].frame),
								CGRectGetMaxY([self paddleNode].frame));
	
	CGMutablePathRef path = CGPathCreateMutable();
	CGPathAddArc(path, nil, 0, 0, kBallRadius, 0, M_PI * 2.0, true);
	ball.path = path;
	ball.fillColor = [SKColor yellowColor];
	ball.strokeColor = [SKColor clearColor];
	
	ball.physicsBody = [SKPhysicsBody bodyWithCircleOfRadius:kBallRadius];
	ball.physicsBody.affectedByGravity = false;
	ball.physicsBody.velocity = CGVectorMake(kBallVelocityX, kBallVelocityY);
	ball.physicsBody.friction = 0;
	ball.physicsBody.restitution = 1;
	ball.physicsBody.linearDamping = 0;
	ball.physicsBody.allowsRotation = false;
	ball.physicsBody.usesPreciseCollisionDetection = true;
	ball.physicsBody.categoryBitMask = ColliderCategoryBall;
	ball.physicsBody.contactTestBitMask = ColliderCategoryBlock;
	
	[self addChild:ball];
}

- (SKNode*)ballNode
{
	return [self childNodeWithName:@"Ball"];
}

//======================================================================
#pragma mark Game Mechanics
//======================================================================

- (void)didBeginContact:(SKPhysicsContact *)contact
{
	SKPhysicsBody *firstBody;
	SKPhysicsBody *secondBody;
	
	if (contact.bodyA.categoryBitMask < contact.bodyB.categoryBitMask)
	{
		firstBody = contact.bodyA;
		secondBody = contact.bodyB;
	}
	else
	{
		firstBody = contact.bodyB;
		secondBody = contact.bodyA;
	}
	
	if (firstBody.categoryBitMask & ColliderCategoryBlock)
	{
		if (secondBody.categoryBitMask & ColliderCategoryBall)
		{
			[self blockTouched:firstBody.node];
		}
	}
}

- (void)gameOver
{
	[[self ballNode] removeFromParent];
	[self addInstructions];
}

- (void)didSimulatePhysics
{
	if ([self ballNode] != nil &&
		[self ballNode].position.y < (kBallRadius * 2.0))
	{
		[self gameOver];
	}
}

//======================================================================
#pragma mark Seaboard Code
//======================================================================


- (void)seaboardDidGetMIDIMessage:(MIDIMessage *)message
{
	
	
	if (message.messageType == MIDIMessageTypeNoteOn)
	{
		noteValue = message.noteNo;
		[self movePaddle];
	}
	else if (message.messageType == MIDIMessageTypeAftertouch)
	{
		pbOffset = message.pitchbend;
		[self movePaddle];
	}
}

//======================================================================
#pragma mark Instructions Label
//======================================================================

- (void)addInstructions
{
	SKLabelNode *instructions = [SKLabelNode node];
	instructions.name = @"Instructions";
	instructions.text = @"Click to Start";
	instructions.fontColor = [SKColor whiteColor];
	instructions.position = CGPointMake(CGRectGetMidX(self.frame),
										CGRectGetMidY(self.frame));
	[self addChild:instructions];
}

- (SKNode*)instructionsLabel
{
	return [self childNodeWithName:@"Instructions"];
}

//======================================================================
#pragma mark Class Setup
//======================================================================

- (instancetype)initWithCoder:(NSCoder *)aDecoder
{
	self = [super initWithCoder:aDecoder];
	if (self)
	{
		noteValue = 0;
		pbOffset = 0;
		
		_seaboard = [[Seaboard alloc] init];
		
		self.physicsBody = [SKPhysicsBody bodyWithEdgeLoopFromRect:self.frame];
		self.physicsBody.friction = 0.0;
		self.physicsWorld.contactDelegate = self;
		
		[self addBlocks:6];
		[self addPaddle];
		[self addInstructions];
		
		_seaboard.delegate = self;
		[_seaboard connect];
		
		self.backgroundColor = [SKColor colorWithRed:50/255.0 green:61/255.0 blue:71/255.0 alpha:1.0];
	}
	return self;
}

-(void)mouseDown:(NSEvent *)theEvent {
	if ([self ballNode] == nil)
	{
		[self addBall];
		[[self instructionsLabel] removeFromParent];
	}
}

-(void)update:(CFTimeInterval)currentTime {
    /* Called before each frame is rendered */
}

@end
