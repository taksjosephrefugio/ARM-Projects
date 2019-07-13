/*
This program outputs the position of the rotary knob 
either in percentile mode (0-100) or in raw ADC digital output. 

Components used:
Rotary Knob
7-Segment

Code Written by: Prof. Zhao Zhang, UIC, CS Department
Code Improved by: TAK'S JOSEPH REFUGIO
Code Written for: ADC Lab for Embedded Systems Class
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "launchpad.h"
#include "seg7.h"
#include "seg7digit.h"
#include "rotary.h"

uint16_t readVal;
int switchState = 0;

// The initial state of the 7-segment display: last number is for colon display
seg7Display_t seg7Display = {
    0, 0, 0, 0, 0
};

// FUNCTION PROTOTYPES
void checkPushButton(uint32_t time);
void displayUpdate(uint32_t time);

int main(void)
{
    lpInit();
    seg7Init();
    ADCInit();

    uprintf("%s\n\r", "Lab 6: Knob Control (ADC)");

    // Update the clock display
    seg7DigitUpdate(&seg7Display);


    displayUpdate(1005);
    schdCallback(checkPushButton, 1000);
    // Run the event scheduler to process callback events
    while (true) {
        schdExecute();
    }
}
void checkPushButton(uint32_t time)
{
    int code = pbRead();
    uint32_t delay;

    switch(code)
    {
    case 1:
        switchState = 1;
        delay = 250;
        break;
    case 2:
        switchState = 2;
        delay = 250;
        break;
    }
    schdCallback(checkPushButton, time + delay);
}

// Event-driven code for display to the 7-segment
void displayUpdate(uint32_t time)
{
    uint32_t delay = 100;
    readVal = ADCRead();

    uint16_t temp2 = (readVal*100)/4095;


    if(switchState == 2) {
        int num3 = temp2 / 100;
        temp2 = temp2 % 100;

        int num2 = temp2 / 10;
        temp2 = temp2 % 10;

        int num1 = temp2;

        seg7Display.d4 = 10;
        seg7Display.d3 = num3;
        seg7Display.d2 = num2;
        seg7Display.d1 = num1;
        seg7Display.colon = 0;

        if (num3 == 0) {
            seg7Display.d4 = 10;
            seg7Display.d3 = 10;
            seg7Display.d2 = num2;
            seg7Display.d1 = num1;
            seg7Display.colon = 0;
        }
    }
    else {
        uint16_t temp = readVal;
        int num4 = temp / 1000;
        temp = temp % 1000;

        int num3 = temp / 100;
        temp = temp % 100;

        int num2 = temp / 10;
        temp = temp % 10;

        int num1 = temp;

        seg7Display.d4 = num4;
        seg7Display.d3 = num3;
        seg7Display.d2 = num2;
        seg7Display.d1 = num1;
        seg7Display.colon = 0;
    }

    uprintf("/SwitchState: %d\n\r", switchState);
    seg7DigitUpdate(&seg7Display);
    schdCallback(displayUpdate, time + delay);
}
