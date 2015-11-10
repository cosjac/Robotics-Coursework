
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

#include "love.h"

void love(void)
{
	e_init_port();
	e_init_motors();
	e_init_ad_scan(ALL_ADC);
	
	e_calibrate_ir();

	e_set_speed_left(500);
	e_set_speed_right(500);
	//e_set_led(8, 0);
	e_start_agendas_processing();


	//Search for pink objects. When found turn on LED

	//A little exemple which turn the LED0 when an obstacle is detected
 	//by the proximity sensor number 0. 

	int proxy0;
	int proxy1;
	while(1)
	{
		long i;
		proxy0 = e_get_prox(0);
		proxy1 = e_get_prox(1);
	
		if(proxy0 < 30)
 			LED0 = 0;
 		else
			LED0 = 1;

		for(i=0; i<100000; i++) { asm("nop"); }
	}
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
