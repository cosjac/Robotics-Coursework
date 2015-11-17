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
/*
long isGreenVisable;
char gbuffer[160];
int gnumbuffer[80];


//custom cam picture load - Simon and Shaun
void takeImage(){
	e_poxxxx_launch_capture((char *)gbuffer);
    while(!e_poxxxx_is_img_ready()){};
}

// Image processing removes useless information - Simon and Shaun
void processImage(){	
	long i;
	int green, red, vis;
	for(i=0; i<80; i++){
		//RGB turned into an integer value for comparison
		red = (gbuffer[2*i] & 0xF8);
		green = (((gbuffer[2*i] & 0x07) << 5) | ((gbuffer[2*i+1] & 0xE0) >> 3));
		//blue = ((gfbwbuffer[2*i+1] & 0x1F) << 3); we don't need blue for looking for green.
		if(green > red + 50){ //Green is usually much higher then red due the the extra bit place in RGB565
			gnumbuffer[i] = 1;
			vis +=1;
		}else{
			gnumbuffer[i] = 0;
		}
		//If Green is visable then isRedVisable turns to true
		if(vis>0){
			isGreenVisable = 1;
		}else{
			isGreenVisable = 0;
		}
	}	
}

//Decide which way to turn based on the number of true pixels. - Simon and Shaun
int gturnDirection(){
	int sumL = 0;
	int sumR = 0;
	int i;
	for(i=0;i<40;i++){
		sumL += gnumbuffer[i];
		sumR += gnumbuffer[i+40];
	}
	if(sumL<sumR){ 
		return 1;
	}else{
		return 0;
	}
}

void setUpCamera(void){
	e_poxxxx_init_cam();
	e_poxxxx_config_cam(0,(ARRAY_HEIGHT - 4)/2,640,4,8,4,RGB_565_MODE);
	e_poxxxx_set_mirror(1,1);
	e_poxxxx_write_cam_registers(); 
}*/

void love(void)
{
	e_init_port();
	e_init_motors();
	e_init_ad_scan(ALL_ADC);
	
	e_calibrate_ir();
	setUpCamera();

	e_set_speed_left(500);
	e_set_speed_right(500);
	//e_set_led(8, 0);
	e_start_agendas_processing();

	//(1) Get robot to follow (Pink)
	//run_breitenberg_follower();

	//(2) Turn on the LED0 when an obstacle is detected
 	//by the proximity sensor number 0. 
	int proxy0;
	int proxy1;
	while(1)
	{
		long i;
		proxy0 = e_get_prox(0);
		proxy1 = e_get_prox(1);
	
		if(proxy0 < 50)
 			LED0 = 0;
 		else
			e_blink_led();
			LED0 = 1;

		for(i=0; i<100000; i++) { asm("nop"); }
	}

	takeImage();

	//(3) Given the object is in close proximity flash all LED's
	while(1)
	{
		long i;
		proxy0 = e_get_prox(0);
		while(proxy0 < 300)
		{
			e_start_led_blinking(200);
		}
		e_stop_led_blinking();

		for(i=0; i<100000; i++) { asm("nop"); }
	}

	//4) When the object is in close proximity affectionately bump
	//into the object making a kissing noise whilst doing so


/*
	while(1)
	{
		long i;
		proxy0 = e_get_prox(0);
		//proxy1 = e_get_prox(1);
	
		while(proxy0 < 100)
		{
 			e_set_led(8, 1);
		}

 		

		for(i=0; i<100000; i++) { asm("nop"); }
	}*/
	
}
