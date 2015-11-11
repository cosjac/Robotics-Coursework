#include "stdio.h"
#include "string.h"

#include "library/motor_led/e_init_port.h"
#include "library/motor_led/advance_one_timer/e_motors.h"
#include "library/motor_led/advance_one_timer/e_led.h"
#include "library/motor_led/advance_one_timer/e_agenda.h"
#include "library/uart/e_uart_char.h"
#include "library/a_d/advance_ad_scan/e_ad_conv.h"
#include "library/a_d/advance_ad_scan/e_prox.h"
#include "library/codec/e_sound.h"
#include "library/motor_led/e_epuck_ports.h"

#include "fear.h"


void LedFearFlash(void)
{
	e_led_clear();
	int i, x = 0;
	for ( i; i < 10; i++ ) {
		e_set_led(i,1);
		for ( x; x < 10000; x++ ) {asm("nop");}
		e_set_led(i,0);
	}
}

void fear(void)
{
	long i;
	int left, right;

//	e_init_sound();
	

	e_start_agendas_processing();

	e_set_speed_left(500);
	e_set_speed_right(500);


	while(1) {
		e_set_led(1,1);	

		// If robot is stuck, use front 4 sensors to confirm in range, do a degree spin to try get unstuck
		//if front left sensors, turn right
		if (e_get_prox(0) > 800 || e_get_prox(1) > 800) {
			if (e_get_prox(0) > 800) {
				e_set_led(0,1);
			}
			if (e_get_prox(1) > 800) {
				e_set_led(1,1);
			}
				
			// Do a 90 degree spin, 
			left=-450;right=450;
			e_set_speed_right(right);
			e_set_speed_left(left);
			for(i=0;i<40000;i++) {asm("nop");}

			// set back to normal speed
			e_set_speed_left(500);
			e_set_speed_right(500);
			
			//clear all LED lights
			e_led_clear();
		}

		//if front right sensors, turn left
		if (e_get_prox(6) > 800 || e_get_prox(7) > 800) {
			if (e_get_prox(6) > 800) {
				e_set_led(7,1);
			}
			if (e_get_prox(7) > 800) {
				e_set_led(7,1);
			}
				
			left=450;right=-450;
			e_set_speed_right(right);
			e_set_speed_left(left);
			for(i=0;i<40000;i++) {asm("nop");}

			// set back to normal speed
			e_set_speed_left(500);
			e_set_speed_right(500);
			
			//clear all LED lights
			e_led_clear();
		}

		//if back sensors, run away fast
		if (e_get_prox(3) > 800 || e_get_prox(5) > 800) {
			
			// Run away fast with fear flash
			e_set_led(0,1);
			e_set_led(1,1);
			e_set_led(2,1);
			e_set_led(3,1);
			e_set_led(4,1);
			e_set_led(5,1);
			e_set_led(6,1);
			e_set_led(7,1);
			e_play_sound(11028, 8016);

			e_set_speed_left(1500);
			e_set_speed_right(1500);
			for(i=0;i<10000;i++) {asm("nop");}

			// Set back to normal speed
			e_led_clear();
			e_set_speed_left(500);
			e_set_speed_right(500);
		}

		
	};
}

/*	long i;
	InitMotors();
	InitProx();
	LedClear();
	SetStepsRight(1);
	SetStepsLeft(1);
	int left = 200;
	int right = 200;
	SetSpeedRight(right);
	SetSpeedLeft(left);

	// Init sound
	e_init_port();
    e_init_ad_scan(ALL_ADC);
	e_init_sound();

	while (1) {
		// If robot is stuck, use front 4 sensors to confirm in range, do a degree spin to try get unstuck
		//if front left sensors, turn right
		if (e_get_prox(0) > 800 || e_get_prox(1) > 800) {
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
		//if back sensors, run away fast
		if (GetProx(3) > 800 || GetProx(5) > 800) {
			// Run away fast with fear flash
			LedFearFlash();
			left=1000;right=1000;
			SetSpeedRight(left);
			SetSpeedLeft(right);
			for(i=0;i<800000;i++) {asm("nop");}

			// Set back to normal speed
			left=150;right=150;
			SetSpeedRight(left);
			SetSpeedLeft(right);
		}

		//if identify green object
		
		//find closest prox sensor and turn on and make a noise, then spin 180 degrees and move X time, 
		//turn on fear flashing lights, spin 360 degrees then move forward X time
		if (GetProx(0) > 800 || GetProx(1) > 800 || GetProx(2) > 800 || GetProx(4) > 800 || GetProx(6) > 800 || GetProx(7) > 800) {
			if (GetProx(0) > 800) {
				SetLed(0,1);
			}
			if (GetProx(1) > 800) {
				SetLed(1,1);
			}
			if (GetProx(2) > 800) {
				SetLed(2,1);
			}
			if (GetProx(4) > 800) {
				SetLed(4,1);
			}
			if (GetProx(6) > 800) {
				SetLed(6,1);
			}
			if (GetProx(7) > 800) {
				SetLed(7,1);
			}
			
			//signal noise and turn on body led
			//e_play_sound(11028, 8016);
			SetBodyLed(1);

			//halt at the object for a few seconds
			left=1800;right=-1800;
			SetSpeedRight(right);
			SetSpeedLeft(left);
			for(i=0;i<350000;i++) {asm("nop");}

			left=-1000;right=-1000;
			SetSpeedRight(right);
			SetSpeedLeft(left);
			for(i=0;i<100000;i++) {asm("nop");}
	
			//spin 180 degrees 
			left=-900;right=900;
			SetSpeedRight(right);
			SetSpeedLeft(left);
			for(i=0;i<665000;i++) {asm("nop");}
	
			//move X time fast forwards
			left=1000;right=1000;
			SetSpeedRight(left);
			SetSpeedLeft(right);
			for(i=0;i<1000000;i++) {asm("nop");}
	
			//turn on fear led lights 
			LedFearFlash();
	
			//spin 540 degrees 
			left=900;right=-900;
			SetSpeedRight(right);
			SetSpeedLeft(left);
			for(i=0;i<1995000;i++) {asm("nop");}
	
			//move X time fast forwards
			left=1000;right=1000;
			SetSpeedRight(left);
			SetSpeedLeft(right);
			for(i=0;i<500000;i++) {asm("nop");}
	
			//turn off body led and leds
			SetBodyLed(0);
			LedClear();
	
			//return to normal speed
			left=150;right=150;
			SetSpeedRight(left);
			SetSpeedLeft(right);
		}

	}
}*/
