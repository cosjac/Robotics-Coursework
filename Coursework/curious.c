#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "library/motor_led/e_epuck_ports.h"
#include "library/motor_led/e_init_port.h"
#include "library/motor_led/advance_one_timer/e_motors.h"
#include "library/motor_led/advance_one_timer/e_led.h"
#include "library/motor_led/advance_one_timer/e_agenda.h"
#include "library/uart/e_uart_char.h"
#include "library/a_d/advance_ad_scan/e_ad_conv.h"
#include "library/a_d/advance_ad_scan/e_prox.h"
#include "library/camera/fast_2_timer/e_poxxxx.h"
#include "library/followWall/runwallfollow.h"

#include "curious.h"



char gbuffer[160];
int gnumbuffer[80];
long isGreenVisable;


//custom cam picture load
void takeImage(){
	e_poxxxx_launch_capture((char *)gbuffer);
    while(!e_poxxxx_is_img_ready()){};
}
// Image processing removes useless information
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

//Decide which way to turn based on the number of true pixels.
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
//Function to deal with turning.
void gturn(void) {
	if(gturnDirection()){
		e_set_speed_left (500);
		e_set_speed_right(-500);
	}else{
		e_set_speed_left (-500);
		e_set_speed_right(500);
	}
}
void forward(void){
	e_set_speed_left (500);
	e_set_speed_right(500);
}

void setUpCamera(void){
	e_poxxxx_init_cam();
	e_poxxxx_config_cam(0,(ARRAY_HEIGHT - 4)/2,640,4,8,4,RGB_565_MODE);
	e_poxxxx_set_mirror(1,1);
	e_poxxxx_write_cam_registers(); 
}

int greenIsInMiddle(int centreValue){
	if(centreValue > 3)
		return 1;
	return 0;
}
 
//Main function of follower
//Main function of follower
void curious(void){
	setUpCamera();

	e_start_agendas_processing();
	e_set_led(0,1);
	int centreValue;

	while(1){	
		takeImage();
		processImage();
		//Take a section of the center, this means if there is an error with one it won't effect it as a whole.
		centreValue = gnumbuffer[38] + gnumbuffer[39] + gnumbuffer[40] + gnumbuffer[41] + gnumbuffer[42] + gnumbuffer[43]; // removes stray 
		if(centreValue > 3){ //If green is in the middle then it will go forward 
			e_set_led(1,1);
			forward();
			e_destroy_agenda(gturn);
			if(e_get_prox(0) < 1000){
				run_wallfollow();
			}
		}else if(isGreenVisable == 1){//If green isn't in the center but is visable then picks a direction to turn to face it
			e_activate_agenda(gturn, 650);
		}else{// if green isn't visible and no true values it will turn left
			e_set_led(2,1);
			e_destroy_agenda(gturn);
			e_set_speed_left (0);
			e_set_speed_right(0);
		}
	}
}

