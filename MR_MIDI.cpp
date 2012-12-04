//
//  MR_MIDI.cpp
//  
//
//  Created by Rayce Stipanovich on 11/1/12.
//
//

#include "MR_MIDI.h"

extern MIDI_Class MIDI;
extern bool DISPLAY_MENU;

//initialize our channel to our default channel
short int _MIDI_channel = MR_MIDI_DEFAULT_CHANNEL;
short int _MIDI_velocity_min = MR_MIDI_VELOCITY_DEFAUT_MIN;
short int _MIDI_velocity_max = MR_MIDI_VELOCITY_DEFAUT_MAX;
short int _MIDI_velocity_curve = MR_MIDI_VELOCITY_DEFAUT_CURVE;

/*========================================
 FUNCTION:	InitializeMIDI
 USAGE:		Called to initialize the MIDI communications
			protocol at the standard MIDI baudrate.
========================================*/
void InitializeMIDI(int channel = MR_MIDI_DEFAULT_CHANNEL) {
	//update our MIDI channel if need be
	_MIDI_channel = constrain(channel, 0,16);

	MIDI.begin(_MIDI_channel);

	if (MR_MIDI_DEFAULT_THRU) MIDI.turnThruOn();
	else MIDI.turnThruOff();

	MIDI.setHandleNoteOn(_handler_MIDI_Note_On);
	MIDI.setHandleNoteOff(_handler_MIDI_Note_Off);
	MIDI.setHandleControlChange(_handler_MIDI_CC);
}

void MIDISetChannel(int channel) {
	channel = _MIDI_channel = constrain(channel, 0, 16);
	
	MIDI.setInputChannel(channel);
}

int MIDIGetChannel() {
	return _MIDI_channel;
}

int MIDIVelocityCurve(int rawVelocity) {
	rawVelocity = constrain(rawVelocity, 0, 127);
	
	if (rawVelocity < 64) {
		rawVelocity = map(rawVelocity, 0, 63, _MIDI_velocity_min, _MIDI_velocity_curve - 1);
	} else {
		rawVelocity = map(rawVelocity, 64, 127, _MIDI_velocity_curve, _MIDI_velocity_max);
	}

	return rawVelocity;
}

void MIDIConfigureCurve(int min, int max, int curve) {
	_MIDI_velocity_min = constrain(min, 0, 127);
	_MIDI_velocity_max = constrain(max, 0, 127);
	_MIDI_velocity_curve = constrain(curve, 0, 127);
}

void MIDIBuffer() {
	MIDI.read();
}

void MIDIEnableThru() {
	MIDI.turnThruOn(Full);
}

void MIDIDisableThru() {
	MIDI.turnThruOff();
}

void _handler_MIDI_Note_On(byte channel, byte note, byte velocity) {
	if (int(channel) == _MIDI_channel) {
		MIDIHandleNoteOn(int(note), int(velocity));
	}
}

void _handler_MIDI_Note_Off(byte channel, byte note, byte velocity) {
	if (int(channel) == _MIDI_channel) {
		MIDIHandleNoteOff(int(note), MIDIVelocityCurve(int(velocity)));
	}
}

void _handler_MIDI_CC(byte channel, byte number, byte value) {
	if (int(channel) == _MIDI_channel) {
		MIDIHandleControlChange(int(number), int(value));
	}
}

void MIDIHandleNoteOn(int note, int rawVelocity) {
	unsigned int scaledVelocity = MIDIVelocityCurve(rawVelocity);
	char notechar[4];
	unsigned short int notenumber = note % 12;
	short octave = (note / 12)-2;
	switch(notenumber) {
		case 0:
			strcpy(notechar,"  C ");
			break;
		case 1:
			strcpy(notechar,"  C#");
			break;
		case 2:
			strcpy(notechar,"  D ");
			break;
		case 3:
			strcpy(notechar,"  D#");
			break;
		case 4:
			strcpy(notechar,"  E ");
			break;
		case 5:
			strcpy(notechar,"  F ");
			break;
		case 6:
			strcpy(notechar,"  F#");
			break;
		case 7:
			strcpy(notechar,"  G ");
			break;
		case 8:
			strcpy(notechar,"  G#");
			break;
		case 9:
			strcpy(notechar,"  A ");
			break;
		case 10:
			strcpy(notechar,"  A#");
			break;
		case 11:
			strcpy(notechar,"  B ");
			break;
	}
	digitalWrite(MR_LED_1_PIN, HIGH);

	
	//update the buffer
	char tempbuffer[6];
	strcpy(tempbuffer, notechar);
	char vel[4];
	itoa(octave, vel, 10);
	strcat(tempbuffer, vel);
	
	//update globals
	strcpy(MIDI_NOTE_STRING, tempbuffer);
	
	MIDI_NOTE_VELOCITY_VAL = rawVelocity;

	blinkLED(1);

	//handle MIDI cases

	if (notenumber == 0 && octave == 1) {
		DISPLAY_MENU = !DISPLAY_MENU;
	}
}

void MIDIHandleNoteOff(int note, int scaledVelocity) {
	
}

void MIDIHandleControlChange(int cc, int value) {
	//update the buffer
	char tempbuffer[6];
	strcpy(tempbuffer, "CC");
	char vel[4];
	itoa(cc, vel, 10);
	strcat(tempbuffer, vel);

	//update globals
	strcpy(MIDI_NOTE_STRING, tempbuffer);

	MIDI_NOTE_VELOCITY_VAL = value;

	blinkLED(2);

	if (cc==22) updateStepper(value);
}

