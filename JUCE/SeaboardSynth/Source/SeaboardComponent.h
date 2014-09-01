/*
  ==============================================================================

 SeaboardComponent.h
 Created: 22 Aug 2014 12:00:00pm
 Author: Sean Soraghan

  ==============================================================================
*/

#ifndef MAINCOMPONENT_H_INCLUDED
#define MAINCOMPONENT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "SeaboardVisualiser.h"
#include "SineWaveVoice.cpp"

/*
 The SeaboardComponent class will hold all of our Seaboard responder objects.
 
 SeaboardComponent inherits the JUCE Component class, which is a simple graphical container class.
 The paint() function is the main drawing loop and is called continuously. The resized()
 function is called when the window is resized. It is used for positioning other (child) components
 within this component. Child components can be added to a component using the addChildComponent() method.
 
 Our SeaboardComponent class contains a Seaboard object which manages all of the incoming seaboard data and
 passes it on to any of its listeners (see Seaboard.h).
 
 SeaboardComponent also contains a visualiser object (see SeaboardVisualiser.h).
 
 It also contains a SeaboardPlayer object. We create SeaboardVoice objects and add them to the SeaboardPlayer
 object in order to produce sound from the Seaboard data (see SineWaveVoice.cpp for an example).
 */
class SeaboardComponent   : public Component
{
public:
    //==============================================================================
    SeaboardComponent();
    ~SeaboardComponent();

    void paint (Graphics&);
    void resized();

private:
	ScopedPointer<Seaboard> theSeaboard; //Seaboard objects manage all of the incoming seaboard data and pass it on to Seaboard::Listener objects (see Seaboard.h).
	ScopedPointer<SeaboardVisualiser> theVisualiser; //The SeaboardVisualiser produces visual response to Seaboard data
	ScopedPointer<SeaboardPlayer> theSynth; //We create SeaboardVoice objects and add them to the SeaboardPlayer object in order to produce sound from the Seaboard data (see SineWaveVoice.cpp for an example).
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SeaboardComponent)
};


#endif  // MAINCOMPONENT_H_INCLUDED
