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

#include "aggressive.h"
#include "findColour.h"


void aggressive(void)
{
	long i;
	int left, right;

//	e_init_sound();
	

	e_start_agendas_processing();

	e_set_speed_left(500);
	e_set_speed_right(500);

	setColour('r');
	

	while(1)
	{
		e_set_led(1,1);	
/*
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

		//if back sensors, turn to face
		if (e_get_prox(3) > 800 || e_get_prox(5) > 800) {

			e_set_speed_left(1500);
			e_set_speed_right(-1500);
			for(i=0;i<10000;i++) {asm("nop");}

			// Set back to normal speed
			//e_led_clear();
			e_set_speed_left(500);
			e_set_speed_right(500);
		}
*/
		//run aggressive code
		while(isColourVis() == 1)
		{	
			findColour();
			//e_set_speed_left(-500);
			//e_set_speed_right(-500);
			//for(i=0;i<200000;i++) {asm("nop");}

			e_set_speed_left(1000);
			e_set_speed_right(1000);

		/*
			for(i=0;i<500000;i++)
			{
				asm("nop");
				if(isColourVis() == 0)
				{
					break;
				}
			}
		*/
			while(isColourVis() == 0 && i<1000000)
			{
				e_set_speed_left(-500);
				e_set_speed_right(500);
				
				findColour();
				i++;
			}
			

			//e_set_speed_left(0);
			//e_set_speed_right(0);
		}		

		//aggressive noise

	}
}
