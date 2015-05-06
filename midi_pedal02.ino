/// *********************************************************************************
// midi_pedal version 2
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
// a midi value.  Allows the user to select different controller values
//
// History: 
// 
// *********************************************************************************


#include <EEPROM.h>
#include <LiquidCrystal.h>
#include "PedalValue.h"
#include "DButton.h"
#include "ControlSelector.h"

//*****************************************************************************
//	Defines
//*****************************************************************************
#define ANALOG_PIN 0			// Pin for reading potentiometer
#define BUTTON_PIN 9			// Pin for controller selection button
#define MODE_BUTTON 10		// Pin for mode selection button
#define TOP_ROW 0				// LCD row
#define BOTTOM_ROW 1			// LCD row
#define LINEAR 0				// Linear mode
#define LFO 1					// LFO mode
#define LFO_MULTIPLIER 20		// Multiplies the midi pedal value to get a MS delay
#define UP 0					// LFO direction
#define DOWN 1					// LFO direction
#define ONE_MINUTE_MS 60000		// 1 minute in milliseconds

#define LFO_TRIANGLE 1			// Triangle wave mode
#define LFO_SAW_UP	 2			// Slope up sawtooth
#define LFO_SAW_DOWN 3			// Slope down sawtooth
#define LFO_SQUARE   4			// Square wave

//*****************************************************************************
//	Globals
//*****************************************************************************

// initialize the LCD library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Class to manage the controller selection
ControlSelector *controlSelector;

// Class to handle reading the pedal values
PedalValue *pedalValue;

// Used for selecting midi control code
DButton *selectButton;

// Used for selecting between linear and LFO modes
DButton *modeButton;

// Used for building messages on the LCD
char text[17];

// Holds the midi value
byte midiValue;

// The midi controller code
byte midiController = 16;

// Midi status code for controller on channel 1
byte midiStatus = 0xB0;

// Mode LFO or LINEAR
byte mode;

// LFO midi value
byte lfoValue;
byte lfoSendValue;	// this is the value that actually gets sent by midi

// LFO mode
byte lfoMode;

// LFO direction
byte lfoDirection;

// Timer counter for LFO operation in microseconds
unsigned long prevTimeMS;		// records previous time in microseconds
unsigned long lfoDelayMS;		// Delay between LFO writes in microseconds
unsigned long currentMS;		// Current time in microseconds
unsigned long rateTimeMS;		// LFO rate timer in milliseconds
unsigned long ratePrevTimeMS;	// Previous time in milliseconds
unsigned long lfoRatePerMin;	// LFO cycles per minute


//*****************************************************************************
//	Set up
//*****************************************************************************
void setup() {


	// Allocate space for classes
	pedalValue = (PedalValue*)malloc(sizeof(PedalValue));
	selectButton = (DButton*)malloc(sizeof(DButton));
	modeButton = (DButton*)malloc(sizeof(DButton));
	controlSelector = (ControlSelector*)malloc(sizeof(ControlSelector));

	selectButton->initialise(BUTTON_PIN);
	modeButton->initialise(MODE_BUTTON);
	controlSelector->initialise();

	//  Set MIDI baud rate:
	Serial.begin(115200);

	// set up the LCD's number of columns and rows: 
	lcd.begin(16, 2);

	// Print a message to the LCD.
	lcd.print("Set pedal to max");

	// Short delay to allow pedal to be calibrated
	delay(1000);

	// Initialise the pedal and calibrate the max value
	pedalValue->intialise(ANALOG_PIN);

	// Display max value read from pot
	lcd.setCursor(0, BOTTOM_ROW);
	lcd.print(pedalValue->GetMaxValue());


	// Short delay to see calibrated value
	delay(1000);

	lcd.clear();

	// Setup the LFO params
	prevTimeMS = micros();
	lfoValue = 0;
	lfoDirection = UP;
	lfoMode = LFO_TRIANGLE;

	// Set the starting midi value
	midiValue = 0;

	// Set the starting value for calculating the LFO rate per min
	ratePrevTimeMS = millis();
	lfoRatePerMin = 0;

	// Display the controls depending on the mode
	if( mode == LFO)
	{
		displayLFOWave();
	}
	else
	{
		displayControllerCode();
	}

}


//*****************************************************************************
//	Main processing loop
//*****************************************************************************
void loop() 
{

	// Process the mode button
	processModeButton();

	// Process select button
	processSelectButton();

	// Read the pedal value
	pedalValue->readPedal();

	// If the pedal has changed then grab the midi value from the pedal
	// If the mode is LINEAR then send this value to the Midi instrument
	// If the mode is LFO use the midi value to calculate a time delay in microseconds
	// this is used to control the speed of the LFO.
	if(pedalValue->isNewValueReady())
	{
		midiValue = pedalValue->GetMidiValue();

		switch(mode)
		{

		case LINEAR:
			// Send a linear midi message
			sendMidiControl(midiStatus, controlSelector->ControllerCode, midiValue);

			// Update display
			displayMidiValue();

			// reset the LFO params
			prevTimeMS = millis();
			lfoValue = 0;
			lfoDirection = UP;
			delay(5);
			break;

		case LFO:
			// Calculate the delay between midi control value sends
			// Note the saw waves only run for half the cycle so double the delay time for these
			lfoDelayMS = (127 - midiValue) * (lfoMode == LFO_SAW_UP || lfoMode == LFO_SAW_DOWN ? LFO_MULTIPLIER * 2: LFO_MULTIPLIER);
		}
	}

	// If the mode is LFO then send the midi messsage and calculate the time for the next send
	if(mode == LFO)
	{
		// Get the currrent time in microseconds
		currentMS = micros();

		// If the previous time plus the delay is now smaller than the current time
		// send a midi message
		 
		if(prevTimeMS + lfoDelayMS <= currentMS)
		{
			// Calculate the next midi value depending on the type of wave
			switch(lfoMode)
			{

				// * * * * * TRIANGLE WAVE * * * * *
			case LFO_TRIANGLE:

				if(lfoDirection == UP)
				{
					if(lfoValue < 127)
					{ 
						lfoValue++;
					}
					else
					{ 
						lfoDirection = DOWN;
					}
				}
				else if(lfoDirection == DOWN)
				{
					if(lfoValue > 0) 
					{	
						lfoValue--;	
					}
					else
					{
						lfoDirection = UP;

						// Calc and display the cycles per minute at the beginning of the cycle
						calculateCyclesPerMinute();
					}
				}
				lfoSendValue = lfoValue;
				break;

				// * * * * * SAW UP WAVE * * * * *
			case LFO_SAW_UP:

				if(lfoValue < 127)
				{ 
					lfoValue++;
				}
				else
				{ 
					lfoValue = 0;

					// Calc and display the cycles per minute at the beginning of the cycle
					calculateCyclesPerMinute();
				}
				lfoSendValue = lfoValue;
				break;

				// * * * * * SAW DOWN WAVE * * * * *
			case LFO_SAW_DOWN:

				if(lfoValue > 0)
				{ 
					lfoValue--;
				}
				else
				{ 
					lfoValue = 127;

					// Calc and display the cycles per minute at the beginning of the cycle
					calculateCyclesPerMinute();
				}
				lfoSendValue = lfoValue;
				break;

				// * * * * * SQUARE WAVE * * * * *
			case LFO_SQUARE:
				if(lfoDirection == UP)
				{
					if(lfoValue < 127)
					{ 
						lfoValue++;
					}
					else
					{ 
						lfoDirection = DOWN;
					}
				}
				else if(lfoDirection == DOWN)
				{
					if(lfoValue > 0) {	lfoValue--;	}
					else
					{
						lfoDirection = UP;

						// Calc and display the cycles per minute at the beginning of the cycle
						calculateCyclesPerMinute();
					}
				}
				if(lfoValue < 64)
				{
					lfoSendValue = 0;
				}
				else 
				{
					lfoSendValue = 127;
				}
				break;

			} // End of switch case

			sendMidiControl(midiStatus, controlSelector->ControllerCode, lfoSendValue);

			// Set this as the previous value
			prevTimeMS = currentMS;
		}
	}
}



//*****************************************************************************
//	Process the mode button
//*****************************************************************************
void processModeButton()
{

	// Read the mode button
	modeButton->readButton();

	// If the mode button is selected, check the mode
	if( modeButton->isButtonOn())
	{
		// If in LINEAR mode switch to LFO mode
		if(mode == LINEAR)
		{
			mode = LFO;
			modeButton->buttonUsed();
			displayLFOWave();
		}
	}

	// Switch from LFO to linear mode
	if(mode == LFO && !modeButton->isButtonOn())
	{
		mode = LINEAR;
		displayControllerCode();
	}
}


//*****************************************************************************
//	Process the select button
//*****************************************************************************
void processSelectButton()
{
	// Read the selector button
	selectButton->readButton();

	// If the select button is pressed check to see what needs to be changed
	if(selectButton->isButtonOn())
	{
		switch (mode)
		{
		case LINEAR:
			controlSelector->selectController(selectButton);
			displayControllerCode();
			break;

		case LFO:
			if(selectButton->isButtonOn() && !selectButton->IsButtonUsed)
			{
				selectButton->buttonUsed();

				if(lfoMode < LFO_SQUARE)
				{
					lfoMode++;
				}
				else
				{
					lfoMode = LFO_TRIANGLE;
				}
			}

			// Put the appropriate text on the screen
			displayLFOWave();
			delay(50);

			break;
		}
	}
}


//*****************************************************************************
//	Calculate and display the cycles per minute
//*****************************************************************************
void calculateCyclesPerMinute()
{
	rateTimeMS = millis() - ratePrevTimeMS;
	lfoRatePerMin = ONE_MINUTE_MS / rateTimeMS;

	//sprintf(text,"%3d Per Min ",lfoRatePerMin);
	sprintf(text,"%d MS Per/Cyc ", rateTimeMS);

	// Update display
	displayTextValue(text);
	ratePrevTimeMS = millis();
}

//*****************************************************************************
//	Display the text value
//*****************************************************************************
void displayTextValue(char *txt)
{
	lcd.setCursor(0, BOTTOM_ROW);
	lcd.print("   ");
	lcd.setCursor(0, BOTTOM_ROW);
	lcd.print(txt);
}


//*****************************************************************************
//	Display the midi value
//*****************************************************************************
void displayMidiValue()
{
	lcd.setCursor(0, BOTTOM_ROW);
	lcd.print("   ");
	lcd.setCursor(0, BOTTOM_ROW);
	lcd.print(midiValue);
}


//*****************************************************************************
//	Display selected LFO wave
//*****************************************************************************
void displayLFOWave()
{
	switch(lfoMode)
	{
	case LFO_TRIANGLE:
		sprintf(text,"LFO Triangle    ");
		break;
	case LFO_SAW_UP:
		sprintf(text,"LFO Saw Up      ");
		break;
	case LFO_SAW_DOWN:
		sprintf(text,"LFO Saw Down    ");
		break;
	case LFO_SQUARE:
		sprintf(text,"LFO Square      ");
		break;
	}
	lcd.setCursor(0, TOP_ROW);
	lcd.print(text);
}


//*****************************************************************************
//	Display controller code
//*****************************************************************************
void displayControllerCode()
{
	lcd.setCursor(0, TOP_ROW);
	lcd.print(controlSelector->ControllerName);
	lcd.setCursor(0, BOTTOM_ROW);
	lcd.print("                ");
}


//*****************************************************************************
//	Sends a midi control messsage
//*****************************************************************************
void sendMidiControl(byte status, byte control, byte value) 
{
	Serial.write(status);
	Serial.write(control);
	Serial.write(value);
}

