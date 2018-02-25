/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_tpm.h"

#include "fsl_common.h"
#include "pin_mux.h"
#include "clock_config.h"

#include "pwm_robocol.h"
#include "gpio_robocol.h"
#include "i2c_robocol.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* The Flextimer instance/channel used for board */
#define BOARD_TPM_BASEADDR TPM0  /*Influye sobre la configuración de los pines */

/* Interrupt to enable and flag to read; depends on the TPM channel used */
#define TPM_CHANNEL_INTERRUPT_ENABLE kTPM_Chnl5InterruptEnable   /*No influye en la configuración del pin*/

/* Interrupt number and interrupt handler for the TPM instance used */
#define TPM_INTERRUPT_NUMBER TPM0_IRQn
#define TPM_CHANNEL_FLAG kTPM_Chnl5Flag   /*No influye en la configuración del pin*/
#define TPM_LED_HANDLER TPM0_IRQHandler

/* Get source clock for TPM driver */
#define TPM_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_PllFllSelClk)

#define EXAMPLE_I2C_MASTER_BASEADDR I2C0
#define EXAMPLE_I2C_SLAVE_BASEADDR I2C1
#define I2C_MASTER_CLK_FREQ CLOCK_GetFreq(I2C0_CLK_SRC)
#define I2C_SLAVE_CLK_FREQ CLOCK_GetFreq(I2C1_CLK_SRC)
#define I2C_MASTER_IRQ I2C0_IRQn
#define I2C_SLAVE_IRQ I2C1_IRQn
#define I2C_MASTER_SLAVE_ADDR_7BIT 0x7EU
#define I2C_BAUDRATE 100000U
#define  I2C_DATA_LENGTH 1U

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief delay a while.
 */
void delay(void);

/*******************************************************************************
 * Variables
 ******************************************************************************/
//volatile bool brightnessUp = true; /* Indicate LED is brighter or dimmer */
//volatile uint8_t updatedDutycycle = 10U;
//volatile uint8_t getCharValue = 0U;

volatile uint8_t direction;

volatile uint8_t g_slave_buff[I2C_DATA_LENGTH];

volatile bool brightnessUp = true; /* Indicate LED is brighter or dimmer */
volatile uint8_t updatedDutycycle = 10U;
volatile uint8_t getCharValue = 0U;

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Main function
 */
gpio_dev_t GPIOINA1_MotoresI = {GPIOB, PORTB, 20u, kGPIO_DigitalOutput};
gpio_dev_t GPIOINB1_MotoresI = {GPIOA, PORTA, 2u, kGPIO_DigitalOutput};
gpio_dev_t GPIOINA2_MotoresD = {GPIOD, PORTD, 3u, kGPIO_DigitalOutput};
gpio_dev_t GPIOINB2_MotoresD = {GPIOA, PORTA, 4u, kGPIO_DigitalOutput};

gpio_dev_t GPIOEN_MotoresI = {GPIOC, PORTC, 9u, kGPIO_DigitalOutput};
gpio_dev_t GPIOEN_MotoresD = {GPIOA, PORTA, 13u, kGPIO_DigitalOutput};
gpio_dev_t GPIOEN_Motor5 = {GPIOC, PORTC, 1u, kGPIO_DigitalOutput};
gpio_dev_t GPIOEN_Motor6 = {GPIOC, PORTC, 2u, kGPIO_DigitalOutput};

pwm_t PWM_PTE29 = {TPM0, PORTE, kPORT_MuxAlt3, kTPM_Chnl2InterruptEnable, kTPM_Chnl2Flag, 29u, 2u}; //Integrated
pwm_t PWM_PTA5 =  {TPM0, PORTA, kPORT_MuxAlt3, kTPM_Chnl2InterruptEnable, kTPM_Chnl2Flag, 5u, 2u}; //Llanta 1
pwm_t PWM_PTC8 = {TPM0, PORTC, kPORT_MuxAlt3, kTPM_Chnl4InterruptEnable, kTPM_Chnl4Flag, 8u, 4u}; //Llanta 2
pwm_t PWM_PTD2 = {TPM0, PORTD, kPORT_MuxAlt4, kTPM_Chnl2InterruptEnable, kTPM_Chnl2Flag, 2u, 2u}; //Llanta 3
pwm_t PWM_PTD4 = {TPM0, PORTD, kPORT_MuxAlt4, kTPM_Chnl3InterruptEnable, kTPM_Chnl3Flag, 3u, 3u}; //Llanta 4
pwm_t PWM_PTA12 = {TPM1, PORTA, kPORT_MuxAlt3, kTPM_Chnl0InterruptEnable, kTPM_Chnl0Flag, 12u, 0u}; //Llantas 5 y 6


int main(void)
{

	i2c_slave_dev_t s_dev;

    /* Board pin, clock, debug console init */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    /* Select the clock source for the TPM counter as kCLOCK_PllFllSelClk */
    CLOCK_SetTpmClock(1U);

    PRINTF("\r\nI2C example -- SlaveInterrupt.\r\n");

        //Inicialización de esclavo
        i2c_slaveInit(&s_dev,I2C_MASTER_SLAVE_ADDR_7BIT,g_slave_buff);

        //Esclavo queda a la espera de transferencia por parte del maestro
    	i2c_slaveTransfer(&s_dev,I2C_DATA_LENGTH);

        /*3.Transfer completed. Check the data.*/

    	direction = g_slave_buff[0];

        PRINTF("0x%2x  ", g_slave_buff[0]);

        PRINTF("\r\n\r\n");

        PRINTF("\r\n\r\n");

        /*  Already setup the slave transfer ready in item 1. */

        PRINTF("\r\nEnd of I2C example .\r\n");


    //Inicia el pwm al pin asignado
    pwm_init(&PWM_PTA5);
    pwm_init(&PWM_PTA12);
    pwm_init(&PWM_PTC8);
    pwm_init(&PWM_PTD2);
    pwm_init(&PWM_PTD4);
    pwm_init(&PWM_PTE29);

    //Salida digitales para los INA/B de los motores
    gpio_confPin(&GPIOINA1_MotoresI);
    gpio_confPin(&GPIOINB1_MotoresI);
    gpio_confPin(&GPIOINA2_MotoresD);
    gpio_confPin(&GPIOINB2_MotoresD);

    gpio_confPin(&GPIOEN_MotoresI);
    gpio_confPin(&GPIOEN_MotoresD);
    gpio_confPin(&GPIOEN_Motor5);
    gpio_confPin(&GPIOEN_Motor6);
    gpio_confPin(&GPIO13_LED);


    while (1)
    {

    	direction = g_slave_buff[0];

    	if(g_slave_buff[0] != 0)
    	{

    	//Ir hacia delante
    	    	if( direction == 1)
    	    	{

    	    		// Dirección de los motores

    	    		gpio_writePin(&GPIOINA1_MotoresI, 0); //Dirección Motores Izquierda
    	    		gpio_writePin(&GPIOINA2_MotoresD, 1); //Dirección Motores Derecha
//    	    		gpio_writePin(&GPIOINB1_MotoresI, 1); //Dirección Motores 5
//    	    		gpio_writePin(&GPIOINB2_MotoresD, 1); //Dirrección Motores 6

    	    		//Enables Llantas Traseras

    	    		gpio_writePin(&GPIOEN_Motor5, 1); //Habilita el motor 5
    	    		gpio_writePin(&GPIOEN_Motor6, 1); //Habilita el motor 6

    	    		pwm_updatedDutycycle(&PWM_PTA5, 5); //PWM Motores Derecha
    	    		pwm_updatedDutycycle(&PWM_PTA12, 7); //PWM Motores Izquierda
    	    		pwm_updatedDutycycle(&PWM_PTC8, 5); //PWM Llantas Traseras

    	    		gpio_writePin(&GPIO13_LED, 1);

    	    	}

    	    	//Turn left
    	    	else if ( direction == 2)
    	    	{
    	     		// Dirección de los motores

    	    		gpio_writePin(&GPIOINA1_MotoresI, 0); //Dirección Motores Izquierda
    	    		gpio_writePin(&GPIOINA2_MotoresD, 0); //Dirección Motores Derecha
//    	    		gpio_writePin(&GPIOINB1_MotoresI, 1); //Dirección Motores 5
//    	    		gpio_writePin(&GPIOINB2_MotoresD, 0); //Dirrección Motores 6

    	    		//Enables Llantas Traseras

    	    		gpio_writePin(&GPIOEN_Motor5, 0); //Habilita el motor 5
    	    		gpio_writePin(&GPIOEN_Motor6, 1); //Habilita el motor 6

    	    		pwm_updatedDutycycle(&PWM_PTA5, 7); //PWM Motores Derecha
    	    		pwm_updatedDutycycle(&PWM_PTA12, 7); //PWM Motores Izquierda
    	    		pwm_updatedDutycycle(&PWM_PTC8, 5); //PWM Llantas Traseras

    	    		gpio_writePin(&GPIO13_LED, 0);

    	    	}

    	    	//Go on
    	    	else if ( direction == 3)
    	    	{
    	    		// Dirección de los motores

    	    		gpio_writePin(&GPIOINA1_MotoresI, 1); //Dirección Motores Izquierda
    	    		gpio_writePin(&GPIOINA2_MotoresD, 0); //Dirección Motores Derecha
//    	    		gpio_writePin(&GPIOINB1_MotoresI, 0); //Dirección Motores 5
//    	    		gpio_writePin(&GPIOINB2_MotoresD, 1); //Dirrección Motores 6

    	    		//Enables Llantas Traseras

    	    		gpio_writePin(&GPIOEN_Motor5, 1); //Habilita el motor 5
    	    		gpio_writePin(&GPIOEN_Motor6, 1); //Habilita el motor 6

    	    		pwm_updatedDutycycle(&PWM_PTA5, 7); //PWM Motores Derecha
    	    		pwm_updatedDutycycle(&PWM_PTA12, 7); //PWM Motores Izquierda
    	    		pwm_updatedDutycycle(&PWM_PTC8, 5); //PWM Llantas Traseras

    	    		gpio_writePin(&GPIO13_LED, 1);


    	    	}

    	    	//Turn right
    	    	else if ( direction == 4)
    	    	{
    	    		// Dirección de los motores

    	    		    	    		gpio_writePin(&GPIOINA1_MotoresI, 1); //Dirección Motores Izquierda
    	    		    	    		gpio_writePin(&GPIOINA2_MotoresD, 1); //Dirección Motores Derecha
//    	    		    	    		gpio_writePin(&GPIOINB1_MotoresI, 0); //Dirección Motores 5
//    	    		    	    		gpio_writePin(&GPIOINB2_MotoresD, 0); //Dirrección Motores 6

    	    		    	    		//Enables Llantas Traseras

    	    		    	    		gpio_writePin(&GPIOEN_Motor5, 1); //Habilita el motor 5
    	    		    	    		gpio_writePin(&GPIOEN_Motor6, 0); //Habilita el motor 6

    	    		    	    		pwm_updatedDutycycle(&PWM_PTA5, 7); //PWM Motores Derecha
    	    		    	    		pwm_updatedDutycycle(&PWM_PTA12, 7); //PWM Motores Izquierda
    	    		    	    		pwm_updatedDutycycle(&PWM_PTC8, 5); //PWM Llantas Traseras

    	    		    	    		gpio_writePin(&GPIO13_LED, 0);

    	    	}



    }

    	else
    	    	    	    	{
    	    		// Dirección de los motores

    	    		    	    		gpio_writePin(&GPIOINA1_MotoresI, 0); //Dirección Motores Izquierda
    	    		    	    		gpio_writePin(&GPIOINA2_MotoresD, 0); //Dirección Motores Derecha
    	    		    	    		gpio_writePin(&GPIOINB1_MotoresI, 0); //Dirección Motores 5
    	    		    	    		gpio_writePin(&GPIOINB2_MotoresD, 0); //Dirrección Motores 6

    	    		    	    		//Enables Llantas Traseras

    	    		    	    		gpio_writePin(&GPIOEN_Motor5, 0); //Habilita el motor 5
    	    		    	    		gpio_writePin(&GPIOEN_Motor6, 0); //Habilita el motor 6

    	    		    	    		pwm_updatedDutycycle(&PWM_PTA5, 0); //PWM Motores Derecha
    	    		    	    		pwm_updatedDutycycle(&PWM_PTA12, 0); //PWM Motores Izquierda
    	    		    	    		pwm_updatedDutycycle(&PWM_PTC8, 0); //PWM Llantas Traseras

    	    		    	    		gpio_writePin(&GPIO13_LED, 0);


    	    	    	    	}

}
}






