#include <stdint.h>
#include <stdbool.h>
#include <driverlib/sysctl.h>
#include <inc/hw_memmap.h>
#include <driverlib/gpio.h>
#include <driverlib/timer.h>
#include <driverlib/pin_map.h>
#include "launchpad.h"
#include "pwmbuzzer.h"


#define TIMER_BASE 		WTIMER0_BASE
#define INIT_PERIOD		100
#define	INIT_DLOW		1

void pwmBuzzerInit(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinConfigure(GPIO_PC4_WT0CCP0);
	GPIOPinTypeTimer(GPIO_PORTC_BASE,GPIO_PIN_4);
	TimerConfigure(TIMER_BASE,(TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM));
	TimerLoadSet(TIMER_BASE, TIMER_A, INIT_PERIOD); //  Initial Period
	TimerMatchSet(TIMER_BASE,TIMER_A, INIT_PERIOD - INIT_DLOW); // Initial Suty Cycle
	TimerEnable(TIMER_BASE,TIMER_A);

}

void pwmBuzzerPlay(int period,int duty){
	TimerLoadSet(TIMER_BASE,TIMER_A,period); // Set Period
	TimerMatchSet(TIMER_BASE,TIMER_A,period - duty); //  Set Duty Cycle.
}
