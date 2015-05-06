/// *********************************************************************************
// PedalValue
// *********************************************************************************
// (c) D K Dickens
// Date created: 19/5/2014
//
// ----------------------------------------------------------------
// This code was created for experimentation with the Arduino.  
// It is free to use and to modify for your own needs.
//
// ----------------------------------------------------------------
// License:
// This library is distributed under the GNU GENERAL PUBLIC LICENSE
// Version 3, 29 June 2007.
//
// http://www.gnu.org/licenses/gpl.html
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
//
// See the GNU General Public License for more details.
//
//---------------------------------------------------------------
//
// Description:
// Reads the value of the potentiometer attached to the pedal and converts to
// a midi value.
//
// History:
// 
// *********************************************************************************

#pragma once

#include <arduino.h>

class PedalValue
{
private: 
	byte previousMidiValue;
	double currentReading;
	double maxValue;
	byte midiValue;
	byte analogPin;
	boolean newValueReady;


public:
	PedalValue(void);
	~PedalValue(void);

	void intialise(byte analogPin);
	boolean isNewValueReady();
	void readPedal();
	byte GetMidiValue();
	int GetMaxValue();

};

