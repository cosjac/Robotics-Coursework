#include "Libraries Clean/init_port.h"
#include "Libraries Clean/motors.h"
#include "Libraries Clean/prox.h"
#include "Libraries Clean/led.h"
#include "library/motor_led/e_led.h"

void LedFearFlash(void)
{
	int i;
	for (i=0;i<7;i++) {
		SetLed(i,1);
		for (i=0;i<10000;i++) {asm("nop");}
	}
}

void fear(void)
{
	//set up robot
	InitMotors();
	InitProx();
	LedClear();

	while (1) {

	}

}
