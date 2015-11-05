#include "Libraries Clean/init_port.h"
#include "Libraries Clean/motors.h"
#include "Libraries Clean/prox.h"
#include "Libraries Clean/led.h"
#include "library/motor_led/e_led.h"

void roam(void)
{
	long i;
	InitMotors();
	InitProx();
	LedClear();
	SetStepsRight(1);
	SetStepsLeft(1);

	//set initial robot speed to 150 (normal speed)
	int left = 150;
	int right = 150;
	SetSpeedRight(right);
	SetSpeedLeft(left);

	while (1) {
		// If robot is stuck, use front 4 sensors to confirm in range, do a degree spin to try get unstuck
		if (GetProx(0) > 800 || GetProx(1) > 800) {
			if (GetProx(0) > 800) {
				SetLed(0,1);
			}
			if (GetProx(1) > 800) {
				SetLed(1,1);
			}
			if (GetProx(6) > 800) {
				SetLed(7,1);
			}
			if (GetProx(7) > 800) {
				SetLed(7,1);
			}
				
			// Do a 90 degree spin, 
			left=-450;right=450;
			SetSpeedRight(right);
			SetSpeedLeft(left);
			for(i=0;i<80000;i++) {asm("nop");}

			// set back to normal speed
			left = 150;
			right = 150;
			SetSpeedRight(right);
			SetSpeedLeft(left);
			
			//clear all LED lights
			LedClear();
		}

		if (GetProx(6) > 800 || GetProx(7) > 800) {
			if (GetProx(0) > 800) {
				SetLed(0,1);
			}
			if (GetProx(1) > 800) {
				SetLed(1,1);
			}
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
			left = 150;
			right = 150;
			SetSpeedRight(right);
			SetSpeedLeft(left);
			
			//clear all LED lights
			LedClear();
		}

		//after X amount of time, perform a random spin.
		
	}

}
