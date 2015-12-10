/********************************************************************************

			Programm to follow a wall								      
			Version 1.0 août 2007				                          
			Michael Bonani, Jonathan Besuchet


This file is part of the e-puck library license.
See http://www.e-puck.org/index.php?option=com_content&task=view&id=18&Itemid=45

(c) 2004-2007 Michael Bonani, Jonathan Besuchet

Robotics system laboratory http://lsro.epfl.ch
Laboratory of intelligent systems http://lis.epfl.ch
Swarm intelligent systems group http://swis.epfl.ch
EPFL Ecole polytechnique federale de Lausanne http://www.epfl.ch

**********************************************************************************/

/*! \file
 * \brief Follow a wall
 * \section sect1 Introduction
 * With this program, the e-puck will follow a wall.
 * 
 * \section sect_sound2 Playing the demo
 * First of all, move the selector to the position 2 and reset the e-puck.
 * \n The e-puck will now follow the first wall he finds. You can change
 * the side on which the e-puck must follow the wall with the selector.
 *
 * \section sect_sound3 Video of the demo
 * The video of this demo: http://www.youtube.com/watch?v=xaqpoQ_XGbU
 *
 * \author Code: Michael Bonani, Jonathan Besuchet \n Doc: Jonathan Besuchet
 */

#include "p30f6014A.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "../motor_led/e_epuck_ports.h"
#include "../motor_led/e_init_port.h"
#include "../motor_led/advance_one_timer/e_motors.h"
#include "../motor_led/advance_one_timer/e_led.h"
#include "../motor_led/advance_one_timer/e_agenda.h"
#include "../uart/e_uart_char.h"
#include "../a_d/advance_ad_scan/e_ad_conv.h"
#include "../a_d/advance_ad_scan/e_prox.h"
#include "../camera/fast_2_timer/e_poxxxx.h"

#include "../../Other Libraries/utility.h"
#include "runwallfollow.h"


#define LEFT_FOLLOW			0		// behaviors IDs	
#define RIGHT_FOLLOW		1 

#define NB_SENSORS          8		// number of sensors
#define BIAS_SPEED      	350		// robot bias speed
#define SENSOR_THRESHOLD	300		// discount sensor noise below threshold
#define MAXSPEED 			800		// maximum robot speed


int follow_sensorzero[8];
int follow_weightleft[8] = {-10,-10,-5,0,0,5,10,10};
int follow_weightright[8] = {10,10,5,0,0,-5,-10,-10};


int followgetSelectorValue() 
{
	if(e_get_prox(0) > e_get_prox(7))
	{
		return 1;	
	}
	return 0;
}


/*! \breif Read the sensors proxymities
 * \param sensorTable Where the value must be stocked
 */
void followGetSensorValues(int *sensorTable) {
	unsigned int i;
	for (i=0; i < NB_SENSORS; i++) {
		sensorTable[i] = e_get_calibrated_prox(i); //e_get_prox(i) - follow_sensorzero[i];
	}		
}

int doesWallExist(int selectionValue, int *distances)
{
	if(selectionValue == 1)
	{
		if(((e_get_prox(1) < 100) && (e_get_prox(2) < 150) && (e_get_prox(3) < 100)) && e_get_prox(0) < 100)
		{
			return 0;
		}
	}
	else
	{
		if(((e_get_prox(5) < 150) && (e_get_prox(6) < 100) && (e_get_prox(4) < 100) ) && e_get_prox(0) < 100)
		{
			return 0;
		}
	}
	return 1;
}

/*! \brief Set robot speed */
void followsetSpeed(int LeftSpeed, int RightSpeed) {
	if (LeftSpeed < -MAXSPEED) {LeftSpeed = -MAXSPEED;}
	if (LeftSpeed >  MAXSPEED) {LeftSpeed =  MAXSPEED;}
	if (RightSpeed < -MAXSPEED) {RightSpeed = -MAXSPEED;}
	if (RightSpeed >  MAXSPEED) {RightSpeed =  MAXSPEED;}
	e_set_speed_left(LeftSpeed);
	e_set_speed_right(RightSpeed); 
}

/*! \brief The "main" function of the program */
void run_wallfollow() {
	int leftwheel, rightwheel;		// motor speed left and right
	int distances[NB_SENSORS];		// array keeping the distance sensor readings
	int i;							// FOR-loop counters
	int gostraight;
	int loopcount;
	int selectionValue;
	unsigned char selector_change;
	 
	e_init_port();
	e_init_uart1();
  	e_send_uart1_char("\f\a", 2);

	e_init_ad_scan(ALL_ADC);
	e_init_motors();
	e_start_agendas_processing();
	
	e_calibrate_ir();

	e_set_steps_right(0);

	selectionValue = followgetSelectorValue();
	followGetSensorValues(distances);

	while (doesWallExist(selectionValue, distances)) 
	{
		followGetSensorValues(distances); // read sensor values

		e_start_led_blinking(2500);
		gostraight=0;

		if (selectionValue == 1) {
			for (i=0; i<8; i++) 
			{
				if (distances[i]>50) 
				{
					break;
				}
			}

			if (i==8) 
			{
				gostraight=1;
			} 
			else 
			{
				follow_weightleft[0]=-10;
				follow_weightleft[7]=-10;
				follow_weightright[0]=10;
				follow_weightright[7]=10;

				if (distances[2]>300) 
				{
					distances[1]-=200;
					distances[2]-=600;
					distances[3]-=100;
				}
			}
		} 
		else
		{
			for (i=0; i<8; i++) 
			{
				if (distances[i]>50) 
				{
					break;
				}
			}

			if (i==8) 
			{
				gostraight=1;
			}
			else 
			{
				follow_weightleft[0]=10;
				follow_weightleft[7]=10;
				follow_weightright[0]=-10;
				follow_weightright[7]=-10;

				if (distances[5]>300) 
				{
					distances[4]-=100;
					distances[5]-=600;
					distances[6]-=200;
				}
			}
		}

		leftwheel=BIAS_SPEED;
		rightwheel=BIAS_SPEED;

		if (gostraight==0) 
		{
			for (i=0; i<8; i++) 
			{
				leftwheel+=follow_weightleft[i]*(distances[i]>>4);
				rightwheel+=follow_weightright[i]*(distances[i]>>4);
			}
		}

		// set robot speed
		followsetSpeed(leftwheel, rightwheel);

		wait(15000);
	}	
}
