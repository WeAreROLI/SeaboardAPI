//
//  GameScene.swift
//  Breakout
//
//  Created by Christopher Fonseka on 07/10/2014.
//  Copyright (c) 2014 ROLI. All rights reserved.
//

import SpriteKit

enum ColliderType: UInt32
{
	case Block = 1
	case Ball  = 2
}

class GameScene: SKScene, SKPhysicsContactDelegate, SeaboardDelegate
{
	let seaboard : Seaboard
	
	// Boundry Information
	let kMIDILeft = 48
	let kMIDIRight = 72
	
	// Block Info
	let kBlockWidth  : CGFloat = 40
	let kBlockHeight : CGFloat = 20
	let kBlockMargin : CGFloat = 16
	
	// Paddle Info
	let kPaddleWidth : CGFloat = 120
	let kPaddleHeight: CGFloat = 14
	let kPaddleY	 : CGFloat = 40
	
	// Ball Info
	let kBallRadius  : CGFloat = 6
	let kBallVelocityX: CGFloat = 600
	let kBallVelocityY: CGFloat = 600
	
	func newBlock()->SKNode
	{
		let node = SKSpriteNode(color: NSColor.blueColor(), size: CGSizeMake(CGFloat(kBlockWidth), CGFloat(kBlockHeight)))
		node.name = "Block"
		
		node.physicsBody = SKPhysicsBody(rectangleOfSize: node.size)
		node.physicsBody?.dynamic = false
		node.physicsBody?.categoryBitMask = ColliderType.Block.rawValue
		
		self.addChild(node)
		return node
	}
	
	func addBlocks(rows: Int)
	{
		let columns = floor(CGRectGetWidth(self.frame) - kBlockMargin) / (kBlockWidth + kBlockMargin)
		var y = CGRectGetHeight(self.frame) - kBlockMargin - kBlockHeight / 2;
		
		for i in 0...rows
		{
			var x : CGFloat = kBlockMargin + kBlockWidth / 2
			
			for j in 0...(Int)(columns)
			{
				let block = newBlock()
				block.position = CGPointMake(x, y)
				x += (kBlockWidth + kBlockMargin)
			}
			
			y -= kBlockHeight + kBlockMargin
		}
	}
	
	func addPaddle()
	{
		let paddle = SKSpriteNode(color: NSColor.greenColor(), size: CGSizeMake(kPaddleWidth, kPaddleHeight))
		paddle.name = "Paddle"
		paddle.position = CGPointMake(CGRectGetMidX(self.frame), kPaddleY)
		
		paddle.physicsBody = SKPhysicsBody(rectangleOfSize: paddle.size)
		paddle.physicsBody?.dynamic = false
		
		self.addChild(paddle)
	}
	
	func paddleNode()->SKNode!
	{
		return self.childNodeWithName("Paddle")
	}
	
	func blockTouched(block : SKNode)
	{
		block.removeFromParent()
	}
	
	func addBall()
	{
		let ballX = CGRectGetMidX(paddleNode().frame)
		let ballY = CGRectGetMaxY(paddleNode().frame)
		
		let ball = SKShapeNode()
		ball.name = "Ball"
		ball.position = CGPointMake(ballX, ballY)
		
		var path = CGPathCreateMutable()
		CGPathAddArc(path, nil, 0, 0, kBallRadius, 0, CGFloat(M_PI * 2), true);
		ball.path = path
		ball.fillColor = SKColor.yellowColor()
		ball.strokeColor = SKColor.clearColor()
		
		ball.physicsBody = SKPhysicsBody(circleOfRadius: kBallRadius)
		ball.physicsBody?.affectedByGravity = false
		ball.physicsBody?.velocity = CGVectorMake(kBallVelocityX, kBallVelocityY)
		//		ball.physicsBody?.velocity = CGVectorMake(1000, kBallVelocityY)
		ball.physicsBody?.friction = 0
		ball.physicsBody?.restitution = 1
		ball.physicsBody?.linearDamping = 0
		ball.physicsBody?.allowsRotation = false
		ball.physicsBody?.usesPreciseCollisionDetection = true
		ball.physicsBody?.categoryBitMask = ColliderType.Ball.rawValue
		ball.physicsBody?.contactTestBitMask = ColliderType.Block.rawValue
		
		self.addChild(ball)
	}
	
	func ballNode()->SKNode?
	{
		return self.childNodeWithName("Ball")
	}
	
	func didBeginContact(contact: SKPhysicsContact)
	{
		var firstBody : SKPhysicsBody
		var secondBody: SKPhysicsBody
		
		if (contact.bodyA.categoryBitMask < contact.bodyB.categoryBitMask)
		{
			firstBody  = contact.bodyA
			secondBody = contact.bodyB
		}
		else
		{
			firstBody  = contact.bodyB
			secondBody = contact.bodyA
		}
		
		if ((firstBody.categoryBitMask & ColliderType.Block.rawValue) == ColliderType.Block.rawValue)
		{
			if ((secondBody.categoryBitMask & ColliderType.Ball.rawValue) == ColliderType.Ball.rawValue)
			{
				blockTouched(firstBody.node!)
			}
		}
	}
	
	func getPositionForMIDI(message : MIDIMessage)->CGPoint
	{
		var noteNo : Int = (Int)(message.noteNo)
		noteNo = (noteNo < kMIDILeft ? kMIDILeft : noteNo)
		noteNo = (noteNo > kMIDIRight ? kMIDIRight : noteNo)
		
		let noteCount = kMIDIRight - kMIDILeft
		let normalisedPosition = CGFloat(Float(noteNo - kMIDILeft) / Float(noteCount))
		
		let paddleX = self.frame.size.width * normalisedPosition
		
		return CGPointMake(paddleX, kPaddleY)
	}
	
	func seaboardDidGetMIDIMessage(message : MIDIMessage)
	{
		if (message.messageType.value == MIDIMessageTypeNoteOn.value)
		{
			paddleNode().position = getPositionForMIDI(message)
		}
	}
	
	func gameOver()
	{
		ballNode()?.removeFromParent()
	}
	
	override func didSimulatePhysics()
	{
		if ((ballNode() != nil) && ballNode()?.position.y < (kBallRadius * CGFloat(2.0)))
		{
			gameOver()
		}
	}
	
	required init?(coder aDecoder: NSCoder)
	{
		seaboard = Seaboard()
		
		super.init(coder: aDecoder)
		
		self.physicsBody = SKPhysicsBody(edgeLoopFromRect: self.frame)
		self.physicsBody?.friction = 0.0
		self.physicsWorld.contactDelegate = self;
		
		addBlocks(5)
		addPaddle()
		
		seaboard.delegate = self;
		seaboard.connect()
	}
	
	override func didMoveToView(view: SKView) {
		
	}
	
	override func mouseDown(theEvent: NSEvent) {
		/* Called when a mouse click occurs */
		
		if ((ballNode()) == nil)
		{
			addBall()
		}
	}
	
	override func update(currentTime: CFTimeInterval) {
		/* Called before each frame is rendered */
	}
}
