#include "stdio.h"
#include "string.h"
#include "math.h"
#include <time.h>
#include <stdlib.h>

#include <codec/e_sound.h>
#include <motor_led/e_init_port.h>
#include <motor_led/advance_one_timer/e_led.h>
#include <motor_led/advance_one_timer/e_motors.h>
#include <motor_led/advance_one_timer/e_agenda.h>
#include <camera/fast_2_timer/e_poxxxx.h>
#include <uart/e_uart_char.h>
#include <a_d/advance_ad_scan/e_ad_conv.h>
#include <a_d/advance_ad_scan/e_prox.h>
#include <a_d/advance_ad_scan/e_acc.h>
#include <a_d/advance_ad_scan/e_micro.h>
#include <bluetooth/e_bluetooth.h>
#include <motor_led/e_epuck_ports.h>

#include "aggressive_c.h"
#include "global_functions.h"


#define TURN_SPEED 500

int agg_current_prox_data[8];
int agg_obstacle_present = 0; 

int agg_check_prox_sensors(void)
{
	int i; 

	//get one single sample for all 8 sensors
	agg_current_prox_data[0]=e_get_prox(0);
	agg_current_prox_data[1]=e_get_prox(1);
	agg_current_prox_data[2]=e_get_prox(2);
	agg_current_prox_data[3]=e_get_prox(3);
	agg_current_prox_data[4]=e_get_prox(4);
	agg_current_prox_data[5]=e_get_prox(5);
	agg_current_prox_data[6]=e_get_prox(6);
	agg_current_prox_data[7]=e_get_prox(7);

    // Detect obstacle_present on any of the 8 sensors
	agg_obstacle_present=0;

	for (i=0; i<8; i++) {
		if(agg_current_prox_data[i]>1000) {
			agg_obstacle_present = 1;
		}
	}

	// Detect if robot upside down
	int z;
	z = e_get_acc(2);

	if(z < 2100)	//LED4 on if e-puck is on the back
	{
		LED0 = 0;
		LED4 = 1;
	}
	else		//LED0 on if e-puck is on his wells
	{
		LED0 = 1;
		LED4 = 0;
	} 

    // if sensors detect obstacle_present return 1
	// robot is upside down return 2
	return agg_obstacle_present;   					
}


//flash LEDs 
void LedAggFlash(void)
{
	e_led_clear();
	int i;
	for (i = 0; i < 10; i++)
	{
		e_set_led(i,1);
		//wait(10000);
		e_set_led(i,0);
	}
}

//turn on all LEDs
void AggLedFearAll(void)
{
	e_led_clear();
	int i;
	for(i = 0; i < 10; i++)
	{
		e_set_led(i,1);
	}
}

void aggressive_c(void)
{
	long i;
	int agg_closest_present;

	e_init_port();    // configure port pins   
	e_init_ad_scan(ALL_ADC);

	// sound
	e_init_sound();
    e_init_motors();
	e_acc_calibr();
	e_calibrate_ir();   
	e_start_agendas_processing();

	e_set_speed_left(TURN_SPEED);
	e_set_speed_right(TURN_SPEED);

	while(1)
	{
		e_led_clear();
		e_set_speed_left(TURN_SPEED);
		e_set_speed_right(TURN_SPEED);

		while (agg_obstacle_present==0)   	// do this loop until an event has occurred
		{
			agg_obstacle_present = agg_check_prox_sensors(); // check if obstacle present is detected
        }

		// if there is an obstacle present then:
		if(agg_obstacle_present==1)
		{
			e_led_clear();
			//turn on closest LED lights to obstacle position

			for (i=0; i<8; i++)
			{
				if(agg_current_prox_data[i]>1000)
				{
					e_set_led(i,1);
					agg_closest_present = i;
				}
			}

			if (agg_closest_present == 4 || agg_closest_present == 3)
			{
				// spin to face the obstacle
				e_set_speed_left(-1800);
				e_set_speed_right(1800);
				wait(680000);
						
				//oi
				e_play_sound(0, 2112);
				LedAggFlash();

				// run into obstacle
				e_set_speed_left(1800);
				e_set_speed_right(1800);
				wait(500000);
			}
			else if(agg_closest_present == 0 || agg_closest_present == 7)
			{
				e_play_sound(0, 2112);

				// run into obstacle
				e_set_speed_left(1800);
				e_set_speed_right(1800);
				wait(500000);
			}

			//reverse
			e_set_speed_left(-1800);
			e_set_speed_right(-1800);
			wait(200000);
			
			agg_obstacle_present = 0;
   		}
		
		// if robot upside down then:
		if (agg_obstacle_present==2) {
			e_set_led(0,1);
			e_set_led(1,1);
			e_set_led(2,1);
			e_set_led(3,1);
			e_set_led(4,1);
			e_set_led(5,1);

			agg_obstacle_present = 0;
			e_led_clear();
		}
	};
}
