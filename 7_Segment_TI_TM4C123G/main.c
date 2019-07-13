/*
This program displays a running clock that could
be reset and/or incremented using pushbuttons

Microcontroller used: 
ARMv7 Cortex M
Texas Instruments
TM4C123GH6PM

Components used:
7-Segment

Code Written by: Prof. Zhao Zhang, UIC, CS Department
Code Improved by: TAK'S JOSEPH REFUGIO
Code Written for: 7-Segment Lab for Embedded Systems Class
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
#include "seg7.h"

// 7-segment coding table. See https://en.wikipedia.org/wiki/Seven-segment_display. The segments
// are named as A, B, C, D, E, F, G. In this coding table, segments A-G are mapped to bits 0-7.
// Bit 7 is not used in the coding. This display uses active high signal, in which '1' turns ON a
// segment, and '0' turns OFF a segment.
static uint8_t seg7Coding[10] = {
        0b00111111,         // digit 0
        0b00000110,         // digit 1
        0b01011011,         // digit 2
        0b01001111,         // digit 3
        0b01100110,         // digit 4
        0b01101101,         // digit 5
        0b01111101,         // digit 6
        0b00000111,         // digit 7
        0b01111111,         // digit 8
        0b01100111,         // digit 9
};

// The colon status: if colon == 0b10000000, then the colon is on,
// otherwise it is off.
static uint8_t colon = 0;

void checkPushButton(uint32_t time)
{
    int code = pbRead();
    uint32_t delay;
    int cntr_k=0, cntr_l=0,cntr_i=0,cntr_j=0;
    switch (code) {
    case 1:
        uprintf("%s\n\r", "SW1 is pushed.");
        delay = 250;                            // Use an inertia for soft debouncing
        if (cntr_k < 9)
            cntr_k++;
        else if (cntr_l < 5){
            cntr_k=0;
            cntr_l++;}
        else{
            cntr_k = 0;
            cntr_l = 0;}
        break;

    case 2:
        uprintf("%s\n\r", "SW2 is pushed");
        delay = 250;                            // Use an inertia for soft debouncing
        if (cntr_i < 9)
            cntr_i++;
        else if (cntr_j < 5){
            cntr_i = 0;
            cntr_j++;}
        else{
            cntr_i=0;
            cntr_j=0;}
        break;

    default:
        delay = 100;
    }

    schdCallback(checkPushButton, time + delay);
}


// GLOBAL VARS
// These variables are used for display on the clock
int min1 = 0;
int min2 = 0;
int sec1 = 0;
int sec2 = 0;
// Update the clock display
void clockUpdate(uint32_t time)                             // pointer to a 4-byte array
{
    uint8_t code[4];                                    // The 7-segment code for the four clock digits

    // Display 01:23 on the 7-segment displays
    // The colon ':' will flash on and off every 0.5 seconds
    code[0] = seg7Coding[sec2] + colon;
    code[1] = seg7Coding[sec1] + colon;
    code[2] = seg7Coding[min2] + colon;
    code[3] = seg7Coding[min1] + colon;
    seg7Update(code);

    // Calculate the display digits and colon setting for the next update
    if (colon == 0b00000000)
    {
        colon = 0b10000000;

        // If second2 hits 9 -> second2 resets, second1 increments
        if(sec2 >= 9)
        {
            sec2 = 0;                   // second2 resets

            // If second1 hits 5 -> second1 resets, minute2 increments
            if (sec1 >= 5)
            {
                sec1 = 0;               // second1 resets

                // If minute2 hits 9 -> minute2 resets, minute1 increments
                if (min2 >= 9)
                {
                    min2 = 0;           // minute2 resets

                    // If minute1 hits 5 -> RESET ALL
                    if (min1 >= 5)
                    {
                        min1 = 0;       // minute1 resets
                    }
                    else
                    {
                        min1++;         // INCREMENT
                    }
                }
                else
                {
                    min2++;             // INCREMENT
                }
            }
            // second1 less than 5 -> second1 increments
            else
            {
                sec1++;                 // INCREMENT
            }
        }
        // second2 less than 9 -> second2 increments
        else
        {
            sec2++;                     // INCREMENT
        }
    }
    else
    {
        colon = 0b00000000;
    }

    // Call back after 1 second
    schdCallback(clockUpdate, time + 500);
}

int main(void)
{
    lpInit();
    seg7Init();

    uprintf("%s\n\r", "Lab 2: Wall clock");

    // Schedule the first callback events for LED flashing and push button checking.
    // Those trigger callback chains. The time unit is millisecond.
    schdCallback(clockUpdate, 1000);

    // Loop forever
    while (true) {
        schdExecute();
    }
}
