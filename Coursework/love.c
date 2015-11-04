#include "Libraries Clean/init_port.h"
#include "Libraries Clean/motors.h"
#include "Libraries Clean/prox.h"
#include "Libraries Clean/led.h"

void love(void)
{
	//set up robot
	InitMotors();
	InitProx();
	LedClear();
	
	while (1) {

	}
}
