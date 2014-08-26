/*
  ==============================================================================

    SeaboardVisualiser.cpp
    Created: 1 Jul 2014 12:41:02pm
    Author:  Christopher Fonseka

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "SeaboardVisualiser.h"

//==============================================================================

// Value Tree Property Names
const String kChannelNo		= "ChannelNumber";
const String kNoteNo		= "NoteNumber";
const String kAftertouch	= "Aftertouch";
const String kPitchBend		= "PitchBend";
const int minNote = 21; //The minimum midi note number on the Seaboard
const int maxRad = 20; //The maximum radius of note circles
const float numNotes = 87.f; //The number of notes on the Seaboard

#pragma mark Initialisers, Constructors, and Destructors

SeaboardVisualiser::SeaboardVisualiser()
{
	theSeaboardData = ValueTree("SeaboardData");//Set up the Seaboard data value tree and call it "SeaboardData"
	theSeaboardData.addListener(this); //Set this class to respond to changes to the SeaboardData value tree.
}

SeaboardVisualiser::~SeaboardVisualiser()
{
}

#pragma mark Drawing Methods

void SeaboardVisualiser::paint (Graphics& g)
{
	// Clear Background
    g.fillAll (Colours::black);
	
	// Set the drawing colour to white
	g.setColour(Colours::white);
	
	/*
	 For each note being played on the Seaboard, draw a circle.
	 */
	for(int i=0; i<theSeaboardData.getNumChildren(); i++)
	{
		int r = maxRad;
		
		// Position each circle along the x-axis, according to midi note number
		int x = ((int)(theSeaboardData.getChild(i).getProperty(kNoteNo))-minNote)*(getWidth()/numNotes) + (getWidth()/numNotes)*0.5;
		
		//Circles appear half-way down the screen.
		int y = getHeight()/2.0;
		
		//Draw the circle
		g.fillEllipse(x-r, y-r, r*2, r*2);
	}
}

void SeaboardVisualiser::resized()
{
	// Do Nothing
}

#pragma mark Seaboard::Listener Methods

// When this Seaboard::Listener object receives note on/note off messages, we overwrite the following methods and implement our desired response.
void SeaboardVisualiser::seaboardDidGetNoteOn(const juce::MidiMessage &message)
{
	/*
	 We will set up a ValueTree object and add it as a child to the SeaboardData value tree. Its name will be set to the channel number that it is assigned to.
	 */
	
	// Get the relevant midi note information from the incoming message
	int channel = message.getChannel();
	int note = message.getNoteNumber();
	
	// Setup the name for the new child ValueTree structure.
	String childName = kChannelNo + String(channel);
	ValueTree channelInfo(childName);
	
	// Add the note information to the ValueTree structure.
	channelInfo.setProperty(kNoteNo, note, 0);
	channelInfo.addListener(this);
	
	// Add the note value tree as a child to the seaboard data value tree.
	theSeaboardData.addChild(channelInfo, -1, 0);
}

void SeaboardVisualiser::seaboardDidGetNoteOff(const juce::MidiMessage &message)
{
	int channel = message.getChannel();
	String childName = kChannelNo + String(channel);
	ValueTree removeMe = theSeaboardData.getChildWithName(kChannelNo + String(channel));
	// We check if the channel number does have a current note (if the seabaord data tree has a child tree named after the relevant channel). If so, we remove it.
	if (removeMe.isValid())
	{
		theSeaboardData.removeChild(removeMe, 0);
	}
}

#pragma mark ValueTree Listeners
/*
 This is called when the property of a value tree is changed when either that tree, or a parent of that tree, has this object as a listener.
 */
void SeaboardVisualiser::valueTreePropertyChanged(juce::ValueTree &treeWhosePropertyHasChanged, const juce::Identifier &property)
{
	// Ideally the rect to be changed would be calculated here, rather than redrawing everything
	const MessageManagerLock mmLock;
	repaint();
}
/*
 This is called when a child tree is added to a tree that has this object as a listener.
 */
void SeaboardVisualiser::valueTreeChildAdded(ValueTree& parentTree, ValueTree& childWhichHasBeenAdded)
{
	const MessageManagerLock mmLock;
	repaint();
}
/*
 This is called when a child tree is removed from a tree that has this object as a listener.
 */
void SeaboardVisualiser::valueTreeChildRemoved(ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved)
{
	const MessageManagerLock mmLock;
	repaint();
}
