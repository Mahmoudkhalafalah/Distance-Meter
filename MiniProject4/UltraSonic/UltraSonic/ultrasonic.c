/*
 * ultrasonic.c
 *
 *  Created on: Oct 21, 2023
 *      Author: Mahmoud Khalafallah
 */

#include "ultrasonic.h"
#include <util/delay.h>
#include "../ICU/icu.h"

uint8 g_tick = 0;
uint16 g_timeHigh = 0;
/*
 * Description:
 * Initialize the ICU driver as required.
 * Setup the ICU call back function.
 * Setup the direction for the trigger pin as output pin through the GPIO driver
 */
void Ultrasonic_init(void) {
	ICU_ConfigType configurations = { F_CPU_8, RISING };
	GPIO_setupPinDirection(ULTRASONIC_TRIGGER_PORT_ID,
	ULTRASONIC_TRIGGER_PIN_ID, PIN_OUTPUT);
	ICU_init(&configurations);
	ICU_setCallBack(Ultrasonic_edgeProcessing);
}
/*
 * Description:
 * Send the Trigger pulse to the ultrasonic.
 */
void Ultrasonic_Trigger(void) {

	/*Set trigger Pin high for 10 microseconds to send 8 pulses of 40 KHZ frequency*/
	GPIO_writePin(ULTRASONIC_TRIGGER_PORT_ID, ULTRASONIC_TRIGGER_PIN_ID,
	LOGIC_HIGH);
	_delay_us(10);
	GPIO_writePin(ULTRASONIC_TRIGGER_PORT_ID, ULTRASONIC_TRIGGER_PIN_ID,
	LOGIC_LOW);
}
/*
 * Description:
 * Send the trigger pulse by using Ultrasonic_Trigger function.
 * Start the measurements by the ICU from this moment.
 */
uint16 Ultrasonic_readDistance(void) {
	/*Call trigger function to start triggering signal*/
	Ultrasonic_Trigger();
	/* Wait until the echo pin reads A rising edge and a falling edge to measure high time*/
	while(g_tick != 2);
	/*
	 * cpu frequency = 8 MHZ and timer prescaler = F_CPU / 8
	 * Time for one instruction = 1 us
	 * sound speed = 34000 cm/s and the time taken by the signal is for going and coming back
	 * distance = (34000*time) / 2 = 17000*time
	 * distance = 17000*(timer value * 1 us)
	 * distance = 0.017*Timer value = Timer value / 58.8
	 */
	return (g_timeHigh/58)+1;

}
/*
 * Description:
 * This is the call back function called by the ICU driver.
 * This is used to calculate the high time (pulse time) generated by the ultrasonic sensor
 */
void Ultrasonic_edgeProcessing(void) {
	g_tick++;
	if (g_tick == 1) {
		/*
		 * Clear the timer counter register to start measurements from the
		 * first detected rising edge
		 */
		ICU_clearTimerValue();
		/* Detect falling edge */
		ICU_setEdgeDetectionType(FALLING);
	} else if (g_tick == 2) {
		/* Store the high time value +*/
		g_timeHigh = ICU_getInputCaptureValue();
		/* Clear the timer counter register to start measurements again*/
		ICU_clearTimerValue();
		/* Detect rising edge */
		ICU_setEdgeDetectionType(RISING);
	}
	else {
		/*reset the counter*/
		g_tick = 0;
	}
}
