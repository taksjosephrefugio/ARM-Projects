#include <stdint.h>
#include <stdbool.h>
#include <driverlib/sysctl.h>
#include <inc/hw_memmap.h>
#include <driverlib/gpio.h>
#include <driverlib/timer.h>
#include <driverlib/pin_map.h>
#include "launchpad.h"
#include "ranger.h"


void rangerInit(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0); // Enable Wide Timer 0 
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC); // Enable GPIO Pin
	TimerConfigure(WTIMER0_BASE,(TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_TIME_UP));
	TimerControlEvent(WTIMER0_BASE,TIMER_A,TIMER_EVENT_BOTH_EDGES);

}

uint32_t getPulse(){
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,GPIO_PIN_4); //Configure Pin as a GPIO Output Pin	
	GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_4,0x00); 	// Set Pin to low
	waitUs(2);										// Wait for 2µs 
	GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_4,0xFF);	// Set Pin to High
	waitUs(5);										// Wait for 5µs
	GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_4,0x00);	// Set Pin to Low
	
	
	GPIOPinConfigure(GPIO_PC4_WT0CCP0);				// Configure Pin as a Timer
	GPIOPinTypeTimer(GPIO_PORTC_BASE,GPIO_PIN_4); 	// Route Timer to GPIO Pin
	TimerPrescaleSet(WTIMER0_BASE,TIMER_A,10);
	TimerEnable(WTIMER0_BASE,TIMER_A);				// Enable Timer
	TimerIntEnable(WTIMER0_BASE,TIMER_CAPA_EVENT);	// 
	TimerIntClear(WTIMER0_BASE,TIMER_CAPA_EVENT);	// Clear Timer Interrupt
	
	
	while(!TimerIntStatus(WTIMER0_BASE,true));
	TimerIntClear(WTIMER0_BASE,TIMER_CAPA_EVENT);
	float rise = ((float)(TimerValueGet(WTIMER0_BASE,TIMER_A)))/32768;	
	while(!TimerIntStatus(WTIMER0_BASE,true));
	TimerIntClear(WTIMER0_BASE,TIMER_CAPA_EVENT);
	float fall = ((float)(TimerValueGet(WTIMER0_BASE,TIMER_A)))/32768;
	uint32_t dif = ((fall - rise)*170);
	return dif;
}

