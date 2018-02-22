#ifndef I2C_CAFFES_H
#define I2C_CAFFES_H

/* Freescale includes. */
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_i2c.h"
#include "fsl_device_registers.h"
#include "pin_mux.h"
#include <stdbool.h>
#include <stdlib.h>
#include "clock_config.h"
#include "stdbool.h"
#include "fsl_common.h"
#include "fsl_port.h"
#include "gpio_robocol.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define I2C_MASTER_BASE (I2C0_BASE)
#define I2C_MASTER_IRQN (I2C0_IRQn)

#define I2C_SLAVE_BASE (I2C1_BASE)
#define I2C_SLAVE_IRQN (I2C1_IRQn)

#define I2C_MASTER ((I2C_Type *)I2C_MASTER_BASE)
#define I2C_SLAVE ((I2C_Type *)I2C_SLAVE_BASE)

#define I2C_MASTER_CLK_SRC (I2C0_CLK_SRC)
#define I2C_SLAVE_CLK_SRC (I2C1_CLK_SRC)

#define I2C_MASTER_CLK_FREQ CLOCK_GetFreq(I2C0_CLK_SRC)
#define I2C_SLAVE_CLK_FREQ CLOCK_GetFreq(I2C1_CLK_SRC)



/*******************************************************************************
 * ERROR STATES
 ******************************************************************************/
#define I2C_WrongDeviceType		0x10


typedef struct i2c_master_dev{
	uint32_t				baudRate;
	volatile bool			completionFlag;
} i2c_master_dev_t;

typedef struct i2c_slave_dev{
	uint8_t 				address;      	/*!< 7-bit slave address. */
	uint8_t	        		*buffer;
	uint8_t					transfer_size;
	i2c_slave_handle_t		slave_handle;
	volatile bool	    	completionFlag;
} i2c_slave_dev_t;




/******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief  Inicializa el dispositivo i2c slave dev  configurando los pines necesarios de la tarjeta
 * @param dev 		Puntero a un i2c_slave_dev_t
 * @param address	Direccion que se le asignará al dispositivo esclavo creado
 * @param buff		Puntero a la direccion de memoria que se usara como buffer del esclavo
 */
void i2c_slaveInit(i2c_slave_dev_t *dev, uint8_t address, uint8_t *buff);

/*!
 * @brief Pone al I2C esclavo en espera de la siguiente transferencia que envie el maestro
 * @param dev 				Puntero a un i2c_slave_dev_t
 * @param transfer_size  	Tamaño de la transferencia en bytes
 * @return Estado final de la transaccion
 */
uint8_t i2c_slaveTransfer(i2c_slave_dev_t *dev, uint8_t data_legth);


/*!
 * @brief  Inicializa el dispositivo i2c master dev  configurando los pines necesarios de la tarjeta
 * @param dev 		puntero a un i2c_master_dev_t
 * @param baud_rate tasa de baudios de la comunicación i2c
 */
void i2c_masterInit(i2c_master_dev_t *dev,  uint32_t baud_rate);


/*!
 * @brief  Realiza una transferencia de escritura al dispositivo esclavo indicado
 * @param dev 		Puntero a un i2c_dev_t
 * @param address   Direccion de 7bits del dispositivo esclavo
 * @param data_out  Puntero a datos a enviar
 * @param transfer_size  Tamaño de la transferencia en bytes
 * @return Estado final de la transaccion
 */
uint8_t i2c_masterWrite(i2c_master_dev_t *dev, uint8_t subaddress, uint8_t data_in[], uint8_t transfer_size);

/*!
 * @brief  Realiza una transferencia de lectura al dispositivo esclavo indicado
 * @param dev 		Puntero a un i2c_dev_t
 * @param address   Direccion de 7bits del dispositivo esclavo
 * @param data_out  Puntero posición de memoria para almacenar los datos recibidos
 * @param transfer_size  Tamaño de la transferencia en bytes
 * @return Estado final de la transaccion
 */
uint8_t i2c_masterRead(i2c_master_dev_t *dev, uint8_t subaddress, uint8_t data_out[], uint8_t transfer_size);

#endif
