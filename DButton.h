/// *********************************************************************************
// DButton
// *********************************************************************************
// (c) D K Dickens
// Date created: 21/5/2014
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
// Detects if a button has been pressed.  
// Debounces by testing the button over 3 consecutive reads
//
// History:
// 
// *********************************************************************************
#pragma once

#include <Arduino.h>

#define DEBOUNCE_MAX 10
#define OFF 0
#define ON 1



class DButton
{

private:
	byte _pin;					// Input pin number
	byte _count;				// Debounce counter


	byte _buttonState;			// Either ON or OFF
	


public:

	// Properties
	byte IsButtonUsed;		// Either USED or UNUSED

	// Functions
	DButton(void);

	void initialise(byte pinNumber);
	void readButton();
	boolean isButtonOn();
	void buttonUsed();

};

