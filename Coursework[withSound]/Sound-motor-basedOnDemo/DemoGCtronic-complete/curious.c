#include "stdio.h"
#include "string.h"
#include "math.h"
#include <time.h>

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

#include "curious.h"
#include "global_functions.h"

//Main function of follower
void curious(void){
	int centreValue;
	int gnumbuffer[80];
	long isGreenVisable;
 
	setUpCamera();

	e_start_agendas_processing();

	while(1){
		takeImage();
		processImage();
		//Take a section of the center, this means if there is an error with one it won't effect it as a whole.
		centreValue = gnumbuffer[38] + gnumbuffer[39] + gnumbuffer[40] + gnumbuffer[41] + gnumbuffer[42] + gnumbuffer[43]; // removes stray 
		if(centreValue > 3){ //If green is in the middle then it will go forward 
			e_destroy_agenda(gturn);
			forward();
		}else if(isGreenVisable == 1){//If green isn't in the center but is visable then picks a direction to turn to face it
			e_activate_agenda(gturn, 650);
		}else{// if green isn't visible and no true values it will turn left
			e_destroy_agenda(gturn);
			e_set_speed_left (0);
			e_set_speed_right(0);
		}
	}
}

