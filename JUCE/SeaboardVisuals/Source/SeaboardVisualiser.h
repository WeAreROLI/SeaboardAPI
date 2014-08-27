/*
  ==============================================================================

    SeaboardVisualiser.h
    Created: 1 Jul 2014 12:41:02pm
    Author:  Christopher Fonseka

  ==============================================================================
*/

#ifndef SEABOARDVISUALISER_H_INCLUDED
#define SEABOARDVISUALISER_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "../../Library/SeaboardLibraryCode/Seaboard.h"

/*
 This is a simple example of a visualiser Component for incoming Seaboard data.
 It inherits the Seaboard::Listener class in order to receive the incoming data from the Seaboard (see Seaboard.h).
 
 It also inherits the ValueTree::Listener class. ValueTree objects are very useful JUCE objects that allow us to store
 Heirarchical information in a tree structure, which can be referenced using Strings. We can add and remove children 
 from the tree, and access their properties by refering to the property name as a String. In this example we store a
 ValueTree object named theSeaboardData. Every time we receive a noteOn message we add a child to the ValueTree, and give 
 it properties such as note number and aftertouch value. We can then access all of the current notes and their parameters by 
 looping through all of the children in the ValueTree.
 
 Our visualiser implementation will simply display current notes as individual circles, with sizes controlled by aftertouch values.
 */

class SeaboardVisualiser    :	public Component,
								public Seaboard::Listener,
								private ValueTree::Listener
{
public:
    SeaboardVisualiser();
    ~SeaboardVisualiser();

    void paint (Graphics&);
    void resized();

private:
	/** 
	 The Seaboard::Listener class provides functions that are called whenever messages are received from the Seaboard.
	 We overwrite them in order to implement our desired functionality.
	 */
	void seaboardDidGetNoteOn(const juce::MidiMessage &message);
	void seaboardDidGetNoteOff(const juce::MidiMessage &message);
	
	/*
	 Whenever theSeaboardData ValueTree is changed one of these functions will be called accordingly. We can implement responses
	 that will allow us to update our graphical information in response to incoming midi information.
	 */
	void valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property);
	void valueTreeChildAdded(ValueTree& parentTree, ValueTree& childWhichHasBeenAdded);
	void valueTreeChildRemoved(ValueTree& parentTree, ValueTree& childWhichHasBeenRemoved);
	void valueTreeChildOrderChanged(ValueTree& parentTreeWhoseChildrenHaveMoved) {};
	void valueTreeParentChanged(ValueTree &treeWhoseParentHasChanged) {};
	
	/*
	 A data tree containing information on all of the current notes being played on the Seaboard.
	 */
	ValueTree theSeaboardData;
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SeaboardVisualiser)
};


#endif  // SEABOARDVISUALISER_H_INCLUDED
