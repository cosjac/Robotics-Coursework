#include "library/motor_led/e_init_port.h"
#include "library/motor_led/advance_one_timer/e_motors.h"
#include "library/motor_led/advance_one_timer/e_led.h"
#include "library/motor_led/advance_one_timer/e_agenda.h"
#include "library/uart/e_uart_char.h"
#include "library/a_d/advance_ad_scan/e_ad_conv.h"
#include "library/a_d/advance_ad_scan/e_prox.h"
#include "Other Libraries\utility.h"

#include "aggressive.h"
#include "fear.h"
#include "curious.h"
#include "love.h"


void robot_off(void)
{
	while (1) {
		e_led_clear();
		wait(10000);
	}
}

int main(void)
{
	e_init_port();
	e_init_motors();
	e_init_ad_scan(ALL_ADC);
	e_calibrate_ir();
	
	int selector=getselector();
	switch (selector) {
		case 0:robot_off();break;
		case 1:aggressive();break;
		case 2:fear();break;
		case 3:curious();break;
		case 4:love();break;
		case 5:robot_off();break;
		default:/*own high level behaviour*/break;
	}
}
