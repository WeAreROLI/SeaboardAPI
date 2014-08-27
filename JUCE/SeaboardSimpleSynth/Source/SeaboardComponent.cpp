/*
  ==============================================================================

 SeaboardComponent.cpp
 Created: 22 Aug 2014 12:00:00pm
 Author: Sean Soraghan

  ==============================================================================
*/

#include "SeaboardComponent.h"
//==============================================================================

const int kWidth	= 800;
const int kHeight	= 600;

SeaboardComponent::SeaboardComponent()
{
	//Set the size of our window.
    setSize (kWidth, kHeight);
	
	//Set up the Seaboard object.
	theSeaboard = new Seaboard();
	
	//Set up the SeaboardPlayer object, which will handle
	theSynth = new SeaboardPlayer();
	
	//Add some custom voices to the SeaboardPlayer (see SineWaveVoice.cpp).
	for (int i=0; i<9; i++)
	{
		theSynth->addNewVoice(new SineWaveVoice());
	}
	
	//Now we add the synth object as a listener to the Seaboard object, such that incoming data from the Seaboard can be passed on.
	theSeaboard->addListener(theSynth);
}

SeaboardComponent::~SeaboardComponent()
{
}

void SeaboardComponent::paint (Graphics& g)
{
	// This is where we would draw graphical content in our window (see SeaboardVisualiser for an example).
}

void SeaboardComponent::resized()
{
	// This is where we reposition our child components whenever the window is resized, by calling the setBounds method for all of the child components.
}
