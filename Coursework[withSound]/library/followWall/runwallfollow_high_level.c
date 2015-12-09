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
#include "runwallfollow_high_level.h"
#include "../../Sound-motor-basedOnDemo/DemoGCtronic-complete/high_level.h"


#define LEFT_FOLLOW			0		// behaviors IDs	
#define RIGHT_FOLLOW		1 

#define NB_SENSORS          8		// number of sensors
#define BIAS_SPEED      	350		// robot bias speed
#define SENSOR_THRESHOLD	300		// discount sensor noise below threshold
#define MAXSPEED 			800		// maximum robot speed

int follow_sensorzero_hl[8];
int follow_weightleft_hl[8] = {-10,-10,-5,0,0,5,10,10};
int follow_weightright_hl[8] = {10,10,5,0,0,-5,-10,-10};

int corner_prox_data_hl[8];
int corner_present_fw = 0;

/*! \brief Looking at the selector value
 * \return The selector value from 0 to 15
 */
int followgetSelectorValue_hl() {
	return SELECTOR0 + 2*SELECTOR1 + 4*SELECTOR2 + 8*SELECTOR3;
}


/*! \breif Read the sensors proxymities
 * \param sensorTable Where the value must be stocked
 */
void followGetSensorValues_hl(int *sensorTable) {
	unsigned int i;
	for (i=0; i < NB_SENSORS; i++) {
		sensorTable[i] = e_get_calibrated_prox(i); //e_get_prox(i) - follow_sensorzero[i];
	}		
}

/*! \brief Set robot speed */
void followsetSpeed_hl(int LeftSpeed, int RightSpeed) {
	if (LeftSpeed < -MAXSPEED) {LeftSpeed = -MAXSPEED;}
	if (LeftSpeed >  MAXSPEED) {LeftSpeed =  MAXSPEED;}
	if (RightSpeed < -MAXSPEED) {RightSpeed = -MAXSPEED;}
	if (RightSpeed >  MAXSPEED) {RightSpeed =  MAXSPEED;}
	e_set_speed_left(LeftSpeed);
	e_set_speed_right(RightSpeed); 
}

//check front and left or front and right sensors
void check_in_corner_fw( )
{
	int i; 

	// get one single sample for all front, left and right sensors
	corner_prox_data_hl[0]=e_get_prox(1);	//right
	corner_prox_data_hl[1]=e_get_prox(2);	//front-right
	corner_prox_data_hl[2]=e_get_prox(0);	//front
	corner_prox_data_hl[3]=e_get_prox(7);	//front
	corner_prox_data_hl[4]=e_get_prox(5);	//left
	corner_prox_data_hl[5]=e_get_prox(6);	//front-left

    // detect if in corner
	corner_present_fw = 0;

	//check if in left corner
	for (i=2; i<6; i++) 
	{
		if(corner_prox_data_hl[i]>300) 
		{
			++corner_present_fw;
		}
	}

	//check if in left corner
	for (i=0; i<4; i++) 
	{
		if(corner_prox_data_hl[i]>400) 
		{
			++corner_present_fw;
		}
	}
}

/*! \brief The "main" function of the program */
void run_wallfollow_hl() {
	int leftwheel, rightwheel;		// motor speed left and right
	int distances[NB_SENSORS];		// array keeping the distance sensor readings
	int i;							// FOR-loop counters
	int gostraight;
	int loopcount;
	unsigned char selector_change;
	 
	e_init_port();
	e_init_ad_scan(ALL_ADC);
	e_init_motors();
	e_start_agendas_processing();

	//follow_sensor_calibrate();	
	e_activate_agenda(left_led, 2500);
	e_activate_agenda(right_led, 2500);
	e_pause_agenda(left_led);
	e_pause_agenda(right_led);
	
	e_calibrate_ir();
	loopcount=0;
	selector_change = !(followgetSelectorValue_hl() & 0x0001);

	check_in_corner_fw( );

	while (corner_present_fw < 4) {	
		followGetSensorValues_hl(distances); // read sensor values
		check_in_corner_fw( );

		gostraight=0;
		if ((followgetSelectorValue_hl() & 0x0001) == RIGHT_FOLLOW) {
			if(selector_change == LEFT_FOLLOW) {
				selector_change = RIGHT_FOLLOW;
				e_led_clear();
				e_pause_agenda(left_led);
				e_restart_agenda(right_led);
			}  
			for (i=0; i<8; i++) {
				if (distances[i]>50) {break;}
			}
			if (i==8) {
				gostraight=1;
			} else {
				follow_weightleft_hl[0]=-10;
				follow_weightleft_hl[7]=-10;
				follow_weightright_hl[0]=10;
				follow_weightright_hl[7]=10;
				if (distances[2]>300) {
					distances[1]-=200;
					distances[2]-=600;
					distances[3]-=100;
				}
			}
		} else {
			if(selector_change == RIGHT_FOLLOW) {
				selector_change = LEFT_FOLLOW;
				e_led_clear();
				e_pause_agenda(right_led);
				e_restart_agenda(left_led);
			}
			for (i=0; i<8; i++) {
				if (distances[i]>50) {break;}
			}
			if (i==8) {
				gostraight=1;
			} else {
				follow_weightleft_hl[0]=10;
				follow_weightleft_hl[7]=10;
				follow_weightright_hl[0]=-10;
				follow_weightright_hl[7]=-10;
				if (distances[5]>300) {
					distances[4]-=100;
					distances[5]-=600;
					distances[6]-=200;
				}
			}
		}

		leftwheel=BIAS_SPEED;
		rightwheel=BIAS_SPEED;
		if (gostraight==0) {
			for (i=0; i<8; i++) {
				leftwheel+=follow_weightleft_hl[i]*(distances[i]>>4);
				rightwheel+=follow_weightright_hl[i]*(distances[i]>>4);
			}
		}

		// set robot speed
		followsetSpeed_hl(leftwheel, rightwheel);

		wait(15000);
	}

	
	e_destroy_agenda(left_led);
	e_destroy_agenda(right_led);	
}
