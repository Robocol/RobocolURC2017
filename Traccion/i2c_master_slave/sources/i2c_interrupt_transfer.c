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

/*  Standard C Included Files */
#include <string.h>
/*  SDK Included Files */
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_i2c.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "i2c_robocol.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define EXAMPLE_I2C_MASTER_BASEADDR I2C0
#define EXAMPLE_I2C_SLAVE_BASEADDR I2C1
#define I2C_MASTER_CLK_FREQ CLOCK_GetFreq(I2C0_CLK_SRC)
#define I2C_SLAVE_CLK_FREQ CLOCK_GetFreq(I2C1_CLK_SRC)
#define I2C_MASTER_IRQ I2C0_IRQn
#define I2C_SLAVE_IRQ I2C1_IRQn
#define I2C_MASTER_SLAVE_ADDR_7BIT 0x7EU
#define I2C_BAUDRATE 100000U
#define  I2C_DATA_LENGTH 5U


/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

uint8_t g_slave_buff[I2C_DATA_LENGTH];
uint8_t g_master_buff[I2C_DATA_LENGTH];

i2c_master_handle_t g_m_handle;
i2c_slave_handle_t g_s_handle;
volatile bool g_MasterCompletionFlag = false;
volatile bool g_SlaveCompletionFlag = false;

/*******************************************************************************
 * Code
 ******************************************************************************/


/*!
 * @brief Main function
 */
int main(void)
{
	//Declaracón de los dispositivos I2C virtuales, maestro y esclavo
	i2c_master_dev_t m_dev;
	i2c_slave_dev_t s_dev;


    BOARD_InitPins();			//Inicio de pines por defecto para Debug
    BOARD_BootClockRUN();		//Configuración de relojes de arranque
    BOARD_InitDebugConsole();	//Configuración de consola de Debug

    PRINTF("\r\nI2C example -- MasterInterrupt_SlaveInterrupt.\r\n");

    //Inicialización de esclavo
    i2c_slaveInit(&s_dev,I2C_MASTER_SLAVE_ADDR_7BIT,g_slave_buff);
    for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++)
    {
        g_slave_buff[i] = 0;
    }

    //Esclavo queda a la espera de transferencia por parte del maestro
	i2c_slaveTransfer(&s_dev,I2C_DATA_LENGTH);

	//Inicializacion del maestro
    i2c_masterInit(&m_dev,I2C_BAUDRATE);

    /*2.Set up i2c master to send data to slave*/
    for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++)
    {
        g_master_buff[i] = i;
    }

    PRINTF("Master will send data :");
    for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++)
    {
        if (i % 8 == 0)
        {
            PRINTF("\r\n");
        }
        PRINTF("0x%2x  ", g_master_buff[i]);
    }
    PRINTF("\r\n\r\n");


    //Escritura del maestro al esclavo
    i2c_masterWrite(&m_dev,I2C_MASTER_SLAVE_ADDR_7BIT,g_master_buff,I2C_DATA_LENGTH);


    /*3.Transfer completed. Check the data.*/
    for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++)
    {
        if (g_slave_buff[i] != g_master_buff[i])
        {
            PRINTF("\r\nError occured in this transfer ! \r\n");
            break;
        }
    }

    PRINTF("Slave received data :");
    for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++)
    {
        if (i % 8 == 0)
        {
            PRINTF("\r\n");
        }
        PRINTF("0x%2x  ", g_slave_buff[i]);
    }
    PRINTF("\r\n\r\n");

    /*4.Set up slave ready to send data to master.*/
    for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++)
    {
        g_slave_buff[i] = ~g_slave_buff[i];
    }

    PRINTF("This time , slave will send data: :");
    for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++)
    {
        if (i % 8 == 0)
        {
            PRINTF("\r\n");
        }
        PRINTF("0x%2x  ", g_slave_buff[i]);
    }
    PRINTF("\r\n\r\n");

    /*  Already setup the slave transfer ready in item 1. */

    /* 5.Set up master to receive data from slave. */

    for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++)
    {
        g_master_buff[i] = 0;
    }


    //Lectura del esclavo por parte del maestro
    i2c_masterRead(&m_dev,I2C_MASTER_SLAVE_ADDR_7BIT,g_master_buff,I2C_DATA_LENGTH);

    /*  Wait for transfer completed. */
    while (!m_dev.completionFlag)
    {
    }

    /* 6.Transfer completed. Check the data.*/
    for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++)
    {
        if (g_slave_buff[i] != g_master_buff[i])
        {
            PRINTF("\r\nError occured in the transfer ! \r\n");
            break;
        }
    }

    PRINTF("Master received data :");
    for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++)
    {
        if (i % 8 == 0)
        {
            PRINTF("\r\n");
        }
        PRINTF("0x%2x  ", g_master_buff[i]);
    }
    PRINTF("\r\n\r\n");

    PRINTF("\r\nEnd of I2C example .\r\n");
    while (1)
    {
    }
}

