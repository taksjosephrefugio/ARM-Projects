/* ###################################################################
**     Filename    : Events.c
**     Project     : round1_a
**     Processor   : MKL25Z128VLK4
**     Component   : Events
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2019-04-21, 20:06, # CodeGen: 0
**     Abstract    :
**         This is user's event module.
**         Put your event handler code here.
**     Contents    :
**         Cpu_OnNMIINT - void Cpu_OnNMIINT(void);
**
** ###################################################################*/
/*!
** @file Events.c
** @version 01.00
** @brief
**         This is user's event module.
**         Put your event handler code here.
*/         
/*!
**  @addtogroup Events_module Events module documentation
**  @{
*/         
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"

#ifdef __cplusplus
extern "C" {
#endif 


/* User includes (#include below this line is not maintained by Processor Expert) */

/*
** ===================================================================
**     Event       :  Cpu_OnNMIINT (module Events)
**
**     Component   :  Cpu [MKL25Z128LK4]
*/
/*!
**     @brief
**         This event is called when the Non maskable interrupt had
**         occurred. This event is automatically enabled when the [NMI
**         interrupt] property is set to 'Enabled'.
*/
/* ===================================================================*/
void Cpu_OnNMIINT(void)
{
  /* Write your code here ... */
}

/*
** ===================================================================
**     Event       :  CLK_PWM_OnEnd (module Events)
**
**     Component   :  CLK_PWM [PWM]
**     Description :
**         This event is called when the specified number of cycles has
**         been generated. (Only when the component is enabled -
**         <Enable> and the events are enabled - <EnableEvent>). The
**         event is available only when the <Interrupt service/event>
**         property is enabled and selected peripheral supports
**         appropriate interrupt.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
extern volatile int clock_count;
extern int measure;
extern int done;
void CLK_PWM_OnEnd(void)
{
  // Write your code here
  clock_count = clock_count + 1;

  if (clock_count >= 0 && clock_count <= 126)
  {
	  measure = 1;
  }

  if (clock_count >= 130)
  {
	  SI_TI_Enable();
	  SI_SetVal(SI_Init);
	  clock_count = 0;
  }
}

/*
** ===================================================================
**     Event       :  SI_TI_OnInterrupt (module Events)
**
**     Component   :  SI_TI [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the component is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
#define START_SAMPLE 0
#define END_SAMPLE 126
#define SAMPLE_SIZE (END_SAMPLE - START_SAMPLE + 1)

extern volatile int si_count;
extern volatile int si_encoder;
extern volatile char digital_output[SAMPLE_SIZE];
void SI_TI_OnInterrupt(void)
{
  /* Write your code here ... */
	if (si_count < 15)
	{
		si_count = si_count + 1;
	}
	if (si_encoder >=0 && si_encoder <= 5)
	{
		si_encoder = si_encoder + 1;
	}

	SI_ClrVal(SI_Init);
	SI_TI_Disable();
}

/*
** ===================================================================
**     Event       :  PID_OnInterrupt (module Events)
**
**     Component   :  PID [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the component is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
#define START_SAMPLE 0
#define END_SAMPLE 126
#define SAMPLE_SIZE (END_SAMPLE - START_SAMPLE + 1)
#define LEFT_3_START 8
#define LEFT_3_END 13
#define LEFT_2_START 14
#define LEFT_2_END 25
#define LEFT_1_START 26
#define LEFT_1_END 64
#define RIGHT_1_START 65
#define RIGHT_1_END 90
#define RIGHT_2_START 91
#define RIGHT_2_END 116
#define RIGHT_3_START 117
#define RIGHT_3_END 122
#define MAX_RIGHT_PWM 1500
#define MAX_LEFT_PWM 1100
#define LSB 10
#define RSB 116


int count_0_left1;
int count_0_left2;
int count_0_left3;
int count_0_right1;
int count_0_right2;
int count_0_right3;

static int weight_left1 = 2;	// 3
static int weight_left2 = 5;	// 8
static int weight_left3 = 15;	// 15
static int weight_right1 = -2;
static int weight_right2 = -5;
static int weight_right3 = -15;


int prev_error;
int error = 0;
static float Kp = 1.5;     // 2
static float Kd = 0.5;   // 0.2
static int servo_setpoint = 1300;

uint16_t update_duty;
uint16_t prev_update_duty;

extern volatile char digital_output[SAMPLE_SIZE];

int index = 0;
int right_index = 126;
int left_index = 0;
int bound;
int total_count;

void PID_OnInterrupt(void)
{

	 count_0_left1 = 0;
	 count_0_left2 = 0;
	 count_0_left3 = 0;
	 count_0_right1 = 0;
	 count_0_right2 = 0;
	 count_0_right3 = 0;


	  //loop through digital_output zones to find zeros
	 for (int i = 10; i < 110; i++)
	 {
		 if (i >= LEFT_3_START && i <= LEFT_3_END && digital_output[i] == '0')
		 {
			 count_0_left3 = count_0_left3 + 1;
		 }

		 if (i >= LEFT_2_START && i <= LEFT_2_END && digital_output[i] == '0')
		 {
			 count_0_left2 = count_0_left2 + 1;
		 }

		 if (i >= LEFT_1_START && i <= LEFT_1_END && digital_output[i] == '0')
		 {
			 count_0_left1 = count_0_left1 + 1;
		 }

		 if (i >= RIGHT_1_START && i <= RIGHT_1_END && digital_output[i] == '0')
		 {
			 count_0_right1 = count_0_right1 + 1;
		 }

		 if (i >= RIGHT_2_START && i <= RIGHT_2_END && digital_output[i] == '0')
		 {
			 count_0_right2 = count_0_right2 + 1;
		 }

		 if (i >= RIGHT_3_START && i <= RIGHT_3_END && digital_output[i] == '0')
		 {
			 count_0_right3 = count_0_right3 + 1;
		 }
	 }


	 total_count = count_0_left3 + count_0_left2 + count_0_left1 + count_0_right1 + count_0_right2 + count_0_right3;

	 if (total_count < 5)
	 {

		 if (bound == -1)
		 {
			 update_duty = MAX_LEFT_PWM;
		 }
		 if(bound == 1)
		 {
			 update_duty = MAX_RIGHT_PWM;
		 }

	 }
	 else
	 {

		 error = (count_0_left3*weight_left3)+
				 (count_0_left2*weight_left2)+
				 (count_0_left1*weight_left1)+
				 (count_0_right1*weight_right1)+
				 (count_0_right2*weight_right2)+
				 (count_0_right3*weight_right3);


		 if (error == 0)
		 {
			 bound = 0;
		 }

		 // right bound
		 if (error < 0)
		 {
			 bound = 1;
		 }

		 // left bound
		 if (error > 0)
		 {
			 bound = -1;
		 }

		 //update_duty = -(Kp*error) + servo_setpoint;
		 update_duty = -((Kp*error) + (Kd*(error-prev_error))) + servo_setpoint;
		 prev_error = error;

		 if (update_duty < 1124)
		 {
			 update_duty = MAX_LEFT_PWM;
		 }

		 if (update_duty > 1790)
		 {
			update_duty = MAX_RIGHT_PWM;
		 }
	 }

	 SERVO_PWM_SetDutyUS(20000 - update_duty);

}

/*
** ===================================================================
**     Event       :  SERVO_PWM_OnEnd (module Events)
**
**     Component   :  SERVO_PWM [PWM]
**     Description :
**         This event is called when the specified number of cycles has
**         been generated. (Only when the component is enabled -
**         <Enable> and the events are enabled - <EnableEvent>). The
**         event is available only when the <Interrupt service/event>
**         property is enabled and selected peripheral supports
**         appropriate interrupt.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void SERVO_PWM_OnEnd(void)
{
  /* Write your code here ... */
}

/* END Events */

#ifdef __cplusplus
}  /* extern "C" */
#endif 

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
