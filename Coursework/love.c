#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "library/motor_led/e_init_port.h"
#include "library/motor_led/advance_one_timer/e_motors.h"
#include "library/motor_led/advance_one_timer/e_led.h"
#include "library/motor_led/advance_one_timer/e_agenda.h"
#include "library/uart/e_uart_char.h"
#include "library/a_d/advance_ad_scan/e_ad_conv.h"
#include "library/a_d/advance_ad_scan/e_prox.h"
#include "library/codec/e_sound.h"
#include "library/motor_led/e_epuck_ports.h"
#include "library/camera/fast_2_timer/e_poxxxx.h"

#include "love.h"
#include "curious.h"
#include "findColour.h"

char rbuffer[160];
int rnumbuffer[80];


//custom cam picture load
void rgetImage(){
	e_poxxxx_launch_capture((char *)rbuffer);
    while(!e_poxxxx_is_img_ready()){};
}

//Function to turn the Puck
int rturnDirection(){
	int sumL = 0;
	int sumR = 0;
	int i;
	for(i=0;i<40;i++){
		sumL += rnumbuffer[i];
		sumR += rnumbuffer[i+40];
	}
	if(sumL<sumR){ 
		return 1;
	}else{
		return 0;
	}
}
//Function to deal with turning.
void rturn(void) {
	if(rturnDirection()){
		e_set_speed_left (500);
		e_set_speed_right(-500);
	}else{
		e_set_speed_left (-500);
		e_set_speed_right(500);
	}
}

//Function to make the bot move forward.
void rforward(void){
	e_set_speed_left (500);
	e_set_speed_right(500);
}

//Function to process image
void processImageRed(){	
	long i;
	int isRedVisable = 0;
	int green, red, vis;
	for(i=0; i<80; i++){
		//RGB turned into an integer value for comparison
		red = (rbuffer[2*i] & 0xF8);
		green = (((rbuffer[2*i] & 0x07) << 5) | ((rbuffer[2*i+1] & 0xE0) >> 3));
		//blue = ((gfbwbuffer[2*i+1] & 0x1F) << 3); //we don't need blue for looking for green.
		if(red > green + 20){ //Green is usually much higher then red due the the extra bit place in RGB565
			rnumbuffer[i] = 1;
			vis +=1;
		}else{
			rnumbuffer[i] = 0;
		}
		//If Red is visable then isRedVisable turns to true
		if(vis>0){
			isRedVisable = 1;
		}else{
			isRedVisable = 0;
		}
	}	
}


//MAIN PROGRAM

void love(void)
{

	setColour('r');
	e_init_port();
	e_init_motors();
	e_init_ad_scan(ALL_ADC);
	
	int isRedVisable = 0;

	e_calibrate_ir();
	setUpCamera();

	e_set_speed_left(500);
	e_set_speed_right(500);
	e_start_agendas_processing();

	int proxy0;
	int proxy1;

	proxy0 = e_get_prox(0);
	proxy1 = e_get_prox(1);

	int i = 0;	

	while(1)
	{

	e_stop_led_blinking();
		//(1) GET PUCK TO FOLLOW RED

		//--- Dodgy Code ---
/*
		while(isColourVis() == 1)
		{	
			findColour();

			e_set_speed_left(400);
			e_set_speed_right(400);

			while(isColourVis() == 0 && i<1000000)
			{
				e_set_speed_left(-200);
				e_set_speed_right(200);
				
				if(isColourVis()== 1)
				{
					e_start_led_blinking(200);
					break;
				}
			}
				
		}
*/
		//--------------------

		int centreValue;

		takeImage();
		processImageRed();
		
		//Take a section of the center, this means if there is an error with one it won't effect it as a whole.
		centreValue = rnumbuffer[38] + rnumbuffer[39] + rnumbuffer[40] + rnumbuffer[41] + rnumbuffer[42] + rnumbuffer[43]; // removes stray 
		if(centreValue > 3){ //If green is in the middle then it will go forward 
			e_destroy_agenda(rturn);
			rforward();
		}else if(isRedVisable == 1){//If red isn't in the center but is visable then picks a direction to turn to face it
			e_activate_agenda(rturn, 650);
		}else{// if red isn't visible and no true values it will turn left
			e_destroy_agenda(rturn);
			e_set_speed_left (0);
			e_set_speed_right(0);
		}	


		//(2) TURN ON LED0 WHEN AN OBSTICAL IS DETECTED
	 	//Blinks front LED when object is v. near (50 less near) - Did work now doesn't
	
		if(proxy0 < 100)
 			LED0 = 0;
 		else
			LED0 = 1;

		//for(i=0; i<100000; i++) { asm("nop"); } //WAIT - Delay getting back to the while

	
		//(3) GIVEN THE OBJECT IS IN CLOSE PROXIMITY, FLASH ALL LED's
		//Currently does nothing
/*
		while(proxy0 > 300)
		{
			e_start_led_blinking(200);
		}
		e_stop_led_blinking();
*/
		//for(i=0; i<100000; i++) { asm("nop"); }
		
	
		//4) WHEN THE OBJECT IS IN CLOSE PROXIMITY, AFFECTIONATELY BUMP
		//INTO THE OBJECT WHILST MAKING A KISSING SOUND
		//Means lights are permenantly on	
/*
		while(proxy0 < 100)
		{
 			e_set_led(8, 1);
		}
*/		
			for(i=0; i<100000; i++) { asm("nop"); }
		
	}
}





