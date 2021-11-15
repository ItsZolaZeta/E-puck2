#include "ch.h"
#include "hal.h"
#include <chprintf.h>
#include <usbcfg.h>

#include <main.h>
#include <sensors/proximity.h>
#include <motors.h>

void testIR(void){
	unsigned int ir_1 = get_prox(0);
	unsigned int ir_2 = get_prox(1);
	unsigned int ir_3 = get_prox(2);
	unsigned int ir_4 = get_prox(3);
	unsigned int ir_5 = get_prox(4);
	unsigned int ir_6 = get_prox(5);
	unsigned int ir_7 = get_prox(6);
	unsigned int ir_8 = get_prox(7);


	unsigned int ambient = get_ambient_light(0);
	chprintf((BaseSequentialStream *)&SD3, "IR1 = %u, IR2 = %u, IR3 = %u, IR4 = %u, IR5 = %u, IR6 = %u,IR7 = %u,"
			"IR8 = %u, ambient = %u \n", ir_1, ir_2, ir_3, ir_4, ir_5, ir_6, ir_7, ir_8, ambient);
}
/*
static THD_WORKING_AREA(waIRprocessing, 1024);
static THD_FUNCTION(IRprocessing, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

  //  proximity_start(); moved to main
	calibrate_ir(i);

	// for IR1 and IR8, i = 0, 7
	int value = get_prox(i);

    for(int i = 0; i<8; i++)
    {
    	//if yes obstacle
    	if(get_prox(i) - get_ambient_light(i) < 0) //bool true
    	{
    		//move forward another 3 cm,
    		if(get_prox(0) > get_prox(7)) //closer to IR1 than IR8
    		{
    			// turn left, watch for IR2,3,4
    			//want IR3, get_prox(2) at a constant distance, regulator
    			//once depasse, IR4 see something?, turn right, rewatch for IR3
    			//if see the black line, then turn a bit left (until only ambient light seen), then quit this thread
    		}
    		else
    		{
    			// turn right, watch for IR7,6,5
    			//idem
    		}
    	}
    	else // if no obstacle
    	{
    		// follow line, process_image.c
    	}

    }

    while(1)
    {

    }

}

void process_ir_start(void){
	chThdCreateStatic(waIRprocessing, sizeof(waIRprocessing), NORMALPRIO, IRprocessing, NULL);
}
*/


