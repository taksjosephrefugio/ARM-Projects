/*
The microcontroller has a presaved user password. Then the
user is given a 4x3 keypad for navigation such as entering 
the password, clearing the typed numbers, etc. 

If the password entered is good, the microcontroller will 
output "Good" on the 7-segment which tells the user that
he/she has access to the bank. Otherwise, the 7-segment will 
indicate "Deny" that means no access to the bank. If the user
is denied because of wrong typed password, the piezo buzzer 
will ring.

Components used:
Keypad
7-segment
buzzer

Code Written by: TAK'S JOSEPH REFUGIO, MOHAMMED ARSHAD
Code Written on: Fall 2017
Code Written for: Project for UIC Embedded Systems Class
*/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <driverlib/sysctl.h>
#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <inc/hw_i2c.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/i2c.h>
#include "launchpad.h"

#include "keypad.h"
#include "led.h"
#include "seg7.h"
#include "seg7digit.h"
#include "buzzer.h"

// Stores user input to array
int userInput[4] = {0, 0, 0, 0};
int password[4] = {2, 0, 1, 7};

// Stores last number pressed on keypad
int lastKeyPressed = -1;
int keyPressCtr = 0;
int keyPressCtrLR = 0;
int configFlag = 0;

// Initial State of Launchpad LED
bool rState = false;
bool bState = false;
bool gState = false;

// Struct for State of Passcode Acceptance
typedef enum { Good, Deny, Reset } AccountState;
AccountState acceptState = Reset;
static enum	{Unlocked, Locked}	LockState = Locked;

// The initial state of the 7-segment display: last number is for colon display
seg7Display_t seg7Display = { 10, 10, 10, 10, 0 };

// Toggles the locking state
void toggleState(){
	switch(LockState){
		case 	Unlocked:
			uprintf("Unlocked to Locked\r\n ");
			LockState = Locked;
			break;
		case 	Locked:	
			uprintf("Locked to Unlocked\r\n ");
			LockState = Unlocked;
			break;
	}
	return;
}
// Checking passcode
void checkPasscode(int passcodeArray[4], int inputArray[4]) {	
	int i;
	for(i = 0; i < 4; i ++){
		if(!(passcodeArray[i] == inputArray[i])){
			acceptState = Deny;
			return;
		}
	}
	uprintf("Unlocked\n\r");
	acceptState = Good;
	ledGreenOn();
	ledRedOff();
	toggleState();
	
	return;
}
// Event-driven code for display to the 7-segment
void displayUpdate(uint32_t time) {
	// Delay
	int delay = 103;

	// Updating 7-segment; case 4 resets keyPressCtr and sets higher delay
	switch(keyPressCtr) {
	case 0:
		seg7Display.d4 = 10;
		seg7Display.d3 = 10;
		seg7Display.d2 = 10;
		seg7Display.d1 = 10;
		break;
	case 1: // Leftmost
		userInput[0] = lastKeyPressed;
		seg7Display.d4 = lastKeyPressed;
		seg7Display.d3 = 10;
		seg7Display.d2 = 10;
		seg7Display.d1 = 10;
		break;
	case 2: 
		userInput[1] = lastKeyPressed;
		seg7Display.d3 = lastKeyPressed;
		seg7Display.d2 = 10;
		seg7Display.d1 = 10;
		break;
	case 3: 
		userInput[2] = lastKeyPressed;
		seg7Display.d2 = lastKeyPressed;
		seg7Display.d1 = 10;
		break;
	case 4: // Rightmost
		userInput[3] = lastKeyPressed;
		seg7Display.d1 = lastKeyPressed;
		keyPressCtr = 0;
		checkPasscode(password,userInput);
		delay = 500;
		break;
	}

	uprintf("%d %d %d %d\n\r", userInput[0], userInput[1], userInput[2], userInput[3]);
	seg7DigitUpdate(&seg7Display);
	schdCallback(displayUpdate, time + delay);
}
// Updates lastKeyPressed
void checkKeypad(uint32_t time) {
	// Counter for this function only
	int ctr;

	// Delay
	int delay = 100;

	if (keypadRead() != -1) {
		// When pressing '*'
		if (keypadRead() == 42) {
			LockState == Locked;
			uprintf("State is locked.\n\r");
			ledConfig(Deny);

			delay = 250;
		}
		// When pressing '#'
		else if (keypadRead() == 66) {
			ledConfig(Reset);
			
			// Clear Display 
			seg7Display.d4 = 10;
			seg7Display.d3 = 10;
			seg7Display.d2 = 10;
			seg7Display.d1 = 10;

			
			// Clear userInput
			for (ctr = 0; ctr < 4; ctr++) {
				userInput[ctr] = 0;
			}

			delay = 250;
			configFlag = 1;
		}
		else {
			// Read key press
			lastKeyPressed = keypadRead();

			// Increment keyPressCtr
			keyPressCtr++;
			delay = 250;
		}
	}

	if (configFlag) {
		// Store user input as a digit in new password
		password[keyPressCtr] = lastKeyPressed;
	}

	// Run again after delay
	schdCallback(checkKeypad, time + delay);
}

//LED On/Off Configuration
void ledConfig(AccountState code){
	switch(code)
	{
	case Good:
		ledGreenOn();
		ledRedOff();
		break;
	case Deny:
		ledGreenOff();
		ledRedOn();
		break;
	case Reset:
		ledGreenOn();
		ledRedOn();
		break;
	}
}

// Main
int main(void) {
	// Initializations
	lpInit();
	keypadInit();
	seg7Init();
	ledInit();
	buzzerInit();
	ledConfig(Deny);

	// Callback Events
	schdCallback(checkKeypad, 1000);
	schdCallback(displayUpdate, 1003);


	// Loop forever
	while (true) {
		schdExecute();
		uprintf("%d %d %d %d\n\r", password[0], password[1], password[2], password[3]);
	}
}
