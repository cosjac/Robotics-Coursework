
#include "p30f6014A.h"
#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "string.h"
#include "motor_led/e_epuck_ports.h"
#include "motor_led/e_init_port.h"
#include "motor_led/advance_one_timer/e_agenda.h"
#include "motor_led/advance_one_timer/e_led.h"
#include "motor_led/advance_one_timer/e_motors.h"
#include "uart/e_uart_char.h"
#include "a_d/advance_ad_scan/e_ad_conv.h"
#include "a_d/advance_ad_scan/e_prox.h"

#include "utility.h"
#include "goal_seeking.h"


#define LEFT_FOLLOW			0		// behaviors IDs	
#define RIGHT_FOLLOW		1 

#define NB_SENSORS          8		// number of sensors
#define BIAS_SPEED      	500		// robot bias speed
#define SENSOR_THRESHOLD	300		// discount sensor noise below threshold
#define MAXSPEED 			800		// maximum robot speed
#define PI 					3.1415	// PI
#define WHEEL_RADIUS		0.0205	 
#define AXLE_LENGTH			0.053	


int gssensorzero[8];
int gsweightleft[8] = { -10, -10, -5, 0, 0, 5, 10, 10 };
int gsweightright[8] = { 10, 10, 5, 0, 0, -5, -10, -10 };

double l_past, r_past;
double dl, dr, d, da;
double theta, x, y;
double iniX, iniY, iniTheta;
double displacement;
double eudist, delta, dist_mline;
double gx = 0.5;
double gy = 0.0;

int leftwheel, rightwheel;		// motor speed left and right
int distances[NB_SENSORS];		// array keeping the distance sensor readings


void compute_odometry() {
	double l = e_get_steps_left();
	double r = e_get_steps_right();

	dl = ((l - l_past) / 1000.0) * 2 * PI * WHEEL_RADIUS; // distance covered by left wheelin meter
	dr = ((r - r_past) / 1000.0) * 2 * PI * WHEEL_RADIUS; // distance covered by right wheelin meter
	d = (dr + dl) / 2;
	da = (dr - dl) / AXLE_LENGTH; // delta orientation
	r_past = r;
	l_past = l;
	displacement = displacement + (d * 100);
	//printf("estimated distance covered by left wheel: %gm. \n\n", dl);
	//printf("estimated distance covered by right wheel : %gm. \n\n", dr);
	//printf("estimated distance covered by the robot : %gm. \n\n", d);
	//printf("estimated change of orientation : %g rad .\ n", da);
	//printf("Wheel distances: l = %f; r = %f; dl = %f; dr= %f; d= %f; da= %f; \n\n", l, r, dl, dr, d, da);

	theta = theta + da;
	x = x + d*cos(theta);
	y = y + d*sin(theta);
	//printf("displacement : %f \n\n", displacement);
}

double getDistanceToGoal(double currentX, double currentY, double goalX, double goalY){
	return sqrt((goalX - currentX) * (goalX - currentX) + (goalY - currentY) * (goalY - currentY));
}

double getCurrentDistanceToMline(double goalX, double goalY, double currentX, double currentY){
	// Compute the Euclidean distance
	double vac = (currentX*goalY) - (currentY*goalX);
	if (vac > 0)
		return ((currentX*goalY) - (currentY*goalX) / (sqrt(goalY*goalY + goalX*goalX)));
	else
		return (-(currentX*goalY) - (currentY*goalX) / (sqrt(goalY*goalY + goalX*goalX)));
}

double getDeltaCurrentPosToGoal(float goalX, float goalY, float currentX, float currentY, float theta){
	double alpha = 0;
	alpha = ((atan2((goalY - currentY), (goalX - currentX)) * 180) / PI);
	//printf("\n alpha = %.1lf", alpha);

	// Wrap theta
	double theta_wrap = theta * (180 / PI);

	//printf("\n theta = %.1lf", theta);

	if (theta_wrap >180)
	{
		theta_wrap = theta_wrap - 360;
	}
	else if (theta_wrap < -180)
	{
		theta_wrap = theta_wrap + 360;
	}

	double vac = alpha - theta_wrap;

	//printf("\n theta wrap = %.1lf", theta_wrap);
	//printf("\n vac = %.1lf", vac);
	return vac;
}

void TurnTowardsGoal(){
	printf("\n Turning towards goal: %f \n\n", delta);

	if (delta > 0){
		while (delta > 5){
			printf("Delta: %f \n\n", delta);
			compute_odometry();
			delta = getDeltaCurrentPosToGoal(gx, gy, x, y, theta);
			gsSetSpeed(-350, 350);
		}
	}
	else{
		while (delta < 5){
			printf("Delta: %f \n\n", delta);
			compute_odometry();
			delta = getDeltaCurrentPosToGoal(gx, gy, x, y, theta);
			gsSetSpeed(350, -350);
		}
	}
}

/*! \breif Read the sensors proxymities
* \param sensorTable Where the value must be stocked
*/
void gsGetSensorValues(int *sensorTable) {
	unsigned int i;
	for (i = 0; i < NB_SENSORS; i++) {
		sensorTable[i] = e_get_calibrated_prox(i); //e_get_prox(i) - gssensorzero[i];
	}
}

/*! \brief Set robot speed */
void gsSetSpeed(int LeftSpeed, int RightSpeed) {
	if (LeftSpeed < -MAXSPEED) { LeftSpeed = -MAXSPEED; }
	if (LeftSpeed >  MAXSPEED) { LeftSpeed = MAXSPEED; }
	if (RightSpeed < -MAXSPEED) { RightSpeed = -MAXSPEED; }
	if (RightSpeed >  MAXSPEED) { RightSpeed = MAXSPEED; }
	e_set_speed_left(LeftSpeed);
	e_set_speed_right(RightSpeed);
}

void goal_seeking() {
	e_init_port();
	e_init_ad_scan(ALL_ADC);
	e_init_motors();
	e_start_agendas_processing();
	e_activate_agenda(left_led, 2500);
	e_activate_agenda(right_led, 2500);
	e_pause_agenda(left_led);
	e_pause_agenda(right_led);

	e_calibrate_ir();

	//initalise global variables
	theta = 0.0;
	x = 0.0;
	y = 0.0;

	l_past = e_get_steps_left();
	r_past = e_get_steps_right();
	iniX = x;
	iniY = y;
	iniTheta = theta;
	displacement = 0;

	//local vars
	int thres = 50;
	int mline_thres = 2;
	int i;							// FOR-loop counters
	int gostraight;
	int goalNotFound = 1;
	int hitObjectOnMline = 0;
	int stepsToWaitBeforeCheck = 0;
	int state = 0;

	while (goalNotFound){
		gsGetSensorValues(distances);
		compute_odometry();
		eudist = getDistanceToGoal(x, y, gx, gy);
		dist_mline = getCurrentDistanceToMline(gx, gy, x, y);
		delta = getDeltaCurrentPosToGoal(gx, gy, x, y, theta);
		//printf("Theta : %f \n\n", theta);
		//printf("Delta : %f \n\n", delta);
		e_set_led(1, 0);

		switch (state){
		case 0:
			TurnTowardsGoal();

			if (distances[0] > thres || distances[1] > thres || distances[6] > thres || distances[7] > thres){
				//// Go to the next state
				printf("Detected object -> state 1 \n ");
				state = 1;
			}
			else{
				printf("Distance to goal : %f \n\n", eudist);
				if (eudist < 0.02){
					gsSetSpeed(0, 0);
					goalNotFound = 0;
					while (1){
						e_blink_led();
					}

					printf("\n GOAL FOUND!\n\n");
				}
				else{
					leftwheel = BIAS_SPEED;
					rightwheel = BIAS_SPEED;
					gsSetSpeed(leftwheel, rightwheel);
				}
			}
			break;
		case 1:
			//printf("Distance to mLine : %f \n\n", dist_mline);
			//printf("Distance to goal : %f \n\n", eudist);
			e_set_led(1, 1);
			if ((dist_mline * 1000 < 2 && dist_mline * 1000 > -2) && stepsToWaitBeforeCheck > 100)
			{
				printf("\n Detected the m-line -> turn towards goal \n");
				state = 0;
			}
			else{
				gostraight = 0;

				for (i = 0; i<8; i++) {
					if (distances[i]>50) { break; }
				}
				if (i == 8) {
					gostraight = 1;
				}
				else {
					gsweightleft[0] = -10;
					gsweightleft[7] = -10;
					gsweightright[0] = 10;
					gsweightright[7] = 10;
					if (distances[2]>300) {
						distances[1] -= 200;
						distances[2] -= 600;
						distances[3] -= 100;
					}
				}

				leftwheel = BIAS_SPEED;
				rightwheel = BIAS_SPEED;
				if (gostraight == 0) {
					for (i = 0; i<8; i++) {
						leftwheel += gsweightleft[i] * (distances[i] >> 4);
						rightwheel += gsweightright[i] * (distances[i] >> 4);
					}
				}

				// set robot speed
				gsSetSpeed(leftwheel, rightwheel);
				wait(15000);
				stepsToWaitBeforeCheck++;
			}

			if (state != 0){
				state = CheckIfLostWall(thres);
			}

			break;
		}
	}
}

int CheckIfLostWall(int thres){
	gsGetSensorValues(distances);
	int isTriggered = 0;
	int i;

	for (i = 0; i<8; i++) {
		if (distances[i]>50) {
			isTriggered = 1;
			break;
		}
	}
	return isTriggered;
}

