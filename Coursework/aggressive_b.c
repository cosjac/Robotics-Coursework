/**
 * aggressive Behaviour
 **/

#include "p30f6014A.h"
#include <math.h>
#include "library/motor_led/advance_one_timer/e_agenda.h"
#include "library/motor_led/advance_one_timer/e_motors.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "library/motor_led/e_epuck_ports.h"
#include "library/motor_led/e_init_port.h"
#include "library/motor_led/advance_one_timer/e_led.h"
#include "library/uart/e_uart_char.h"
#include "library/a_d/advance_ad_scan/e_prox.h"
#include "library/a_d/advance_ad_scan/e_ad_conv.h"

#include "aggressive_b.h"

#define PI 3.1415


/* defines used in turn_to_direction.c */
#define TURN_SPEED 1000
#define STEPS_FOR_2PI 1300.




int check_for_event1(void);
float calculate_direction_led(void);
void turn_to_direction1(float direction);
int current_samples2[8];
char buffer[80];

/*
 Check for event

 Checks if an event has happened. This function dynamically 
 takes the average of the soundstream and then checks, whether
 it is tresspassing a predefined treshold.
 
 in:  void
 out: int (1: no event has occured)
          (0: an event has occured!)
*/

int check_for_event1(void)
{
	int event; 
	int i; 
	// get one single sample for all 8 sensors
	current_samples2[0]=e_get_prox(0);
	current_samples2[1]=e_get_prox(1);
	current_samples2[2]=e_get_prox(2);
	current_samples2[3]=e_get_prox(3);
	current_samples2[4]=e_get_prox(4);
	current_samples2[5]=e_get_prox(5);
	current_samples2[6]=e_get_prox(6);
	current_samples2[7]=e_get_prox(7);
    // Detect event on any of the 8 sensors
	event=0;

	for (i=0; i<8; i++) {
		if(current_samples2[i]>350) {
			event = 1;
		}
	}
    // if sensors detect close object return 1
	return event;   					
}

// Calculate direction of object and turn corresponding LED on
float calculate_direction_led(void)
{
	float direction;
	int led_nb = 6;
	long int i;	
	
	if ( current_samples2[0]>350 )
		{direction=(PI/8.0);
		led_nb = 7;
		}
	if ( current_samples2[1]>350 )
		{direction=(2.0*PI/8.0);
		led_nb = 9;
		}
	if ( current_samples2[2]>350 )
		{direction=(4.0*PI/8.0);
		led_nb = 12;
		}
	if ( current_samples2[3]>350 )
		{direction=(6.0*PI/8.0);
		led_nb = 10;
		}
	if ( current_samples2[4]>350 )
		{direction=(10.0*PI/8.0);
		led_nb = 13;
		}
	if ( current_samples2[5]>350 )
		{direction=(12.0*PI/8.0);
		led_nb = 14;
		}
	if ( current_samples2[6]>350 )
		{direction=(14.0*PI/8.0);
		led_nb = 15;
		}
	if ( current_samples2[7]>350 )
		{direction=(15.0*PI/8.0);
		led_nb = 6;
		}

// set the bit on PortA to illuminate the led
	LATA = 1 << led_nb;
	
	for (i=0;i<500000;i++);   // Wait to indicate the direction
	
	LATA = 0;	// turn all LEDs off

	return direction;
}
	

/*
 Turn to direction

 Turns the robot to the appropriate direction

 in:  float (angle between 0 and 2PI clockwise)
 out: void
*/
void turn_to_direction1(float direction)
{
	int end_turn;
		
		if (direction < PI)     // turn clockwise
		{
			e_set_steps_left(0);
			e_set_speed_left(TURN_SPEED);  // motor speed in steps/s
			e_set_speed_right(-TURN_SPEED);  // motor speed in steps/s
			
			// calculate how many steps the robot needs to turn
			end_turn = (int)(STEPS_FOR_2PI*(direction/(2*PI)));   
			while(e_get_steps_left() < end_turn);   // turn until done 
		}
		else 					// turn counterclockwise
		{
			e_set_steps_right(0);
			e_set_speed_left(-TURN_SPEED);  // motor speed in steps/s
			e_set_speed_right(TURN_SPEED);  // motor speed in steps/s
			
			// calculate how many steps the robot needs to turn
			end_turn = (int)(STEPS_FOR_2PI*((2*PI-direction)/(2*PI)));
			while(e_get_steps_right() < end_turn);   // turn until done
		}

		// stop motors
		e_set_speed_left(0);  // motor speed in steps/s
		e_set_speed_right(0);  // motor speed in steps/s
}
//Main function of fear behaviour
void aggressive_behaviour() {
	int event;
	float direction;
	long i;

 
    e_init_motors();
    e_start_agendas_processing();
	e_init_ad_scan(ALL_ADC);
	e_init_port();

	while(1) {
		event = 0;
		while (event==0)   	// do this loop until an event has occurred
		{
			event = check_for_event1(); // check if attacker is detected
        }      
		// if there is an event then:
		if (event==1) {

    	    direction = calculate_direction_led();   // do all the calculations where the attack is coming from
         	turn_to_direction1(direction); // turn the robot to the direction the attack is coming from
			e_set_steps_right(0);

			e_set_speed_left(TURN_SPEED);  // motor speed in steps/s
			e_set_speed_right(TURN_SPEED);  // motor speed in steps/s//ram
			
			for(i=0;i<40000;i++) {asm("nop");} //attack for amount of time
	    	
			e_set_speed_left(0);  // stop motors
			e_set_speed_right(0);
        
   		}        	
	}								
}

