/*
 * gpio_robocol.h
 *
 *  Created on: Sep 12, 2017
 *      Author: german
 */

#ifndef SOURCES_GPIO_ROBOCOL_H_
#define SOURCES_GPIO_ROBOCOL_H_

#include "fsl_gpio.h"
#include "fsl_gpio.c"

#define PIN0	0
#define PIN1	1
#define PIN2	2
#define PIN3	3
#define PIN4	4
#define PIN5	5
#define PIN6	6
#define PIN7	7
#define PIN8	8
#define PIN9	9
#define PIN10	10

#define OUTPUT_PIN	   1u
#define INPUT_PIN	   0u

#define GPIO_OK				0
#define GPIO_WRONG_DIR		0xFF

#define GPIO_HIGH	1u
#define GPIO_LOW	0u

typedef  uint8_t state_t;

typedef struct dev{
	GPIO_Type * base;
	PORT_Type * port;
	uint8_t 	pin;
	uint8_t dir;
}gpio_dev_t;

//Configuración de los GPIO's


gpio_dev_t GPIO13_LED = {GPIOE, PORTE, 29u, kGPIO_DigitalOutput}; //LED

void  gpio_confPin(gpio_dev_t* dev);


state_t  gpio_writePin(gpio_dev_t* dev, uint8_t value);


state_t  gpio_setPin(gpio_dev_t* dev);


state_t  gpio_clearPin(gpio_dev_t* dev);


state_t  gpio_readPin(gpio_dev_t* dev, uint8_t *value);


state_t  gpio_switchPinDir(gpio_dev_t* dev, uint8_t dir);


#endif /* SOURCES_GPIO_ROBOCOL_H_ */
