#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>


#include <main.h>
#include <motors.h>
#include <pi_regulator.h>
#include <process_image.h>


//#define KP 0.811
//simple PI regulator implementation
int16_t pi_regulator(float distance, float goal){ // returns a speed to be compensated from original speed

	float error = 0;
	float speed = 0;

	static float sum_error = 0;

	error = distance - goal; //distance = begin

	//disables the PI regulator if the error is to small
	//this avoids to always move as we cannot exactly be where we want and 
	//the camera is a bit noisy
	if(fabs(error) < ERROR_THRESHOLD){ //error threshold = +- 20 pixels
		return 0;
	}

	sum_error += error;

	//we set a maximum and a minimum for the sum to avoid an uncontrolled growth
	if(sum_error > MAX_SUM_ERROR){
		sum_error = MAX_SUM_ERROR;
	}else if(sum_error < -MAX_SUM_ERROR){
		sum_error = -MAX_SUM_ERROR;
	}

	speed = KP * error + KI * sum_error;

	// derivator here

    return (int16_t)speed;
}

static THD_WORKING_AREA(waPiRegulator, 256);
static THD_FUNCTION(PiRegulator, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    systime_t time;

    int16_t speed = 150;
    int16_t speed_correction = 0;

    while(1){
        time = chVTGetSystemTime();

        //computes the speed to give to the motors
        //distance_cm is modified by the image processing thread
        speed = pi_regulator(get_distance_cm(), GOAL_DISTANCE);
        //computes a correction factor to let the robot rotate to be in front of the line
        speed_correction = (get_line_position() - (IMAGE_BUFFER_SIZE/2));

        //if the line is nearly in front of the camera, don't rotate
        if(abs(speed_correction) < ROTATION_THRESHOLD){
        	speed_correction = 0;
        }

        //applies the speed from the PI regulator and the correction for the rotation
		right_motor_set_speed(speed - ROTATION_COEFF * speed_correction);
		left_motor_set_speed(speed + ROTATION_COEFF * speed_correction);

        //100Hz
        chThdSleepUntilWindowed(time, time + MS2ST(10));
    }
}

static THD_WORKING_AREA(waPRegulator, 256);
static THD_FUNCTION(PRegulator, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    systime_t time;

    int16_t speed = 154;
    int16_t speed_correction = 0;

    while(1){
        time = chVTGetSystemTime();
        
        //computes the speed to give to the motors
        //distance_cm is modified by the image processing thread
        //speed = pi_regulator(get_distance_cm(), GOAL_DISTANCE);
        //computes a correction factor to let the robot rotate to be in front of the line
        speed_correction = correction(200);

        //if the line is nearly in front of the camera, don't rotate


        //applies the speed from the PI regulator and the correction for the rotation
		right_motor_set_speed(speed + speed_correction/2);
		left_motor_set_speed(speed- speed_correction/2);

        //100Hz
        chThdSleepUntilWindowed(time, time + MS2ST(10));
    }
}

void pi_regulator_start(void){
	chThdCreateStatic(waPiRegulator, sizeof(waPiRegulator), NORMALPRIO, PiRegulator, NULL);
}
void p_regulator_start(void){
	chThdCreateStatic(waPRegulator, sizeof(waPRegulator), NORMALPRIO, PRegulator, NULL);
}
//psuedo -code

//vitesse_goal = 154 steps/s = 2cm/s
//position_goal = 200 pixel
//position error_threshold = +- 20 pixels
//motor left = vitesse constant = 154 steps/s
//motor right = vitesse constant + correction
//calculer error,
/*
 *
*/
int16_t correction(int16_t position_goal)
{
	uint16_t pos = get_line_position();
	uint16_t no_line = get_line();
	if(no_line)
		return 0;
	float error = pos -position_goal;
	if(fabs(error) < ERROR_T)
		return 0;
	if(error < 0) //motor right faster
	{
	 	 if(fabs(error) > 180)
	 	 	 return  146;  //steps/s
	 	 else
	 	 	 return (KP * error);
	}
	if(error > 0) //motor right slow down
	{
		if(fabs(error) > 128)
			return  -104; //steps/s
		else
		 	return (KP * error);
	}
	return 0;
}

/*
 correction speed:
 error = begin - position goal
 if(fabs(error) < error threshold)
 	 return 0;

 if(error < 0) //motor right faster
 	 if(fabs(error) > 180)
 	 	 correction = + 146 steps/s
 	 else
 	 	 correction = Kp(0.811) * error

if(error > 0) motor right slow down
	if(fabs(error) > 128)
		correction = -104 steps/s
	else
	 	correction = Kp * error

 */
