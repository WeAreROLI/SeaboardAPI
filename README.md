#Music Hack Day

This repository contains tutorial content to get hackers started with the Seaboard GRAND.

If you have not yet, please visit our official [ROLI Music Hack Day Intro Page](https://www.roli.com/musichackday/) to find out more about the Seaboard. If you've just come from there, and you're ready to start hacking, then clone and fork away (or download as a [.zip](https://github.com/WeAreROLI/SeaboardAPI/archive/master.zip)). Inside you will find fully documented tutorials for [JUCE](http://www.juce.com/about-juce) and [Max/MSP](http://cycling74.com/products/max/), and you can also use the Quick Start instructions below to get going.

[![Seaboard](https://www.roli.com/wp-content/uploads/2014/05/Seaboard-GRAND-LFE-Standing.png)](roli.com/seaboard)

###JUCE Quick Start

- Check out [JUCE](http://www.juce.com/about-juce) for background info
- Clone this repo
- Launch one of the `.jucer` Seaboard example files using Introjucer.app
- Choose 'Save Project and Open in Xcode...'
- Connect a Seaboard (ask someone with a ROLI t-shirt)
- Build in Xcode

###CINDER Quick Start

- Place the project file next to your "Cinder" framework directory.
- Select the midi device index of the Seaboard GRAND.
- Edit the number of notes variable to the size of your Seaboard GRAND.
- Run the Cinder project

###PROCESSING Quick Start

- Place the "midibus" library in your processing libraries folder.
- Place the example project in your sketches folder
- Select the midi device index of the Seaboard GRAND.
- Edit the number of notes variable to the size of your Seaboard GRAND.
- Run the Processing sketch.

###Max/MSP Quick Start

- [Install Max/MSP](http://cycling74.com/products/max/) free 30 day trial (we have copies on USB sticks if downloading is slow)
- Clone this repo
- Connect a Seaboard (ask someone with a ROLI t-shirt)
- Launch a Max/MSP patch

###Python Quick Start
- Install python-rtmidi using `sudo pip --pre python-rtmidi`
- Clone this repo
- Connect a Seaboard (ask someone with a ROLI t-shirt)
- Launch Terminal on Mac or Linux, or a Python Environment in Windows
- Navigate to your repo directory and use `python pySeaboard.py Seaboard` to launch

###Feedback

Do you have any feedback for us? If so, please get in touch support@roli.com
