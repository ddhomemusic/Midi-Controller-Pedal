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

#include "PedalValue.h"

//*****************************************************************************
//	Constructor
//*****************************************************************************
PedalValue::PedalValue(void)
{
}


//*****************************************************************************
//	Initialise
//*****************************************************************************
void PedalValue::intialise(byte analogPin)
{
	// Set the analog pin to read
	this->analogPin = analogPin;

	// Initialise class variables
	previousMidiValue = 0;
	currentReading = 0;
	newValueReady = false;

	// Calibrate the max value.  This depends on the pedal being set to the
	// high position when the unit is switched on
	maxValue = analogRead(analogPin);
}


//*****************************************************************************
//	Read the value of the potentiometer
//	Convert the reading to a fraction of the max reading
//  Multiply the max midi value 127 by the fraction
//  Compare the current midi value with the previous midi value
//  If the value has changed set the newValueReady flag to true
//*****************************************************************************
void PedalValue::readPedal()
{
	// Get current reading and calculate the midi value
	currentReading = (double)analogRead(analogPin);

	double fractionValue = currentReading / maxValue;
	midiValue = (127 * fractionValue);

	midiValue = (midiValue > 127 ? 127 : midiValue);

	if (midiValue == previousMidiValue)
	{
		newValueReady = false;
	}
	else
	{
		newValueReady = true;
	}
	previousMidiValue = midiValue;
}

//*****************************************************************************
//	Calculate the fraction of the potentiometer rotation
//  Apply this to the maximum midi value of 127
//*****************************************************************************
byte PedalValue::GetMidiValue()
{
	return midiValue;
}

//*****************************************************************************
//	returns true if a new value has been read
//  returns false if the value is the same as previous reading
//*****************************************************************************
boolean PedalValue::isNewValueReady()
{
	return newValueReady;
}


//*****************************************************************************
//	Returns the max pedal volage value, highest is 1023
//*****************************************************************************
int PedalValue::GetMaxValue()
{
	return maxValue;
}