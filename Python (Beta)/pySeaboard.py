"""
  Created: 2 Sep 2014 10:18:44am
  Author:  Christopher Fonseka
  Copyright: ROLI, Lambda LTD

  pySeboard.py is a simple demo of using python as an input handler
  for the Seboard GRAND, STAGE, and STUDIO. It utilises the
  python-rtMidi library, and decodes each message to call a specific
  method based on the message type.

  In this demo, the decoded message data is simply outputted to the
  terminal window.

  python-rtmidi can be found here:
    http://trac.chrisarndt.de/code/wiki/python-rtmidi
  and can be installed in terminal with this command:
    sudo pip install --pre python-rtmidi

  The application can be launched with:
    python pySeaboard.py Seaboard

  "Seaboard" may be replaced with the name of another MIDI Input
  device, if desired
"""

import sys
import time
import rtmidi
from rtmidi.midiutil import open_midiport

MIDI_OFF             = 128
MIDI_ON              = 144
MIDI_POLY_AFTERTOUCH = 160
MIDI_CC              = 176
MIDI_PITCH_BEND      = 224
MIDI_SYSEX           = 240

"""
  Seaboard Input Handler is a class designed to handle the MIDI
  input callback messages. The message data is parsed and the
  appropriate method is called based on the message type.
"""
class SeaboardInputHandler(object):
  def __init__(self, port):
    self.port = port

  # Callback Method and Parser
  def __call__(self, event, data=None):
    message = event
    midiData = message[0]
    byte0 = midiData[0]

    if byte0 >= MIDI_OFF and byte0 < MIDI_ON:
      self.noteOff(midiData)

    elif byte0 >= MIDI_ON and byte0 < MIDI_POLY_AFTERTOUCH:
      self.noteOn(midiData)

    elif byte0 >= MIDI_POLY_AFTERTOUCH and byte0 < MIDI_CC:
      self.aftertouch(midiData)

    elif byte0 >= MIDI_PITCH_BEND and byte0 < MIDI_SYSEX:
      self.pitchbend(midiData)

  # MIDI Message Callback Methods
  def noteOn(self, data):
    channel = data[0] - MIDI_OFF + 1
    note = data[1]
    velocity = data[2]
    print "Note On - Ch:" + str(channel) +
          " Note:" + str(note) + " Velocity:" + str(velocity)

  def noteOff(self, data):
    channel = data[0] - MIDI_ON + 1
    note = data[1]
    velocity = data[2]
    print "Note Off - Ch:" + str(channel) +
          " Note:" + str(note) + " Velocity:" + str(velocity)

  def aftertouch(self, data):
    channel = data[0] - MIDI_POLY_AFTERTOUCH + 1
    note = data[1]
    pressure = data[2]
    print "Aftertouch - Ch:" + str(channel) +
          " Note:" + str(note) + " Pressure:" + str(pressure)

  def pitchbend(self, data):
    channel = data[0] - MIDI_PITCH_BEND + 1
    MSB = data[2]
    LSB = data[1]
    bend = (MSB << 4) + LSB
    print "Pitch Bend - Ch: " + str(channel) +
          " Bend:" + str(bend)

"""
  MIDI Input Device Connection Establishment
  Note that if unable to connect, or no device is inputted
  as an argument: a list of available MIDI input devices will
  be presented.
"""
port = sys.argv[1] if len(sys.argv) > 1 else None
try:
  midiin, port_name = open_midiport(port)
except (EOFError, KeyboardInterrupt):
  sys.exit()

print("Attaching Midi Callback Handler")
midiin.set_callback(SeaboardInputHandler(port_name))

"""
  Main Loop
  This loop will run until a Keyboard Interrput occurs
  (CTRL + C)
"""
print("Entering Main Loop, CTRL+C to Exit")
try:
  while True:
    time.sleep(1)

except KeyboardInterrupt:
  print(" ")

finally:
  print("Exit..")
  midiin.close_port()
  del midiin
