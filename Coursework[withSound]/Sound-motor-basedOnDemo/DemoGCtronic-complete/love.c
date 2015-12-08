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
#include <a_d/advance_ad_scan/e_acc.h>
#include <a_d/advance_ad_scan/e_micro.h>

#include "love.h"
#include "findColour.h"

char rbuffer[160];
int rnumbuffer[80];
int love_prox_data[8];
int isTouching = 0;

void get_prox(void)
{
	// get one single sample for all 8 sensors
	love_prox_data[0]=e_get_prox(0);
	love_prox_data[1]=e_get_prox(1);
	love_prox_data[2]=e_get_prox(2);
	love_prox_data[3]=e_get_prox(3);
	love_prox_data[4]=e_get_prox(4);
	love_prox_data[5]=e_get_prox(5);
	love_prox_data[6]=e_get_prox(6);
	love_prox_data[7]=e_get_prox(7);
}


void Kiss(void)
{
	//kiss sequence
	int i;
	for (i=0; i<8; i++)
	{
		if(love_prox_data[i]>100)
		{
			e_set_led(i,1);

			e_set_led(0,1);
			e_set_led(7,1);
		}
	}
	
	//sound
	e_play_sound(0, 2112);
}

//MAIN PROGRAM

void love(void)
{
	setColour('r');
	e_init_port();    // configure port pins   
	e_init_ad_scan(ALL_ADC);

	// sound
	e_init_sound();
    e_init_motors();
	e_acc_calibr();
	e_calibrate_ir();   
	e_start_agendas_processing();

	/*e_set_speed_left(0);
	e_set_speed_right(0);*/


	while(1)
	{
		get_prox();

		if(love_prox_data[0]>100 || love_prox_data[7]>100)
		{
			isTouching = 1;
			Kiss();
		} else {
			isTouching = 0;
		}

		if(isColourVis() == 0 && isTouching == 0)
		{
			e_set_speed_left(400);
			e_set_speed_right(-400);
		}

		while(isColourVis() == 1)
		{
			get_prox();
			if(love_prox_data[0]>100 || love_prox_data[7]>100)
			{
				isTouching = 1;

				Kiss();

			} else {
				isTouching = 0;

				e_set_speed_left(300);
				e_set_speed_right(300);
			}

			findColour();

			//double i = 0;
			//for(i=0;i<4000;i++) {asm("nop");}
		}

/*
		if(isTouching == 1)
		{
			//kiss sequence
			int i;
			for (i=0; i<8; i++)
			{
				if(love_prox_data[i]>1000)
				{
					e_set_led(i,1);
				}
			}

			e_play_sound(0, 2112);

			//double j = 0;
			//for(j=0;j<4000;j++) {asm("nop");}
		}
*/		
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




