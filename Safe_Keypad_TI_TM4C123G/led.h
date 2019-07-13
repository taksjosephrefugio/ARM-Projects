#ifndef LED_H_
#define LED_H_

#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

void ledInit();
void ledGreenOn();
void ledRedOn();
void ledGreenOff();
void ledRedOff();
#endif
