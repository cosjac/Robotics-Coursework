
#include "stdio.h"
#include "string.h"

#include "library/motor_led/e_init_port.h"
#include "library/motor_led/advance_one_timer/e_motors.h"
#include "library/motor_led/advance_one_timer/e_led.h"
#include "library/motor_led/advance_one_timer/e_agenda.h"
#include "library/uart/e_uart_char.h"
#include "library/a_d/advance_ad_scan/e_ad_conv.h"
#include "library/a_d/advance_ad_scan/e_prox.h"

#include "aggressive.h"

void aggressive(void)
{	
	e_init_port();
	e_init_motors();
	e_init_ad_scan(ALL_ADC);
	
	e_calibrate_ir();

	e_start_agendas_processing();
	while(1);
}
