/* ###################################################################
**     Filename    : main.c
**     Project     : round1_a
**     Processor   : MKL25Z128VLK4
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2019-04-21, 20:06, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.01
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup main_module main module documentation
**  @{
*/         
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "ADC_AO.h"
#include "AdcLdd1.h"
#include "SIG.h"
#include "CLK_PWM.h"
#include "PwmLdd1.h"
#include "TU1.h"
#include "SI_TI.h"
#include "TimerIntLdd1.h"
#include "TU2.h"
#include "SI.h"
#include "PID.h"
#include "TimerIntLdd2.h"
#include "TU3.h"
#include "SERVO_PWM.h"
#include "PwmLdd2.h"
#include "TU4.h"
#include "onboard_led.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
/* User includes (#include below this line is not maintained by Processor Expert) */

///////////////// MY DEFINES /////////////////////////////
#define __THRESHOLD(Volts) ((Volts/3.3) * 65535)
#define VOLTS 2.3
#define START_SAMPLE 0
#define END_SAMPLE 126
#define SAMPLE_SIZE (END_SAMPLE - START_SAMPLE + 1)

volatile int clock_count = 200;
volatile int SI_count = 0;
int old_count = -1;
int sample_index = 0;
uint16_t value;
volatile char digital_output[SAMPLE_SIZE];
int si_count = 0;
int si_encoder = 0;
int measure = 0;
uint16_t motor_duty = 5500;
///////////////// END - MY DEFINES /////////////////////////////

/*lint -save  -e970 Disable MISRA rule (6.3) checking. */
int main(void)
/*lint -restore Enable MISRA rule (6.3) checking. */
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  onboard_led_ClrVal(onboard_led_Init);
  PID_Disable();
  for(;;)
  {
	  if (measure == 1)
	  {
		  ADC_AO_Measure(1);
		  ADC_AO_GetValue16(&value);


		  if (value > __THRESHOLD(VOLTS))
		  {
			  digital_output[clock_count] = '1';
		  }

		  if (value <= __THRESHOLD(VOLTS))
		  {
			  digital_output[clock_count] = '0';
		  }


		  if (si_count == 10)
		  {
			  PID_Enable();
		  }
		  measure = 0;
	  }
  }
  /* For example: for(;;) { } */

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.5 [05.21]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
