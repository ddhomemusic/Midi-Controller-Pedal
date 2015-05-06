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
#pragma once

#ifndef ControlSelector_H
#define ControlSelector_H

#include <Arduino.h>
#include <EEPROM.h>
#include <stdio.h>
#include "DButton.h";


#define EPROM_ADDRESS 0
#define NUM_CONTROLLERS 5


						
class ControlSelector
{

private:
	byte controllerIndex;
	// Define an array of controller names
	char controlNames[NUM_CONTROLLERS][17];

	// Define an index that links controller values to the to the appropriate controller name
	byte controlCodeArray[NUM_CONTROLLERS];

public:
	char ControllerName[17];
	byte ControllerCode;

	ControlSelector(void);
	void initialise();
	void selectController(DButton *button);


};

#endif
