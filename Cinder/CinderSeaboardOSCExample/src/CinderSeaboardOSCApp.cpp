#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "MidiIn.h"
#include "MidiConstants.h"
#include "MidiMessage.h"
#include "cinder/Font.h"
#include "OscSender.h"
#include "cinder/System.h"
#include "cinder/gl/TextureFont.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CinderSeaboardOSCApp : public AppNative {
  public:
    void prepareSettings( Settings *settings);
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    void midiListener(midi::Message msg);
    int noteArrayIndex(int n);
    
    midi::Input mInput;
    
    Font				mFont;
    gl::TextureFontRef	mTextureFont;
    
    
    // OSC SETUP
    osc::Sender sender;
    std::string host;
    int 		port;
    
    long time = 0;
    
    
    // SEABOARD MIDI INFORMATION /////////////////////////////////////
    
    // set how many keys the seaboard has
    const static int numNotes = 74;
    
    int currentNum;
    
    int startNote;
    
    // current midi note values for visualisation
    float noteOns       [numNotes];
    float noteOffs      [numNotes];
    float pitchBends    [numNotes];
    float afterTouch    [numNotes];
    float cc            [numNotes];
    
    int lastChannelNotes [16];
};




void CinderSeaboardOSCApp::prepareSettings( Settings *settings ){
    settings->setWindowSize( 800, 500 );
    //settings->setFrameRate( 60.0f );
}

void CinderSeaboardOSCApp::setup()
{
    mInput.listPorts();
    console() << "NUMBER OF PORTS: " << mInput.getNumPorts() << endl;
    for (int i = 0; i < mInput.getNumPorts(); i++)
    {
        console() << mInput.getPortName(i) << endl;
    }
    mInput.openPort(0);
    
    mInput.midiSignal.connect(boost::bind(&CinderSeaboardOSCApp::midiListener, this, boost::arg<1>::arg()));
    
    // OSC SETUP
    port = 3000;
    // assume the broadcast address is this machine's IP address but with 255 as the final value
    // so to multicast from IP 192.168.1.100, the host should be 192.168.1.255
    host = System::getIpAddress();
    if( host.rfind( '.' ) != string::npos )
        host.replace( host.rfind( '.' ) + 1, 3, "255" );
    sender.setup( "127.0.0.1", port, false );
    

    mFont = Font( "Arial", 15 );
    mTextureFont = gl::TextureFont::create( mFont );
    
    // handles the size of the keyboard variable
    if (numNotes == 25) startNote = 60;
    startNote = 21;
    if (numNotes == 88) startNote = 0;
    
    for (int i = 0; i < numNotes; ++i)
    {
        noteOns[i] = 0.0f;
        noteOffs[i] = 0.0f;
        pitchBends[i] = 0.0f;
        afterTouch[i] = 0.0f;
        cc[i] = 0.0f;
        
    }
    
    for (int i = 0; i < 16; i++)
    {
        lastChannelNotes[i] = 0;
    }
    
    currentNum = 0;
}



void CinderSeaboardOSCApp::midiListener(midi::Message msg){
    
    int noteNum = msg.pitch;
    
    osc::Message message;
    
    // send the number of the note being modified first
    // so the recieving program knows which parameter to change
    message.addIntArg( (int32_t)noteArrayIndex(noteNum) );
    message.setAddress("/noteNum");
    
    currentNum = noteNum;
    
    switch (msg.status)
    {
        case MIDI_NOTE_ON:
            lastChannelNotes[msg.channel] = noteArrayIndex(noteNum);
            noteOns[noteArrayIndex(noteNum)] = msg.velocity;
            
            message.addIntArg( (int32_t)noteOns[noteArrayIndex(noteNum)] );
            message.setAddress("/noteOn");
            break;
            
        case MIDI_NOTE_OFF:
            noteOffs[noteArrayIndex(noteNum)] = msg.velocity;
            noteOns[noteArrayIndex(noteNum)] = 0.0f;
            afterTouch[noteArrayIndex(noteNum)] = 0.0f;
            pitchBends[noteArrayIndex(noteNum)] = 0.0f;
            
            message.addIntArg( (int32_t)noteOffs[noteArrayIndex(noteNum)] );
            message.setAddress("/noteOff");
            break;
            
        case MIDI_POLY_AFTERTOUCH:
            afterTouch[noteArrayIndex(noteNum)] = msg.value;
            
            message.addIntArg( (int32_t)afterTouch[noteArrayIndex(noteNum)] );
            message.setAddress("/afterTouch");
            break;
            
        case MIDI_PITCH_BEND:
            // find index of latest channel
            pitchBends[ lastChannelNotes[msg.channel] ] = msg.value - 8192;
            
            message.addIntArg( (int32_t)pitchBends[noteArrayIndex(noteNum)] );
            message.setAddress("/pitchBend");
            break;
            
        default:
            break;
    }
    
    // Sends the osc message to the reciever
    message.setRemoteEndpoint(host, port);
    sender.sendMessage(message);
    
}


void CinderSeaboardOSCApp::mouseDown( MouseEvent event )
{
}

void CinderSeaboardOSCApp::update()
{
}

void CinderSeaboardOSCApp::draw()
{
    // clear out the window with black
    gl::clear( Color( 0, 0, 0 ) );
    gl::pushMatrices();
    gl::translate(getWindowCenter());
    
    float angleIncrement = 360.0f/numNotes;
    
    for (int i = 0; i < numNotes; i++)
    {
        float x = 200*sin((i*angleIncrement + pitchBends[i]/683.0f * angleIncrement) * M_PI / 180);
        float y = 200*cos((i*angleIncrement + pitchBends[i]/683.0f * angleIncrement) * M_PI / 180);
        float lx = (200 - 1.5f*afterTouch[i])*sin((i*angleIncrement) * M_PI / 180);
        float ly = (200 - 1.5f*afterTouch[i])*cos((i*angleIncrement) * M_PI / 180);
        
        gl::color(Color(1,1,1));
        gl::drawStrokedCircle(Vec2f(x, y), 5+(noteOns[i]/4));
        gl::drawLine(Vec2f(x,y), Vec2f(lx, ly));
        gl::color(Color(0.0f, noteOns[i], noteOns[i]));
        gl::drawSolidCircle(Vec2f(x, y), 5+(noteOns[i]/4));
        gl::drawSolidCircle(Vec2f(lx, ly), 5);

    }
    
    
    gl::popMatrices();
    
    gl::color( Color::white() );
    mTextureFont->drawString("Sending OSC to port: " + toString(port) + "  current note: " + toString(currentNum), Vec2f(20.0f, 20.0f));
}

int CinderSeaboardOSCApp::noteArrayIndex( int n )
{
    int idx = 0;
    
    if ((n >= 0 && n < startNote) || n > startNote+numNotes-1 || n < 0) idx = 0;
    else idx = n - startNote;
    
    assert (idx < numNotes && idx >= 0);
    
    return idx;
}

CINDER_APP_NATIVE( CinderSeaboardOSCApp, RendererGl )
