/*
 * pwm_led.c, ECE 266 2017, Lab 8
 *
 *  Created on: Oct 13, 2016
 *      Author: zzhang
 */

#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/timer.h>
#include "pwmled.h"

// intensity level table. This table may be calibrated to each Tiva C LaunchPad
typedef struct {
    uint16_t pwmPeriod;
    uint16_t pwmDutyCycle;
} pwm_t;

pwm_t intensityLevelTable[] = {
  {100, 1}, {100, 2}, {100, 3}, {100, 4}, {100, 5}, {100, 6}, {100, 7}, {100, 8}, {100, 9}, {100, 10}
};

#define LED_INTENSITY_LEVEL_NUM     (sizeof(intensityLevelTable) / sizeof(pwm_t))

/*
 * Initialize the timer PWM functions connected to the three sub-LEDs.
 *    Red: 		PF1, T0CCP1, 16/32-bit Timer 0 channel B
 *    Blue:		PF2, T1CCP0, 16/32-bit Timer 1 channel A
 *    Green:    PF3, T1CCP1, 12/32-bit Timer 1 channel B
 */
void
pwmLedInit()
{
	// Enable Timer 0 and Timer 1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

	// Connect pins to those timers
	GPIOPinTypeTimer(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	GPIOPinConfigure(GPIO_PF1_T0CCP1);
	GPIOPinConfigure(GPIO_PF2_T1CCP0);
	GPIOPinConfigure(GPIO_PF3_T1CCP1);

	// Select PWM for Timer 0 sub-Timer B, and Timer 1 sub-Timers A & B
	TimerConfigure(TIMER0_BASE, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PWM));
	TimerConfigure(TIMER1_BASE, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM | TIMER_CFG_B_PWM));

	// The default setting is very bright, so change it to low density
	TimerLoadSet(TIMER0_BASE, TIMER_B, 100);
	TimerMatchSet(TIMER0_BASE, TIMER_B, 100-1);
	TimerLoadSet(TIMER1_BASE, TIMER_A, 100);
	TimerMatchSet(TIMER1_BASE, TIMER_A, 100-1);
	TimerLoadSet(TIMER1_BASE, TIMER_B, 100);
	TimerMatchSet(TIMER1_BASE, TIMER_B, 100-1);

	// Enable the Timer 0's TimerB and Timer 1's TimerA and TimerB
	TimerEnable(TIMER0_BASE, TIMER_B);
	TimerEnable(TIMER1_BASE, TIMER_A | TIMER_B);
}

/*
 * Set the intensity level of the LED (all colors). The lowest level is 1.
 */
void
pwmLedSetIntensity(int intensityLevel)
{	
    int n = intensityLevel - 1;
    pwm_t *pwm = &intensityLevelTable[n];

    // Range check on the intensity level
    if (n >= LED_INTENSITY_LEVEL_NUM)
        return;

	TimerLoadSet(TIMER0_BASE, TIMER_B, pwm->pwmPeriod);
	TimerMatchSet(TIMER0_BASE, TIMER_B, pwm->pwmPeriod - pwm->pwmDutyCycle);
	TimerLoadSet(TIMER1_BASE, TIMER_A, pwm->pwmPeriod);
	TimerMatchSet(TIMER1_BASE, TIMER_A, pwm->pwmPeriod - pwm->pwmDutyCycle);
	TimerLoadSet(TIMER1_BASE, TIMER_B, pwm->pwmPeriod);
	TimerMatchSet(TIMER1_BASE, TIMER_B, pwm->pwmPeriod - pwm->pwmDutyCycle);
}
