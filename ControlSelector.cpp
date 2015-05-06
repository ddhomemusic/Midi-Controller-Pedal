/// *********************************************************************************
// ControlSelector
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
// Allows the user to select a midi controller number
//
// History:
// 
// *********************************************************************************
#include "ControlSelector.h"


//*****************************************************************************
//	Constructor
//*****************************************************************************
ControlSelector::ControlSelector(void)
{
}

//*****************************************************************************
//	initialise
//*****************************************************************************
void ControlSelector::initialise()
{

	// Define an array of controller names
	sprintf(controlNames[0], "General Cntrl");
	sprintf(controlNames[1], "General Cntrl");
	sprintf(controlNames[2], "General Cntrl");
	sprintf(controlNames[3], "General Cntrl");
	sprintf(controlNames[4], "General Cntrl");


	// Define an index that links controller values to the to the appropriate controller name
	controlCodeArray[0] = 16;
	controlCodeArray[1] = 17;
	controlCodeArray[2] = 18;
	controlCodeArray[3] = 19;
	controlCodeArray[4] = 20;

	// read controller index from eeprom and use this index with the controlCodeArray
	// to get the appropriate controller name
	controllerIndex = EEPROM.read(EPROM_ADDRESS);
	ControllerCode = controlCodeArray[controllerIndex];
	sprintf(ControllerName, "%2d %s", ControllerCode, controlNames[controllerIndex]);
}

//*****************************************************************************
//	select the controller
//*****************************************************************************
void ControlSelector::selectController(DButton *button)
{
	if(!button->IsButtonUsed)
	{
		button->buttonUsed();

		// Loop round all the controllers
		if(controllerIndex < NUM_CONTROLLERS -1)
		{
			controllerIndex++;
		}
		else
		{
			controllerIndex = 0;
		}
		ControllerCode = controlCodeArray[controllerIndex];
		sprintf(ControllerName, "%2d %s", ControllerCode, controlNames[controllerIndex]);

		// Save the controller index in the eeprom
		EEPROM.write(EPROM_ADDRESS, controllerIndex);	
	}
}



