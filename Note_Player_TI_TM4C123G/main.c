/*
This program takes a string of characters that corresponds
to the major chromatic scale in music then sends those notes
to be played on a piezo buzzer. Spaces in the string corresponds
to rests. The rests and notes played at the same clock
frequency as indicated in the program. 

Components used:
Piezo buzzer

Code Written by: Prof. Zhao Zhang, UIC, CS Department
Code Improved by: TAK'S JOSEPH REFUGIO
Code Written for: Frequency and Clocks Lab for Embedded Systems Class
 */

#include <pwmled.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "launchpad.h"
#include "pwmbuzzer.h"

static enum {Run, Pause} sysState = Run;
static enum {Play, Stop} mplayer = Stop;
char melody[] = "cddccdddcceeccddc cddccd ceeccddc";
int notes[] = {440,494,262,294,330,349,392}; // note frequencies - a,b,c,d,e,f,g
double period_clk = 0;  // sampled frequency initialized
/*
 * Check the push botton. SW1 is the RUN key, SW2 is the PAUSE key
 */
int volume = 1;
int i = 0;
void
checkPushButton(uint32_t time)
{
	int code = pbRead();
	uint32_t delay = 10;

	switch (code) {
	case 1:				// SW1
		
	    switch (sysState) {
	    case Run:
	        sysState = Pause;
	        break;

	    case Pause:
	        sysState = Run;
	        break;
	    }
		delay = 250;
		break;

	case 2:				// SW2
	    switch (mplayer) {  // music playe toggle
	    case Play:
	        mplayer = Stop;
	        break;

	    case Stop:
	        mplayer = Play;
	        break;
	    }
		delay = 250;
		break;
	default:
		delay = 10;
	}

	schdCallback(checkPushButton, time + delay);
}

/*
 * Change LED intensity level.
 */
void
pwmLedPlay(uint32_t time)
{
    static int intensityLevel = 1;
    static enum {Up, Down} direction = Up;

    // Pre-schedule the next callback with 70 ms delay
    schdCallback(pwmLedPlay, time + 70);

    // Return immediately if system is paused
    if (sysState == Pause)
        return;

    // Set intensity level
    pwmLedSetIntensity(intensityLevel);

    // Update for next intensity level
    switch (direction) {
    case Up:
        // Increment intensity level, switch direction to down if level 10 is reached
        if (++intensityLevel == 10) {
            direction = Down;
        }
        break;

    case Down:
        // Decrement intensity level, switch direction to down if level 1 is reached
        if (--intensityLevel == 1) {
            direction = Up;
        }
        break;
    }
}

void pwmMusicPlay(uint32_t time){
		
	uint32_t delay = 250;
	if(mplayer == Play){
		if(melody[i] == ' '){
			pwmBuzzerPlay(100000,1); // Simple Pause
		}else{
			period_clk = 80000000.0/notes[melody[i]-97];		// Resampling the period with the MCU's
			pwmBuzzerPlay(period_clk,(period_clk*volume)/100); // Creating a Pwm Signal
		}
		i++;
		if(i > strlen(melody) - 1){ // Loop music.
			i = 0;
		}
	}
	else{
		pwmBuzzerPlay(100000,1); // Simple Pause
	}
	schdCallback(pwmMusicPlay,time + delay);
}


void main(void)


{
	lpInit();
	pwmLedInit();
	pwmBuzzerInit();

	// Schedule the first callback events for LED flashing and push button checking.
	// Those trigger callback chains. The time unit is millisecond.
	schdCallback(checkPushButton, 1005);
	schdCallback(pwmLedPlay, 1010);
	schdCallback(pwmMusicPlay,1015);

	// Loop forever
	while (true) {
		schdExecute();
	}
}
