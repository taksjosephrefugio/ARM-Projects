#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "launchpad.h"
#include "keypad.h"
#include "led.h"

#define	PORT		GPIO_PORTC_BASE	// Port used for LED
#define	GREEN_LED	GPIO_PIN_4		//Pin 4 fro green LED
#define RED_LED		GPIO_PIN_5		//Pin 4 fro green LED
// Initializing LED Ports and Pins
void ledInit(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinTypeGPIOOutput(PORT,GREEN_LED|RED_LED);
	GPIOPinWrite(PORT,GREEN_LED|RED_LED, 0xFF);
}

// Turns Green LED On
void ledGreenOn()
{	
		GPIOPinWrite(PORT,GREEN_LED,0xFF);
}
// Turns Green LED On
void ledRedOn()
{
		GPIOPinWrite(PORT,RED_LED,0xFF);
}
// Turns Red LED On
void ledGreenOff()
{	
		GPIOPinWrite(PORT,GREEN_LED,0x00);
}
// Turns REd LED On
void ledRedOff()
{
		GPIOPinWrite(PORT,RED_LED,0x00);
}
