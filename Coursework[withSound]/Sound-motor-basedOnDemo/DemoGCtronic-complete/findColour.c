

#include <motor_led/e_epuck_ports.h>
#include <motor_led/e_init_port.h>
#include <motor_led/advance_one_timer/e_motors.h>
#include <motor_led/advance_one_timer/e_led.h>
#include <motor_led/advance_one_timer/e_agenda.h>
#include <uart/e_uart_char.h>
#include <a_d/advance_ad_scan/e_ad_conv.h>
#include <a_d/advance_ad_scan/e_prox.h>
#include <camera/fast_2_timer/e_poxxxx.h>

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#include "./findColour.h"

char fbwbuffer[160];
int numbuffer[80];
int isColourVisable;
char colour;

void setColour(char colourIn)
{
	colour = colourIn;
}

//custom cam picture load
void getImage()
{
	e_poxxxx_launch_capture((char *)fbwbuffer);
    while(!e_poxxxx_is_img_ready()){};
}
// Image processing removes useless information
void Image()
{	
	long i;
	int green, red, blue;
	int vis = 0;

	switch(colour)
	{
		case 'r':
		{
			for(i=0; i<80; i++){
				//RGB turned into an integer value for comparison
				red = (fbwbuffer[2*i] & 0xF8);
				green = (((fbwbuffer[2*i] & 0x07) << 5) | ((fbwbuffer[2*i+1] & 0xE0) >> 3));
				blue = ((fbwbuffer[2*i+1] & 0x1F) << 3);
				
				if(red > green + 20){ // green will be less then red when red is strong.
					numbuffer[i] = 1;
					vis++;
				}else{
					numbuffer[i] = 0;
				}
				//If colour is visable then isColourVisable turns to true
				if(vis>0){
					isColourVisable = 1;
				}else{
					isColourVisable = 0;
				}
			}
			break;
		}
		case 'g':
		{
			for(i=0; i<80; i++){
				//RGB turned into an integer value for comparison
				red = (fbwbuffer[2*i] & 0xF8);
				green = (((fbwbuffer[2*i] & 0x07) << 5) | ((fbwbuffer[2*i+1] & 0xE0) >> 3));
				blue = ((fbwbuffer[2*i+1] & 0x1F) << 3);
				
				if(green > red + 20){
					numbuffer[i] = 1;
					vis++;
				}else{
					numbuffer[i] = 0;
				}
				if(vis>0){
					isColourVisable = 1;
				}else{
					isColourVisable = 0;
				}
			}
			break;
		}
		case 'b':
		{
			for(i=0; i<80; i++){
				//RGB turned into an integer value for comparison
				red = (fbwbuffer[2*i] & 0xF8);
				green = (((fbwbuffer[2*i] & 0x07) << 5) | ((fbwbuffer[2*i+1] & 0xE0) >> 3));
				blue = ((fbwbuffer[2*i+1] & 0x1F) << 3);
				
				if(blue > red + 20){
					numbuffer[i] = 1;
					vis++;
				}else{
					numbuffer[i] = 0;
				}
				if(vis>0){
					isColourVisable = 1;
				}else{
					isColourVisable = 0;
				}
			}
			break;
		}
	}		
}

int isColourVis()
{
	//basic set up for camera
	e_poxxxx_init_cam();
	e_poxxxx_config_cam(0,(ARRAY_HEIGHT - 4)/2,640,4,8,4,RGB_565_MODE);
	e_poxxxx_set_mirror(1,1);
	e_poxxxx_write_cam_registers(); 

	e_start_agendas_processing();
	
	getImage();
	Image();

	return isColourVisable;		
}
//Decide which way to turn.
int turnDirection()
{
	int sumL = 0;
	int sumR = 0;
	int i;
	for(i=0;i<40;i++){
		sumL += numbuffer[i];
		sumR += numbuffer[i+40];
	}
	if(sumL<sumR){ 
		return 1;
	}else{
		return 0;
	}
}
//Function to deal with turning.
void turn(void)
{
	if(turnDirection()){
		e_set_speed_left (300);
		e_set_speed_right(-300);
	}else{
		e_set_speed_left (-300);
		e_set_speed_right(300);
	}
}
//Main function of follower
void findColour()
{
	//basic set up for camera
	e_poxxxx_init_cam();
	e_poxxxx_config_cam(0,(ARRAY_HEIGHT - 4)/2,640,4,8,4,RGB_565_MODE);
	e_poxxxx_set_mirror(1,1);
	e_poxxxx_write_cam_registers(); 

	e_start_agendas_processing();
	int centreValue = 0;

	while(centreValue < 3)
	{
		getImage();
		Image();
		//Take a section of the center, this means if there is an error with one it won't effect it as a whole.
		centreValue = numbuffer[38] + numbuffer[39] + numbuffer[40] + numbuffer[41] + numbuffer[42] + numbuffer[43]; // removes stray 
		if(centreValue > 3)
		{
			//If colour is in the middle then stay still
			e_destroy_agenda(turn);
			//e_set_speed_left (0);
			//e_set_speed_right(0);
		}
		else if(isColourVisable)
		{
			//If colour isn't in the center but is visable then picks a direction to turn to face it
			e_activate_agenda(turn, 650);
		}
		else
		{
			e_destroy_agenda(turn);
			//e_set_speed_left (300);
			//e_set_speed_right(0);
			break;
		}
	}
}

