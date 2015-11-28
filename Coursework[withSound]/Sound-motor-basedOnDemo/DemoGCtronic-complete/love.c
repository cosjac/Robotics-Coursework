#include <stdlib.h>
#include "stdio.h"
#include "string.h"

#include <motor_led/e_init_port.h>
#include <motor_led/advance_one_timer/e_motors.h>
#include <motor_led/advance_one_timer/e_led.h>
#include <motor_led/advance_one_timer/e_agenda.h>
#include <uart/e_uart_char.h>
#include <a_d/advance_ad_scan/e_ad_conv.h>
#include <a_d/advance_ad_scan/e_prox.h>
#include <codec/e_sound.h>
#include <motor_led/e_epuck_ports.h>
#include <camera/fast_2_timer/e_poxxxx.h>

#include "love.h"
#include "findColour.h"

char rbuffer[160];
int rnumbuffer[80];
int isTouching = 0;

//MAIN PROGRAM

void love(void)
{

	setColour('r');
	e_init_port();
	e_init_motors();
	e_init_ad_scan(ALL_ADC);
	e_init_sound();

	e_calibrate_ir();

	e_start_agendas_processing();

	/*e_set_speed_left(0);
	e_set_speed_right(0);*/

	while(1)
	{
		if(e_get_prox(0)>1000 || e_get_prox(7)>1000)
		{
			isTouching = 1;
		} else {
			isTouching = 0;
		}

		if(isColourVis() == 0)
		{
			e_set_speed_left(400);
			e_set_speed_right(-400);
		}

		while(isColourVis() == 1 && isTouching != 1)
		{
			e_set_speed_left(300);
			e_set_speed_right(300);

			e_set_led(1,1);

			findColour();

			double i = 0;
			for(i=0;i<4000;i++) {asm("nop");}
		}

		if(isTouching == 1)
		{
			//kiss sequence
			e_set_speed_left(0);
			e_set_speed_right(0);
			e_set_led(0,1);
			e_set_led(1,1);
			e_set_led(6,1);
			e_set_led(7,1);

			e_play_sound(11028, 8016);
			e_close_sound;

			double j = 0;
			for(j=0;j<4000;j++) {asm("nop");}
		}
		
	}
}
/*
Changes:
	findColour: Line178 findColour.c
	removed include curious
issues:
	isTouching...
Questions:
	How do you find out what numbers you need for sound?
*/




