/*
 * gpio_robocol.c
 *
 *  Created on: Sep 12, 2017
 *      Author: german
 */

#include "gpio_robocol.h"
#include "fsl_port.h"
#include "fsl_common.h"

void  gpio_confPin(gpio_dev_t* dev){

	gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput, 0,
    };

    /* Init output LED GPIO. */
    GPIO_PinInit(dev->base, dev->pin, &led_config);

    PORT_SetPinMux(dev->port, dev->pin, kPORT_MuxAsGpio);
}


state_t  gpio_writePin(gpio_dev_t* dev, uint8_t value){

	if(dev->dir== kGPIO_DigitalOutput){
		GPIO_WritePinOutput(dev->base,dev->pin,value);
		return GPIO_OK;
	}else{
		return GPIO_WRONG_DIR;
	}
}


state_t  gpio_setPin(gpio_dev_t* dev){
	return gpio_writePin(dev,GPIO_HIGH);
}


state_t  gpio_clearPin(gpio_dev_t* dev){
	return gpio_writePin(dev,GPIO_LOW);
}


state_t  gpio_readPin(gpio_dev_t* dev, uint8_t *value){

	if(dev->dir==INPUT_PIN){
		*value= GPIO_ReadPinInput(dev->base, dev->pin);
		return GPIO_OK;
	}else{
		return GPIO_WRONG_DIR;
	}
}


state_t  gpio_switchPinDir(gpio_dev_t* dev, uint8_t dir){

	gpio_pin_config_t led_config = {
        dir, 0,
    };

    /* Init output LED GPIO. */
    GPIO_PinInit(dev->base, dev->pin, &led_config);
}
