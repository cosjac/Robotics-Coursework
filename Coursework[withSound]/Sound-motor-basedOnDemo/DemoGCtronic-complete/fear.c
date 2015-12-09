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

#include "fear.h"
#include "global_functions.h"


#define TURN_SPEED 500

int current_prox_data[8];
int obstacle_present = 0; 

int check_prox_sensors(void)
{
	int i; 

	// get one single sample for all 8 sensors
	current_prox_data[0]=e_get_prox(0);
	current_prox_data[1]=e_get_prox(1);
	current_prox_data[2]=e_get_prox(2);
	current_prox_data[3]=e_get_prox(3);
	current_prox_data[4]=e_get_prox(4);
	current_prox_data[5]=e_get_prox(5);
	current_prox_data[6]=e_get_prox(6);
	current_prox_data[7]=e_get_prox(7);

    // Detect obstacle_present on any of the 8 sensors
	obstacle_present=0;

	for (i=0; i<8; i++)
	{
		if(current_prox_data[i]>1000)
		{
			obstacle_present = 1;
		}
	}

	// Detect if robot upside down
	int x, y, z;
	z = e_get_acc(0);

	if(z < 200)	//LED4 on if e-puck is on the back
	{
		LED0 = 0;
		LED6 = 1;
	}
	else		//LED0 on if e-puck is on his wells
	{
		LED0 = 1;
		LED6 = 0;
	} 

    // if sensors detect obstacle_present return 1
	// robot is upside down return 2
	return obstacle_present;   					
}


//flash LEDs 
void LedFearFlash(void)
{
	e_led_clear();
	int i;
	for ( i = 0; i < 10; i++ )
	{
		e_set_led(i,1);
	//	wait(100000);
		e_set_led(i,0);
	}
}

//turn on all LEDs
void LedFearAll(void)
{
	e_led_clear();
	int i = 0;
	for(i; i < 10; i++)
	{
		e_set_led(i,1);
	}
}

void fear(void)
{
	long i;
	int closest_present;

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

	e_led_clear();

	while(1)
	{
		e_set_speed_left(TURN_SPEED);
		e_set_speed_right(TURN_SPEED);

		while (obstacle_present==0)   	// do this loop until an event has occurred
		{
			obstacle_present = check_prox_sensors(); // check if obstacle present is detected
        }

		// if there is an obstacle present then:
		if(obstacle_present==1)
		{
			e_led_clear();
			//turn on closest LED lights to obstacle position	    
			for (i=0; i<8; i++)
			{
				if(current_prox_data[i]>1000)
				{
					e_set_led(i,1);
					closest_present = i;
				}
			}

        	if(closest_present == 0 || closest_present == 7)
			{
				// Run away fast with fear flash
				LedFearAll();

				// spin to face away from obstacle
				e_set_speed_left(-1800);
				e_set_speed_right(1800);
				wait(680000);

				e_play_sound(11028, 8016);

				// run away
				e_set_speed_left(1800);
				e_set_speed_right(1800);
				wait(600000);

				// spin back round to normal and return to normal speed
				e_set_speed_left(1800);
				e_set_speed_right(-1800);
				wait(725000);

				e_led_clear();
				e_set_speed_left(TURN_SPEED);
				e_set_speed_right(TURN_SPEED);
			}
			else if(closest_present == 3 || closest_present == 4)
			{
				LedFearAll();
				e_play_sound(11028, 8016);

				// run away
				e_set_speed_left(1800);
				e_set_speed_right(1800);
				wait(600000);

				e_led_clear();
				e_set_speed_left(TURN_SPEED);
				e_set_speed_right(TURN_SPEED);
			}
			
			obstacle_present = 0;
   		}
		
		// if robot upside down then:
		if (obstacle_present==2)
		{
			e_set_led(0,1);
			e_set_led(1,1);
			e_set_led(2,1);
			e_set_led(3,1);
			e_set_led(4,1);
			e_set_led(5,1);

			obstacle_present = 0;
			e_led_clear();
		}
	};
}
