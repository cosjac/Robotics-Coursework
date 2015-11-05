#include "Libraries Clean\init_port.h"
#include "Libraries Clean\motors.h"
#include "Libraries Clean\prox.h"
#include "Libraries Clean\led.h"
#include "Other Libraries\utility.h"

#include "aggressive.h"
#include "fear.h"
#include "curious.h"
#include "love.h"


void robot_off(void)
{
	InitPort();
	InitMotors();
	InitProx();
	while (1) {
		LedClear();
		wait(10000);
	}
}

void test(void)
{
	InitPort();
	InitMotors();
	InitProx();
	long ambient = 0;
	long i;
	int left = 50;
	int right = 50;
	while (1) {
		LedClear();
		for (i=0;i<7;i++) {
			ambient = GetAmbientLight(i);
			if (ambient>3950) {
				switch (i) {
                       //left side sensors
                    case 0: left += 100;right += 50;break;
                    case 1: left += 100;right -= 100;break;
                    case 2: left += 100;right -= 100;break;
                       //back sensors
                   	case 3: left += 200; right -= 200;break;
                       //case 4:left-=300;right+=40;break;
                   	case 4:left += 200;right -= 200;break;
                       //right side sensors
                    case 5:left -= 100;right += 100;break;
                    case 6:left -= 50;right += 50;break;
                    case 7:left += 100;right += 50;break;
                   }	
				SetLed(i,1);
			}	
			else SetLed(i,0);
		}
		SetSpeedRight(right);
		SetSpeedLeft(left);
		wait(10000);
	}
}

int main(void)
{
	int selector=getselector();
	switch (selector) {
		case 0:robot_off();break;
		case 1:aggressive();break;
		case 2:fear();break;
		case 3:curious();break;
		case 4:love();break;
		case 5:test();break;
		default:/*own high level behaviour*/break;
	}
}
