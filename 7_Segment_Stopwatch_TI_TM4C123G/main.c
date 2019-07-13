/*
This program displays a running clock that could
be reset and/or incremented using pushbuttons. 
The 7-segment could also be used as a stopwatch 
simultaneously with the running clock. 

Microcontroller used: 
ARMv7 Cortex M
Texas Instruments
TM4C123GH6PM

Component used:
7-Segment

Code Written by: Prof. Zhao Zhang, UIC, CS Department
Code Improved by: TAK'S JOSEPH REFUGIO
Code Written for: Microcontroller Clock Lab for Embedded Systems Class
*/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "launchpad.h"
#include "seg7.h"
#include "seg7digit.h"


// Stop watch increment function, written in assembly
extern void
stopWatchIncrement(seg7Display_t* seg7Display);

// The running state of the stopwatch system
enum {
    Reset, Run, Pause
}  sysState = Reset;

// The initial state of the 7-segment display: "00:00" with colon on
seg7Display_t seg7Display = {
    0, 0, 0, 0, 1
};

// Callback function for updating clock watch
void stopWatchUpdate(uint32_t time)                          // The scheduled time
{
    // Update clock and display, only if the stopwatch is running
    if (sysState == Run) {
        stopWatchIncrement(&seg7Display);
    }

    seg7DigitUpdate(&seg7Display);

    // Call back after 10 milliseconds
    schdCallback(stopWatchUpdate, time + 10);
}

// Callback function for checking push button. SW1 is the RESET button, and
// SW2 is the START/PAUSE/RESUME button.1
void checkPushButton(uint32_t time)
{
    int code = pbRead();
    uint32_t delay;

    switch (code) {
    case 1:									// SW1 is the Reset button, only when the stopwatch is paused
		uprintf("SW1 has been pressed.\n\r");
        delay = 250;						// software debouncing

		// Toggles between Pause and Run when Switch 1 is pressed 
		static int SW1_counter;
		SW1_counter++;
		switch (SW1_counter % 2) {
		case 0:
			sysState = Pause;
			break;
		case 1:
			sysState = Run;
			break;
		}

        break;
    case 2:										// SW2 is the Start/Pause/Resume button
		uprintf("SW2 has been pressed.\n\r");
        delay = 250;							// software debouncing

		// Resets 7-segment display when system is Paused
		if (sysState == Pause)
		{
			seg7Display.d1 = 0;
			seg7Display.d2 = 0;
			seg7Display.d3 = 0;
			seg7Display.d4 = 0;
			seg7Display.colon = 1;
		}
        break;
    default:
        delay = 10;
    }
    schdCallback(checkPushButton, time + delay);
}

int main(void)
{
    lpInit();
    seg7Init();

    uprintf("%s\n\r", "Lab 3: Stopwatch");

	sysState = Pause;	// Initial state of the clock

    // Update the clock display
    seg7DigitUpdate(&seg7Display);

    // Schedule the first callback events for LED flashing and push button checking.
    // Those trigger callback chains. The time unit is millisecond.
    schdCallback(stopWatchUpdate, 1000);
    schdCallback(checkPushButton, 1005);

    // Run the event scheduler to process callback events
    while (true) {
        schdExecute();
    }
}
