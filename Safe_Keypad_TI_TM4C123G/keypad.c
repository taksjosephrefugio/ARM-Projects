#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc/hw_nvic.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "launchpad.h"

#include "keypad.h"

#define KEYPAD_ROW_0 GPIO_PIN_0 ///< The row beginning with 1.
#define KEYPAD_ROW_1 GPIO_PIN_1 ///< The row beginning with 4.
#define KEYPAD_ROW_2 GPIO_PIN_2 ///< The row beginning with 7.
#define KEYPAD_ROW_3 GPIO_PIN_3 ///< The row beginning with a symbol.
#define KEYPAD_ROWS (KEYPAD_ROW_0|KEYPAD_ROW_1|KEYPAD_ROW_2|KEYPAD_ROW_3) ///< Handy for when you want to reference every pin connected to a row.

#define KEYPAD_COL_LEFT GPIO_PIN_4 ///< The column beginning with 1.
#define KEYPAD_COL_MID GPIO_PIN_5 ///< The column beginning with 2.
#define KEYPAD_COL_RIGHT GPIO_PIN_6 ///< The column beginning with 3.
#define KEYPAD_COLS (KEYPAD_COL_LEFT|KEYPAD_COL_MID|KEYPAD_COL_RIGHT) ///< Handy for when you want to reference every pin connected to a column.

#define KEYPAD_ROW_COUNT 4 ///< The number of rows on the keypad.
#define KEYPAD_COL_COUNT 3 ///< The number of columns on the keypad.

#define KEYPAD_PORT GPIO_PORTB_BASE
///< The variable used to remember which port the keypad is connected to.

static int keys[KEYPAD_ROW_COUNT][KEYPAD_COL_COUNT] = {
	{ 1,2,3 },
	{ 4,5,6 },
	{ 7,8,9 },
	{ 42,0,66 }
}; ///< table for keypad keys

/* Initializes the keypad. Remember keypad is GPIO Port B.*/
void keypadInit() {
	// Enable PORT B GPIO
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	GPIOPinTypeGPIOOutput(
		GPIO_PORTB_BASE,
		KEYPAD_COL_LEFT | KEYPAD_COL_RIGHT | KEYPAD_COL_MID
	);

	GPIOPinTypeGPIOInput(
		GPIO_PORTB_BASE,
		KEYPAD_ROW_0 | KEYPAD_ROW_1 | KEYPAD_ROW_2 | KEYPAD_ROW_3
	);

	GPIOPinWrite(GPIO_PORTB_BASE, KEYPAD_COLS, KEYPAD_COLS); // turn on every column
}

/*----------------------------------------------------------------------------
Checks to see which key (if any) on a given column is active.
*/
int checkKeypadCol(unsigned char checkCol) {
	long rowPins;

	GPIOPinWrite(KEYPAD_PORT, KEYPAD_COLS, checkCol);

	rowPins = GPIOPinRead(KEYPAD_PORT, KEYPAD_ROWS);
	if (rowPins & KEYPAD_ROW_0) {
		return 0;
	}
	else if (rowPins & KEYPAD_ROW_1) {
		return 1;
	}
	else if (rowPins & KEYPAD_ROW_2) {
		return 2;
	}
	else if (rowPins & KEYPAD_ROW_3) {
		return 3;
	}
	return -1;
}

/*----------------------------------------------------------------------------
Checks what number is being pressed on the keypad.

Checks col-by-col, checks the upper-left first.
*/
int keypadRead(void) {
	int row = 0;
	int col = 0;

	row = checkKeypadCol(KEYPAD_COL_LEFT);
	if (row != -1) {
		col = 0;
		return keys[row][col];
	}

	row = checkKeypadCol(KEYPAD_COL_MID);
	if (row != -1) {
		col = 1;
		return keys[row][col];
	}

	row = checkKeypadCol(KEYPAD_COL_RIGHT);
	if (row != -1) {
		col = 2;
		return keys[row][col];
	}
	return -1;
}

