import themidibus.*; //Import the library
import javax.sound.midi.MidiMessage; //Import the MidiMessage classes http://java.sun.com/j2se/1.5.0/docs/api/javax/sound/midi/MidiMessage.html
import javax.sound.midi.SysexMessage;
import javax.sound.midi.ShortMessage;

MidiBus myBus; // The MidiBus

// set how many keys the seaboard has
int numNotes = 37;
    
int currentNum;
int startNote;

// current midi note values for visualisation
float[] noteOns       = new float[numNotes];
float[] noteOffs      = new float[numNotes];
float[] pitchBends    = new float[numNotes];
float[] afterTouch    = new float[numNotes];
float[] cc            = new float[numNotes];

// used to handle polyphonic pitch bend
int[] lastChannelNotes = new int[10]; 



void setup() {
  size(800, 127);
  background(0);
  smooth();
  strokeWeight(2);
  ellipseMode(CENTER);

  MidiBus.list(); // List all available Midi devices on STDOUT. This will show each device's index and name.
  myBus = new MidiBus(this, 2, 0); // Select Seaboard GRAND device
  
  // handles the size of the keyboard variable
  if (numNotes == 37) startNote = 48;
  if (numNotes == 63) startNote = 35;
  if (numNotes == 88) startNote = 0;
  
  for (int i = 0; i < numNotes; ++i)
  {
      noteOns[i] = 0.0f;
      noteOffs[i] = 0.0f;
      pitchBends[i] = 0.0f;
      afterTouch[i] = 0.0f;
  }
  
  for (int i = 0; i < 10; i++)
  {
      lastChannelNotes[i] = 0;
  }
  
  currentNum = 0;
}

void draw() {
  background(0);
  fill(255);
  stroke(255);
  
  float step = width / (float)numNotes;
  
  for (int i = 0; i < numNotes; i++)
  {      
      noFill();
      stroke(255);
      bezier((i+0.5)*step, 0, 
             (i+0.5)*step, afterTouch[i] * 0.5f, 
             (i+0.5)*step, afterTouch[i],
             (i+0.5)*step + pitchBends[i] * 12 * step / 8192, afterTouch[i]);
      
      fill(255);
      ellipse((i+0.5)*step + pitchBends[i] * 12 * step / 8192, afterTouch[i], 5, 5);
             
      // stroke velocity
      noFill();
      stroke(0, 100, 150);
      rect(i*step, 0, step, noteOns[i]);
  }

}







// This handles the midi data including polyphonic pitch bend
//void midiMessage(MidiMessage message) { // You can also use midiMessage(MidiMessage message, long timestamp, String bus_name)
void rawMidi(byte[] data) {
  
  int noteNum = (int)(data[1]);
  int channel = (int)(data[0] & 0x0F);
  int value =   (int)(data[2]);
  currentNum = noteNum;
  
  switch ((byte)(data[0] & 0xF0))
  {
    case (byte)0x90: // NOTE ON
        lastChannelNotes[channel] = noteArrayIndex(noteNum);
        noteOns[noteArrayIndex(noteNum)] = value;
        
        //println("noteOn: " + noteNum + " - Channel: " + channel + " - velocity: " + value);
        break;
        
    case (byte)0x80: // NOTE OFF
        noteOffs[noteArrayIndex(noteNum)] = value;
        noteOns[noteArrayIndex(noteNum)] = 0.0f;
        afterTouch[noteArrayIndex(noteNum)] = 0.0f;
        pitchBends[noteArrayIndex(noteNum)] = 0.0f;
        
        //println("noteOff: " + noteNum + " - Channel: " + channel + " - velocity: " + value);
        break;
        
    case (byte)0xA0: // POLY AFTERTOUCH
        afterTouch[noteArrayIndex(noteNum)] = value;
        
        //println("Aftertouch: " + noteNum + " - Channel: " + channel + " - pressure: " + value);
        break;
        
    case (byte)0xE0: // PITCH BEND
        //gets double byte of pitch bend
        value = (int)(data[2] << 7) + (int) data[1] - 8192;
        // find index of latest channel
        pitchBends[ lastChannelNotes[channel] ] = value;
        
        //println("Pitch Bend: " + noteNum + " - Channel: " + channel + " - bend: " + value);
        break;
        
    default:
        break;
  }
  
}

// handles the note indexes based on the size of the seaboard
int noteArrayIndex( int n )
{
    int idx = 0;
    
    if ((n >= 0 && n < startNote) || n > startNote+numNotes-1 || n < 0) 
    {
      println("index Zeroed"); 
      idx = 0;
    }
    else
    { 
      idx = n - startNote;
    }
    
    if (!(idx < numNotes && idx >= 0)) 
      println("INDEX OUT OF BOUNDS " + idx);
    
    return idx;
}

