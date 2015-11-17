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

#include "love.h"
#include "global_functions.h"
 

void love(void)
{
	int centreValue;
	int gnumbuffer[80];
	long isGreenVisable;

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