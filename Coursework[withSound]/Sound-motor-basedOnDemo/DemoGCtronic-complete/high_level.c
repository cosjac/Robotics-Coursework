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
#include <followWall/runwallfollow_high_level.h>

#include "high_level.h"
#include "findColour.h"

int current_highprox_data[8], corner_prox_data[8];
int obstacle_highpresent, isSelectedColourVisable = 0;
int corner_present = 0;
int random_number, random_number_mod;
char hlbuffer[160];
int hlnumbuffer[80];

#define NORMAL_SPEED 700	
#define FAST_SPEED 1200
#define SLOW_SPEED 350



//custom cam picture load
void hltakeImage()
{
	e_poxxxx_launch_capture((char *)hlbuffer);
    while(!e_poxxxx_is_img_ready()){};
}

// Image processing removes useless information
void hlprocessImage(char colourIn)
{	
	long i;
	int green, red;
	int vis = 0;
		
	switch(colourIn)
	{
		//find colour red
		case 'r':
		{
			for(i=0; i<80; i++)
			{
				//RGB turned into an integer value for comparison
				red = (hlbuffer[2*i] & 0xF8);
				green = (((hlbuffer[2*i] & 0x07) << 5) | ((hlbuffer[2*i+1] & 0xE0) >> 3));
				
				if(red > green + 20)
				{ // green will be less then red when red is strong.
					hlnumbuffer[i] = 1;
					vis++;
				}
				else
				{
					hlnumbuffer[i] = 0;
				}
				//If colour is visable then isColourVisable turns to true
				if(vis>0)
				{
					isSelectedColourVisable = 1;
				}
				else
				{
					isSelectedColourVisable = 0;
				}
			}
			break;
		}
		//find colour green
		case 'g':
		{
			for(i=0; i<80; i++)
			{
				//RGB turned into an integer value for comparison
				red = (hlbuffer[2*i] & 0xF8);
				green = (((hlbuffer[2*i] & 0x07) << 5) | ((hlbuffer[2*i+1] & 0xE0) >> 3));
				
				if(green > red + 20)
				{
					hlnumbuffer[i] = 1;
					vis++;
				}
				else
				{
					hlnumbuffer[i] = 0;
				}
				if(vis>0)
				{
					isSelectedColourVisable = 1;
				}
				else
				{
					isSelectedColourVisable = 0;
				}
			}
			break;
		}
	}		
}

//Decide which way to turn based on the number of true pixels.
int hlturnDirection()
{
	int sumL = 0;
	int sumR = 0;
	int i;
	for(i=0;i<40;i++)
	{
		sumL += hlnumbuffer[i];
		sumR += hlnumbuffer[i+40];
	}
	if(sumL<sumR)
	{ 
		return 1;
	}
	else
	{
		return 0;
	}
}

//Function to deal with turning.
void hlturn(void) 
{
	if(hlturnDirection())
	{
		//turn right
		e_set_speed_left (SLOW_SPEED);
		e_set_speed_right(-SLOW_SPEED);
	}
	else
	{
		//turn left
		e_set_speed_left (-SLOW_SPEED);
		e_set_speed_right(SLOW_SPEED);
	}
}

//check front prox sensors
int check_highprox_sensors(void)
{
	int i; 

	// get one single sample for all front sensors
	current_highprox_data[0]=e_get_prox(0);
	current_highprox_data[1]=e_get_prox(1);
	current_highprox_data[2]=e_get_prox(6);
	current_highprox_data[3]=e_get_prox(7);

    // Detect obstacle_present on any of the front sensors
	obstacle_highpresent = 0;

	for (i=0; i<4; i++) 
	{
		if(current_highprox_data[i]>500) 
		{
			obstacle_highpresent = 1;
		}
	}
	return obstacle_highpresent;   					
}

//move forwards for random amount of time, then turn left or right randomly for random amount of time
void random_roam( ) 
{
	e_destroy_agenda(hlturn);

	e_set_speed_left(SLOW_SPEED);
	e_set_speed_right(SLOW_SPEED);

	random_number =  rand() % 100;         // random_number in the range 0 to 99;	
	random_number = random_number * 10000;

	wait( ( random_number * 8 ) );
	obstacle_highpresent = check_highprox_sensors(); // check if obstacle present is detected

	random_number_mod = rand() % 2; // random_number_mod in the range 0 to 1;

	switch ( random_number_mod ) 
	{
		case 0:
			e_set_speed_left(SLOW_SPEED);
			e_set_speed_right(-SLOW_SPEED);
			break;
		default:
			e_set_speed_left(-SLOW_SPEED);
			e_set_speed_right(SLOW_SPEED);
			break;
	}
	
	wait( random_number * 2 );
}

//rotate left until no obstacle present
void avoid_obstacle( ) 
{
	e_set_speed_left(-NORMAL_SPEED);
	e_set_speed_right(NORMAL_SPEED);
}

//function to face e-puck bum in corner
void turn_to_corner( )
{
	printf("corner present %d", corner_present);
	check_in_corner( );
	check_highprox_sensors(); // check if obstacle present is detected	
	while( obstacle_highpresent == 1 ) 
	{
		//check for obstacle
		check_highprox_sensors(); // check if obstacle present is detected				
		avoid_obstacle( );
	}
}

//check front and left or front and right sensors
void check_in_corner( )
{
	int i; 

	// get one single sample for all front, left and right sensors
	corner_prox_data[0]=e_get_prox(1);	//right
	corner_prox_data[1]=e_get_prox(2);	//front-right
	corner_prox_data[2]=e_get_prox(0);	//front
	corner_prox_data[3]=e_get_prox(7);	//front
	corner_prox_data[4]=e_get_prox(5);	//left
	corner_prox_data[5]=e_get_prox(6);	//front-left

    // detect if in corner
	corner_present = 0;

	//check if in left corner
	for (i=2; i<6; i++) 
	{
		if(corner_prox_data[i]>300) 
		{
			++corner_present;
		}
	}

	//check if in left corner
	for (i=0; i<4; i++) 
	{
		if(corner_prox_data[i]>300) 
		{
			++corner_present;
		}
	}
}

void forwardhl( )
{
	e_set_speed_left(NORMAL_SPEED);
	e_set_speed_right(NORMAL_SPEED);
}


//-------------
//main function
//-------------
void high_level(int hiding) 
{
	int centreValue = 0;
	int hidden = 0;
	int in_corner = 0;
	int prox_at_corner = 0;
	int colourVisible = 0;
	int not_found = 1;
	int turn_left = 1;
	int pause = 0;
	
	//configure robot
	e_init_port();    // configure port pins   
	e_init_ad_scan(ALL_ADC);

	e_init_uart1();
	e_send_uart1_char("\f\a", 2);

    e_init_motors();
	e_acc_calibr();
	e_calibrate_ir();   
	e_start_agendas_processing();

	//basic set up for camera
	e_poxxxx_init_cam();
	e_poxxxx_config_cam(0,(ARRAY_HEIGHT - 4)/2,640,4,8,4,RGB_565_MODE);
	e_poxxxx_set_mirror(1,1);
	e_poxxxx_write_cam_registers(); 

	e_led_clear();

	while(1) 
	{

		//if hiding, find hidden location, stay still until see red robot
		while(hiding) 
		{

			//find hidden location of a corner
			while(!hidden || colourVisible)
			{
				check_in_corner( );
				while (corner_present < 4)
 				{	
					forwardhl( );
					turn_to_corner( );
				}

				hidden = 1;
				colourVisible = 0;
				//when in order, set hidden to 1
			}

			e_set_speed_left(0);
			e_set_speed_right(0);

			e_led_clear();
			
			//while not in corner, move closer to corner
			while( (e_get_prox(0)<1000 || e_get_prox(7)<1000) && !in_corner)
			{
				e_set_speed_left(NORMAL_SPEED);
				e_set_speed_right(NORMAL_SPEED);
			} 

			turn_to_corner( );
	
			//then stop motors
			e_set_speed_left(0);
			e_set_speed_right(0); 

			//found corner and update value
			in_corner = 1;

			hidden = 1;
			turn_left = 0;
			colourVisible = 0;

			while(hidden)
			{
				e_led_clear();
				e_set_speed_left(-SLOW_SPEED);
				e_set_speed_right(SLOW_SPEED);


				//turn left
				while(!turn_left && (e_get_prox(5)<300 || e_get_prox(6)<300) && !colourVisible )
				{
		
					//turn left 90 degrees
					e_set_speed_left(-SLOW_SPEED);
					e_set_speed_right(SLOW_SPEED);
	
					hltakeImage();
					hlprocessImage('r');
					//Take a section of the center, this means if there is an error with one it won't effect it as a whole.
					centreValue = hlnumbuffer[38] + hlnumbuffer[39] + hlnumbuffer[40] + hlnumbuffer[41] + hlnumbuffer[42] + hlnumbuffer[43]; // removes stray 
			
					if(centreValue > 3)
					{ //If red is in the middle then it will go forward 
					
						e_set_speed_left(0);
						e_set_speed_right(0);
	
						if((e_get_prox(0) >75) && (e_get_prox(7) > 150))
						{
							//update to found and follow wall until hidden
							e_set_speed_left(NORMAL_SPEED);
							e_set_speed_right(-NORMAL_SPEED);
							wait(300000);
							colourVisible = 1;
						}
					}
				}
				turn_left = 1;
				e_led_clear();
	
				//turn left
				while(turn_left && (e_get_prox(1)<300 || e_get_prox(2)<300) && !colourVisible)
				{
	
					//turn left 90 degrees
					e_set_speed_left(SLOW_SPEED);
					e_set_speed_right(-SLOW_SPEED);
	
					hltakeImage();
					hlprocessImage('r');
					//Take a section of the center, this means if there is an error with one it won't effect it as a whole.
					centreValue = hlnumbuffer[38] + hlnumbuffer[39] + hlnumbuffer[40] + hlnumbuffer[41] + hlnumbuffer[42] + hlnumbuffer[43]; // removes stray 
			
					if(centreValue > 3)
					{ //If red is in the middle then it will go forward 
						
						e_set_speed_left(0);
						e_set_speed_right(0);
	
						if((e_get_prox(0) >75) && (e_get_prox(7) > 150))
						{
							//update to found and follow wall until hidden
							e_set_speed_left(-NORMAL_SPEED);
							e_set_speed_right(NORMAL_SPEED);
							wait(300000);
							colourVisible = 1;
						}
					}
				}
				turn_left = 0;
	
				while( colourVisible )
				{
					e_set_speed_left(0);
					e_set_speed_right(0);
					e_set_led(1,1);
					e_set_led(2,1);
					e_set_led(3,1);
					e_set_led(4,1);
					e_set_led(5,1);
					e_set_led(6,1);
					e_set_led(7,1);
					colourVisible = 0;
					hidden = 0;
				}
			}


		}



		//if NOT hiding, find hidden robot (green)
		while(hiding == 0) 
		{
			//clear LEDs 
			e_led_clear();
			check_highprox_sensors(); // check if obstacle present is detected	
	
			//while not see green, roam and avoid obstacles
			while(centreValue <= 3 || isSelectedColourVisable == 0)
			{
				pause = 0;	
				//check if see green/red
				hltakeImage();
			
				hlprocessImage('g');
			
				centreValue = hlnumbuffer[38] + hlnumbuffer[39] + hlnumbuffer[40] + hlnumbuffer[41] + hlnumbuffer[42] + hlnumbuffer[43]; // removes stray 
		
				e_destroy_agenda(hlturn);
				random_roam( );
				
				check_highprox_sensors(); // check if obstacle present is detected	
				while( obstacle_highpresent == 1 ) 
				{
					//check for obstacle
					check_highprox_sensors(); // check if obstacle present is detected				
					avoid_obstacle( );
				}
			}
	
			e_led_clear();
		

			//while see green, move forwards towards green
			while(centreValue > 3 || isSelectedColourVisable == 1)
			{
				e_led_clear();
				printf("pause flag: %d //n", pause);
				//turn on all LEDs when found hiding robot
				e_set_led(2,1);
				e_set_led(3,1);
				e_set_led(4,1);
				e_set_led(5,1);
				e_set_led(6,1);
				e_set_led(7,1);
	
				//check if see green/red
				hltakeImage();
			
				hlprocessImage('g');
			
				centreValue = hlnumbuffer[38] + hlnumbuffer[39] + hlnumbuffer[40] + hlnumbuffer[41] + hlnumbuffer[42] + hlnumbuffer[43]; // removes stray 
			
				if(centreValue > 3)
				{ //If green is in the middle then it will go forward 
					e_set_led(1,1);
					
					e_destroy_agenda(hlturn);

					printf("prox0 %d", e_get_prox(0));

					if(e_get_prox(0) > 70 && e_get_prox(7) > 70)
					{	
						pause = 1;
						printf("Found object pause flag: %d //n", pause);
					} 
					else 
					{
						e_set_speed_left(SLOW_SPEED);
						e_set_speed_right(SLOW_SPEED);
					}

				}
				else if(isSelectedColourVisable == 1 && pause==0)
				{//If green isn't in the center but is visable then picks a direction to turn to face it
					e_activate_agenda(hlturn, 650);
				} else if (pause == 1) 
				{
					e_set_speed_left(0);
					e_set_speed_right(0);
						
					while(e_get_prox(0) > 70 && e_get_prox(7) > 70){
						wait(15000);
					}
					printf("Set speed to 0 \n\n");	
				}
			}
		}
	};
}