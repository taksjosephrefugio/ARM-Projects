/*
This program reads data from ultrasonic sensor
and outputs the converted data into distance
(either in inches or cm) to the 7-segment

Components used:
Ultrasonic Sensor
7-Segment

Code Written by: Prof. Zhao Zhang, UIC, CS Department
Code Improved by: TAK'S JOSEPH REFUGIO
Code Written for: Ultrasonic Sensor Lab for Embedded Systems Class
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "launchpad.h"
#include "seg7.h"
#include "seg7digit.h"
#include <math.h>
#include "ranger.h"



seg7Display_t seg7Display = {
    0, 0, 0, 0, 0
};
enum {
    Millimeter,Inch
}  displayState = Millimeter;
enum {
    Pause,Run
}  sysState = Pause;


static uint32_t data = 0;
void display_update(uint32_t time){
       
       
        switch (displayState){
            case Millimeter:
            seg7Display.d4 = (int)(data/1000);
            seg7Display.d3 = (int)((data - (seg7Display.d4*1000))/100);
            seg7Display.d2 = (int)((data - ((seg7Display.d4*1000) + (seg7Display.d3*100)))/10);
            seg7Display.d1 = (int)((data - ((seg7Display.d4*1000) + (seg7Display.d3*100) + (seg7Display.d2*10))));
            break;
            case Inch:
            data = data/25.4;
            seg7Display.d4 = (int)(data/1000);
            seg7Display.d3 = (int)((data - (seg7Display.d4*1000))/100);
            seg7Display.d2 = (int)((data - ((seg7Display.d4*1000) + (seg7Display.d3*100)))/10);
            seg7Display.d1 = (int)((data - ((seg7Display.d4*1000) + (seg7Display.d3*100) + (seg7Display.d2*10))));
            break;
        }
        if(sysState == Run){
            seg7DigitUpdate(&seg7Display);
        }
    

    schdCallback(display_update,time+100);
}

void
checkPushButton(uint32_t time)
{

    int code = pbRead();
    uint32_t delay;

    switch (code) {
    case 1:
        switch(sysState){
            case Pause:
                sysState = Run;
                delay = 250;
                break;
            case Run:   
                sysState = Pause;
                delay = 250;
                break;
            default:
                delay = 10;
                break;
        }
        break;
    case 2:
        switch(displayState){
            case Inch:
                displayState = Millimeter;
                delay = 250;
                break;
            case Millimeter:   
                displayState = Inch;
                delay = 250;
                break;
            default:
                delay = 10;
                break;
        }
        break;
    default:
        delay = 10;
    }

    schdCallback(checkPushButton, time + delay);
}


void readData(uint32_t time){
    data   = getPulse();
    schdCallback(readData, time + 100);
}

int main(void)
{
    lpInit();
    seg7Init();
    rangerInit();
    uprintf("%s\n\r", "Lab 7: Sonar");

    schdCallback(checkPushButton, 1005);
    schdCallback(display_update, 1010);
    schdCallback(readData, 1015);

    // Run the event scheduler to process callback events
    while (true) {
        schdExecute();
    }
}
