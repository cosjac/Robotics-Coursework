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

void checkProx(void)
{	//set isTouching to 1 if front prox sensors are close to object
	e_init_uart1();
	e_send_uart1_char("\f\a", 2);

	printf("proximity0 = %d, proximity7 = %d", e_get_prox(0), e_get_prox(7));
	if(e_get_prox(0)>300 || e_get_prox(7)>300)
	{
		isTouching = 1;
	} else {
		isTouching = 0;
	}
}

//MAIN PROGRAM

void love(void)
{

	setColour('r');	//set colour to look for
	e_init_port();

	e_init_motors();
	e_init_ad_scan(ALL_ADC);

	e_calibrate_ir();

	e_start_agendas_processing();

	while(1)
	{
		checkProx();
		
		//spin e-puck if it has not already found colour
		if(isColourVis() == 0 && isTouching == 0)
		{
			e_set_speed_left(400);
			e_set_speed_right(-400);
		}
		
		//move toward colour until close
		while(isColourVis() == 1 && isTouching == 0)
		{
			checkProx();
			
			e_set_speed_left(300);
			e_set_speed_right(300);

			findColour();
		}

		if(isTouching == 1)
		{
			//kiss sequence
			e_set_speed_left(0);
			e_set_speed_right(0);
			e_set_led(0,1);
			e_set_led(1,1);
			e_set_led(7,1);
		}
		
	}
}





