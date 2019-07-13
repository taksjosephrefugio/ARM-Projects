/*
This is program beeps on and off a buzzer
when a motion is detected similar to a burglar
alarm system. 

Microcontroller used: 
ARMv7 Cortex M
Texas Instruments
TM4C123GH6PM

Components used:
PIR Sensor
Piezo Buzzer

Code Written by: Prof. Zhao Zhang, UIC, CS Department
Code Improved by: TAK'S JOSEPH REFUGIO
Code Written for: 7-Segment Lab for Embedded Systems Class
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inc/hw_memmap.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include "launchpad.h"
#include "buzzer.h"
#include "motion.h"

// Switch type
typedef enum {
    Off, On
} switch_t;

// The states of buzz playing: the state of the buzzer system, the state of sound, the state of the
// buzzer, and time left in the current sound state in millisecond
static switch_t buzzerSysState = Off;
static switch_t soundState = Off;
static switch_t buzzerState = Off;
static uint16_t timeLeft = 0;
static switch_t sysState = Off;

/*
 * Buzzer play callback function. It plays a buzz sound for 0.3 second, then be silent for 3 seconds.
 * In the buzz phase, the buzzer is turned on and off every 2 ms.  In the silent phase, the buzzer is turned
 * off all the time. The buzzer itself has a built-in resonant frequency of about 2300Hz (+/- 300Hz) when
 * turned on.
 *
 * The function also checks buzzerSysState. If the state is off, then it turns of the buzzer and
 * reset all the states.
 */
void
buzzerPlay(uint32_t time)
{
    uint32_t delay = 1;

    if (buzzerSysState == On) {
        switch (soundState) {
        case Off:
            if (timeLeft < delay) {              // switch to sound-on state
                buzzerOn();
                uprintf("%s\n\r", "ON"); // Debug
                soundState = On;
                buzzerState = On;
                timeLeft = 300;
            }
            else {
                timeLeft -= delay;
            }
            break;

        case On:
            if (timeLeft < delay) {              // switch to sound-off state
                buzzerOff();
                uprintf("%s\n\r", "OFF"); // Debug
                soundState = Off;
                buzzerState = Off;
                timeLeft = 500;
            }
            else {
                timeLeft -= delay;
                switch (buzzerState) {           // if buzzer is on, turn it off
                case Off:
                    buzzerOn();
                    buzzerState = On;
                    break;

                case On:                        // if buzzer is off, turn it on
                    buzzerOff();
                    buzzerState = Off;
                    break;
                }
            }
        }
    }
    else {                                      // turn the buzzer system off
        buzzerOff();
        soundState = Off;
        buzzerState = Off;
        timeLeft = 0;
    }

    schdCallback(buzzerPlay, time + delay);    // call back with the delay
}

/*
 * Event driven code for checking push button
 */
void
checkPushButton(uint32_t time)
{
    int code = pbRead();
    uint32_t delay;
    switch (code) {
    case 1:
                                       // SW1: Start the buzzer
       if (sysState == Off) {
           sysState = On;
           delay = 250;
            uprintf("%s\n\r","it's on");
       }
        break;

    case 2:                                     // SW2: Stop the buzzer
        if (sysState == On) {
           sysState = Off;
           delay = 250;
            uprintf("%s\n\r","it's off");
        }
        break;

    default:
        delay = 10;
    }

    schdCallback(checkPushButton, time + delay);
}

void
checkMotion(uint32_t time)
{
    uint32_t delay;
    int motion = motionRead();


    if(sysState == On){
        if(motion == 0){

            buzzerSysState = Off;

            delay = 250;
        }
        else{
            uprintf("%d\n\r",motion);
            buzzerSysState = On;

            delay = 250;
        }
    }
    else{
        buzzerSysState = Off;
        delay = 10;
    }

    schdCallback(checkMotion,time + delay);

}

int main(void)
{
    lpInit();
    buzzerInit();
    motionInit();

    uprintf("%s\n\r", "Lab 4 starts");

    schdCallback(checkPushButton, 1000);
    schdCallback(buzzerPlay, 1005);
    schdCallback(checkMotion, 1007);
    while (true) {
        schdExecute();

    }
}
