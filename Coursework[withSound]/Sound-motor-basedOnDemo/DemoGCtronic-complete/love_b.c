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

#include "love_b.h"



char gbuffer_Love[160];
int gnumbuffer_Love[80];
long isRedVisable;
int currentProxData[8];
int obstaclePresentFar;
int obstaclePresentClose;


//custom cam picture load
void takeImage_Love(){
	e_poxxxx_launch_capture((char *)gbuffer_Love);
	while (!e_poxxxx_is_img_ready()){};
}
// Image processing removes useless information
void processImage_Love(){
	long i;
	int green, red, vis;
	for (i = 0; i<80; i++){
		//RGB turned into an integer value for comparison
		red = (gbuffer_Love[2 * i] & 0xF8);
		green = (((gbuffer_Love[2 * i] & 0x07) << 5) | ((gbuffer_Love[2 * i + 1] & 0xE0) >> 3));
		//blue = ((gfbwbuffer[2*i+1] & 0x1F) << 3); we don't need blue for looking for green.
		if (green > red + 50){ //Green is usually much higher then red due the the extra bit place in RGB565
			gnumbuffer_Love[i] = 1;
			vis += 1;
		}
		else{
			gnumbuffer_Love[i] = 0;
		}
		//If Green is visable then isRedVisable turns to true
		if (vis>0){
			isRedVisable = 1;
		}
		else{
			isRedVisable = 0;
		}
	}
}

//Decide which way to turn based on the number of true pixels.
int gturnDirection_Love(){
	int sumL = 0;
	int sumR = 0;
	int i;
	for (i = 0; i<40; i++){
		sumL += gnumbuffer_Love[i];
		sumR += gnumbuffer_Love[i + 40];
	}
	if (sumL<sumR){
		return 1;
	}
	else{
		return 0;
	}
}
//Function to deal with turning.
void gturn_Love(void) {
	if (gturnDirection_Love()){
		e_set_speed_left(300);
		e_set_speed_right(-300);
	}
	else{
		e_set_speed_left(-300);
		e_set_speed_right(300);
	}
}
void forward_Love(void){
	e_set_speed_left(800);
	e_set_speed_right(800);
}

void forwardSlow_Love(void)
{
	e_set_speed_left(200);
	e_set_speed_right(200);
}

void stop_Love(void){
	e_set_speed_left(0);
	e_set_speed_right(0);
}

void setUpCamera_Love(void){
	e_poxxxx_init_cam();
	e_poxxxx_config_cam(0, (ARRAY_HEIGHT - 4) / 2, 640, 4, 8, 4, RGB_565_MODE);
	e_poxxxx_set_mirror(1, 1);
	e_poxxxx_write_cam_registers();
}

int greenIsInMiddle_Love(int centreValue){
	if (centreValue > 3)
		return 1;
	return 0;
}

int checkProxSensorsFar_Love(void)
{

	// get one single sample for all 8 sensors
	currentProxData[0] = e_get_prox(0);
	currentProxData[1] = e_get_prox(1);
	currentProxData[2] = e_get_prox(2);
	currentProxData[3] = e_get_prox(3);
	currentProxData[4] = e_get_prox(4);
	currentProxData[5] = e_get_prox(5);
	currentProxData[6] = e_get_prox(6);
	currentProxData[7] = e_get_prox(7);

	// Detect obstacle_present on any of the 8 sensors
	obstaclePresentFar = 0;

		if (currentProxData[0]>150 || currentProxData[7]>150)
		{
			obstaclePresentFar = 1;
		}
	return obstaclePresentFar;
}

int checkProxSensorsClose_Love(void)
{

	// get one single sample for all 8 sensors
	currentProxData[0] = e_get_prox(0);
	currentProxData[1] = e_get_prox(7);

	// Detect obstacle_present on any of the 8 sensors
	obstaclePresentClose = 0;

	if (currentProxData[0]>300 || currentProxData[7]>300)
	{
		obstaclePresentClose = 1;
	}
	return obstaclePresentClose;
}

void Kiss_Love(void)
{
		e_set_speed_left(0);
		e_set_speed_right(0);

	while(checkProxSensorsClose_Love() == 1)
	{
		//kiss sequence
		int i;
		for (i=0; i<8; i++)
		{
			if(currentProxData[i]>500)
			{
				e_set_led(i,1);
				
				//e_set_led(0,1);
				//e_set_led(7,1);
			}
		}
		
		//sound
		e_play_sound(0, 2112);
	}
}

//Main function of follower
void Love_b(void)
{
	e_init_sound();
	e_play_sound(0, 2112);
/*
	setUpCamera_Love();
	e_start_agendas_processing();
	int centreValue;
	int foundRed = 0;

	while (!foundRed)
	{
		takeImage_Love();
		processImage_Love();
		//Take a section of the center, this means if there is an error with one it won't effect it as a whole.
		centreValue = gnumbuffer_Love[38] + gnumbuffer_Love[39] + gnumbuffer_Love[40] + gnumbuffer_Love[41] + gnumbuffer_Love[42] + gnumbuffer_Love[43]; // removes stray 
		//If green is in the middle then it will go forward 
		if (centreValue > 3)
		{
			foundRed = 1;
			e_destroy_agenda(gturn_Love);
		}
		else if (isRedVisable == 1)
		{//If green isn't in the center but is visable then picks a direction to turn to face it
			e_activate_agenda(gturn_Love, 650);
		}
		else
		{
			e_destroy_agenda(gturn_Love);
			e_set_speed_left(0);
			e_set_speed_right(0);
		}
	}
	while (foundRed)
	{
		forward_Love();

		while (obstaclePresentFar == 0)
		{
			obstaclePresentFar = checkProxSensorsFar_Love();
		}

		if (obstaclePresentFar == 1)
		{
			forwardSlow_Love();

			while (obstaclePresentClose == 0)
			{
				obstaclePresentClose = checkProxSensorsClose_Love();
			}

			if (obstaclePresentClose == 1)
			{
			//	run_wallfollow();
				foundRed = 0;
				
			//	Kiss();
				e_set_led(0,1);
				e_set_led(7,1);
				e_play_sound(0, 2112);
			}
		}

	}
*/
}

