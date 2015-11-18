#include "p30F6014A.h"


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

#include "global_functions.h"
#include "fear.h"
#include "aggressive_c.h"
#include "love.h"
#include "curious.h"

#include <utility/utility.h>

#define PI 3.14159265358979

void robot_off(void)
{
	e_init_port();
	e_init_motors();
	e_init_ad_scan(ALL_ADC);
	
	e_calibrate_ir();

	e_start_agendas_processing();
	while (1) {
		e_led_clear();
		wait(10000);
	}
}

int main() {
 	e_led_clear();
 	int selector;

	//Reset if Power on (some problem for few robots)
	if (RCONbits.POR) {
		RCONbits.POR=0;
		__asm__ volatile ("reset");
	}

	//system initialization
	e_init_port();    // configure port pins   
	e_init_ad_scan(ALL_ADC);

	// sound
	e_init_sound();
    e_init_motors();
	e_calibrate_ir();   
	e_start_agendas_processing();
      
  
	// Decide upon program
	selector=getselector();

	switch (selector) {
		case 0:robot_off();break;
		case 1:aggressive_c();break;
		case 2:fear();break;
		case 3:curious();break;
		case 4:love();break;
		case 5:robot_off();break;
		default:/*own high level behaviour*/break;
	}

	return 0;
}

