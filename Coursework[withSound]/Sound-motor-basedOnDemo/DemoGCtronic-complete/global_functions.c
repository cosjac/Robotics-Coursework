#include <time.h>
#include <stdlib.h>
#include "stdio.h"
#include "string.h"

#include <motor_led/e_init_port.h>
#include <motor_led/advance_one_timer/e_led.h>
#include <motor_led/advance_one_timer/e_motors.h>
#include <motor_led/advance_one_timer/e_agenda.h>
#include <camera/fast_2_timer/e_poxxxx.h>
#include <a_d/advance_ad_scan/e_ad_conv.h>
#include <a_d/advance_ad_scan/e_prox.h>
#include <a_d/advance_ad_scan/e_acc.h>
#include <a_d/advance_ad_scan/e_micro.h>
#include <uart/e_uart_char.h>
#include <utility/utility.h>

#include "global_functions.h"

void SetUpGlobal(){
	srand(time(NULL));
}

int rand_lim(int limit){
// return a random number between 0 and limit inclusive.

    int divisor = RAND_MAX/(limit+1);
    int retval;

    do { 
        retval = rand() / divisor;
    } while (retval > limit);

    return retval;
}

void StartRandomRoam(){
	SetUpGlobal();
	
	
}
