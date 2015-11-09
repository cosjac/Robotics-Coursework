
#include "stdio.h"
#include "string.h"

#include "library/motor_led/e_init_port.h"
#include "library/motor_led/advance_one_timer/e_motors.h"
#include "library/motor_led/advance_one_timer/e_led.h"
#include "library/motor_led/advance_one_timer/e_agenda.h"
#include "library/uart/e_uart_char.h"
#include "library/a_d/advance_ad_scan/e_ad_conv.h"
#include "library/a_d/advance_ad_scan/e_prox.h"

void roam(void)
{
	/*long i;
	InitMotors();
	InitProx();
	LedClear();

	int left = 200;
	int right = 200;

	while (1) {
		SetLed(1,1);
		SetLed(2,1);

		// If robot is stuck, use front 4 sensors to confirm in range, do a degree spin to try get unstuck
		//if front left sensors, turn right
		if (GetProx(0) > 800 || GetProx(1) > 800) {
			if (GetProx(0) > 800) {
				SetLed(0,1);
			}
			if (GetProx(1) > 800) {
				SetLed(1,1);
			}
				
			// Do a 90 degree spin, 
			left=-450;right=450;
			SetSpeedRight(right);
			SetSpeedLeft(left);
			for(i=0;i<80000;i++) {asm("nop");}

			// set back to normal speed
			left = 200;
			right = 200;
			SetSpeedRight(right);
			SetSpeedLeft(left);
			
			//clear all LED lights
			LedClear();
		}

		//if front right sensors, turn left
		if (GetProx(6) > 800 || GetProx(7) > 800) {
			if (GetProx(6) > 800) {
				SetLed(7,1);
			}
			if (GetProx(7) > 800) {
				SetLed(7,1);
			}
				
			// Do a 90 degree spin, 
			left=-450;right=450;
			SetSpeedRight(left);
			SetSpeedLeft(right);
			for(i=0;i<80000;i++) {asm("nop");}

			// set back to normal speed
			left = 200;
			right = 200;
			SetSpeedRight(right);
			SetSpeedLeft(left);
			
			//clear all LED lights
			LedClear();
		}

		//if approach object or other sensors triggered then exit roam.
		if (GetProx(5) > 800 || GetProx(2) > 800 || GetProx(4) > 800 || GetProx(3) > 800) {
		
			SetLed(0,1);
			SetLed(1,1);
			SetLed(2,1);
			SetLed(3,1);
			SetLed(4,1);
			SetLed(5,1);
			SetLed(6,1);
			SetLed(7,1);
			return; //exit here
		}
	}*/
}
