/*
 * pwdled.h, ECE 266 2017, Lab 8
 *
 *  Created on: Oct 26, 2017
 *      Author: zzhang
 */

#ifndef PWDLED_H_
#define PWDLED_H_

/*
 * Initialize the timer PWM functions connected to the three sub-LEDs.
 */
void pwmLedInit();

/*
 * Set the intensity of the LED
 */
void pwmLedSetIntensity(int intensityLevel);

#endif /* PWDLED_H_ */
