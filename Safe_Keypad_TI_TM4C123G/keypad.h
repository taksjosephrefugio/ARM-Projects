#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

/*--------------------------------------------------------------------------*/
/** Initializes the keypad port.

Sets columns to output, rows to input with internal pull-downs. Uses pins
0-6 and leaves pin 7 on the port alone.

@return No return value.
*/
void keypadInit();

/*--------------------------------------------------------------------------*/
/** Checks to see which key (if any) on a given column is active.

Drives one column high, then checks each row to see if they have been
driven high.

@param checkCol The column to check.

@return The row that's active on the given column. Returns -1 if
no row is active.
*/
int checkKeypadCol(unsigned char checkCol);

/*--------------------------------------------------------------------------*/
/** Checks every column/row of the keypad and returns the key being pressed.

Runs checkKeypadCol on every column until it finds a pressed key, then
looks up the value for that column/row combination in a table.

@see checkKeypadCol()

@return The number of the key pressed.  Symbols # and * each return -1.
*/
int keypadRead(void);

#endif /* KEYPAD_H_ */