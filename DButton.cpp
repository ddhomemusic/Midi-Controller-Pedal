/// *********************************************************************************
// DButton
// *********************************************************************************
// (c) D K Dickens
// Date created: 21/5/2014
// Description:
//
// The button has two states, On & OFF.
// It can only be switched between these states if a debounced
// reading confirms a change.
//
// The button also has a IsButtonUsed status.  This can be used by
// the calling code to avoid an ON button being continually used.
// So if the user keeps his/her finger on the switch the On value
// will only be used once.
//
// Detects if a button has been pressed.  
// Debounces by testing the button over 3 consecutive reads
// The code using the button must call the buttonUsed() method
// it can then test the IsButtonUsed flag to avoid the button's
// on state being continually used.
//
// History:
// 
// *********************************************************************************
#include "DButton.h"

//*****************************************************************************
//	constructor
//*****************************************************************************
DButton::DButton(void)
{
}


//*****************************************************************************
//	initialise
//*****************************************************************************
void DButton::initialise(byte pinNumber)
{
	_pin = pinNumber;
	pinMode(_pin, INPUT_PULLUP);
	_buttonState = OFF;
	IsButtonUsed = false;
	_count = 0;
}

//*****************************************************************************
//	readButton
//  If the button is high and the previous state was high, increment the 
//  counter.  When the counter reaches maximum then set the set flag to true.
//*****************************************************************************
void DButton::readButton()
{
	int reading = digitalRead(_pin);

	// if the button is currently off and the pin reading is high
	// then take DEBOUNCE_MAX readings.  If all readings are high
	// then switch the button on
	if(_buttonState == OFF && reading == LOW)
	{
		if(_count < DEBOUNCE_MAX)
		{
			_count++;
		}
		else
		{
			_count = 0;
			_buttonState = ON;
			IsButtonUsed = false;
		}
	}
	else if(_buttonState == ON && reading == HIGH)
	{
		// If the button is currently ON and the reading is low
		// then take DEBOUNCE_MAX readings.  If they are all low
		// switch the button off
		if(_count < DEBOUNCE_MAX)
		{
			_count++;
		}
		else
		{
			_count = 0;
			_buttonState = OFF;
			IsButtonUsed = false;
		}
	}
	else
	{
		// Reset the counter
		_count = 0;
	}
}


//*****************************************************************************
//	isButtonSet
//  Returns true if the button is set
//*****************************************************************************
boolean DButton::isButtonOn()
{
	return _buttonState == ON;
}


//*****************************************************************************
//	buttonUsed
//	This must be called by the code using the button to indicate that the
//	press has been read and to reset the debounce mechanism
//*****************************************************************************
void DButton::buttonUsed()
{
	IsButtonUsed = true;
}