#include <i2c_robocol.h>



/*!
 * @brief  Funcion ejecutada en caso de que se de interrupcion por finalización de transferencia i2c del master
 */

static void i2c_masterCallback(I2C_Type *base, i2c_master_handle_t *handle, status_t status, void *userData)
{
    /* Signal transfer success when received success status. */
	i2c_master_dev_t *dev=(i2c_master_dev_t *)userData;
    if (status == kStatus_Success)
    {
        dev->completionFlag = true;
    }
}

/*!
 * @brief  Funcion ejecutada en caso de que se de interrupcion por finalización de transferencia i2c del slave
 */
static void i2c_slaveCallback(I2C_Type *base, i2c_slave_transfer_t *xfer, void *userData)
{

	i2c_slave_dev_t *dev=(i2c_slave_dev_t *)userData;

    switch (xfer->event)
    {
        /*  Transmit request */
        case kI2C_SlaveTransmitEvent:
            /*  Update information for transmit process */
            xfer->data = dev->buffer;
            xfer->dataSize = dev->data_length;
            break;

        /*  Receive request */
        case kI2C_SlaveReceiveEvent:
            /*  Update information for received process */
            xfer->data = dev->buffer;
            xfer->dataSize = dev->data_length;
            break;

        /*  Transfer done */
        case kI2C_SlaveCompletionEvent:
        	dev->completionFlag = true;
            break;

        default:
        	dev->completionFlag = false;
            break;
    }
}

/*!
 * @brief  Inicializa el dispositivo i2c master dev  configurando los pines necesarios de la tarjeta
 * Por defecto, se utiliza el I2C_1, el cual tiene  SDA asociado a el pin B3 y el SCL asociado al pin B2
 * TODO  Permitir al usuario seleccionar el I2C deseado
 * @param dev 		puntero a un i2c_master_dev_t
 * @param baud_rate tasa de baudios de la comunicación i2c
 */
void i2c_masterInit(i2c_master_dev_t *dev,  uint32_t baud_rate){


	/* Alocando memoria en heap para dispositivo virtual*/
	dev->baudRate=baud_rate;
    NVIC_SetPriority(I2C_MASTER_IRQN, 1);

	CLOCK_EnableClock(kCLOCK_PortB);                           /* Port D Clock Gate Control: Clock enabled */

	//Configuracion de pines
	const port_pin_config_t portb2_pin55_config = {
		kPORT_PullUp,                                            /* Internal pull-up resistor is enabled */
		kPORT_SlowSlewRate,                                      /* Slow slew rate is configured */
		kPORT_PassiveFilterDisable,                              /* Passive filter is disabled */
		kPORT_LowDriveStrength,                                  /* Low drive strength is configured */
		kPORT_MuxAlt2,                                           /* Pin is configured as I2C0_SCL */
	};
	PORT_SetPinConfig(PORTB, PIN2, &portb2_pin55_config);  /* PORTB2 (pin 55) is configured as I2C0_SCL */

	const port_pin_config_t portb3_pin56_config = {
		kPORT_PullUp,                                            /* Internal pull-up resistor is enabled */
		kPORT_SlowSlewRate,                                      /* Slow slew rate is configured */
		kPORT_PassiveFilterDisable,                              /* Passive filter is disabled */
		kPORT_LowDriveStrength,                                  /* Low drive strength is configured */
		kPORT_MuxAlt2,                                           /* Pin is configured as I2C0_SDA */
	};
	PORT_SetPinConfig(PORTB, PIN3, &portb3_pin56_config);  /* PORTB3 (pin 56) is configured as I2C0_SDA */

}

/*!
 * @brief  Realiza una transferencia de escritura al dispositivo esclavo indicado
 * @param dev 		Puntero a un i2c_dev_t
 * @param address   Direccion de 7bits del dispositivo esclavo
 * @param data_out  Puntero a datos a enviar
 * @param transfer_size  Tamaño de la transferencia en bytes
 * @return Estado final de la transaccion
 */
uint8_t i2c_masterWrite(i2c_master_dev_t *dev, uint8_t address, uint8_t data_out[], uint8_t transfer_size){
	i2c_master_config_t		masterConfig;
	i2c_master_transfer_t	masterXfer;
	i2c_master_handle_t		masterHandle;
	uint32_t				sourceClock;
	status_t				status;

	/*
	     * masterConfig.baudRate_Bps = 100000U;
	     * masterConfig.enableStopHold = false;
	     * masterConfig.glitchFilterWidth = 0U;
	     * masterConfig.enableMaster = true;
	     */
	I2C_MasterGetDefaultConfig(&masterConfig);
	masterConfig.baudRate_Bps = dev->baudRate;

	sourceClock = I2C_MASTER_CLK_FREQ;
	I2C_MasterInit((I2C_Type *)I2C_MASTER_BASE, &masterConfig, sourceClock);

	memset(&masterHandle, 0, sizeof(masterHandle));
	memset(&masterXfer, 0, sizeof(masterXfer));
	masterXfer.slaveAddress = address;
	masterXfer.direction = kI2C_Write;
	masterXfer.subaddress = (uint32_t)NULL;
	masterXfer.subaddressSize = 0;
	masterXfer.data = data_out;
	masterXfer.dataSize = transfer_size;
	masterXfer.flags = kI2C_TransferDefaultFlag;

	I2C_MasterTransferCreateHandle((I2C_Type *)I2C_MASTER_BASE, &masterHandle, i2c_masterCallback, dev);

	dev->completionFlag=false;
	status=I2C_MasterTransferNonBlocking((I2C_Type *)I2C_MASTER_BASE, &masterHandle, &masterXfer);

	while (!dev->completionFlag)
    {

    }


	/*  Wait for transfer completed. */

	return status;
}


/*!
 * @brief  Realiza una transferencia de lectura al dispositivo esclavo indicado
 * @param dev 		Puntero a un i2c_dev_t
 * @param address   Direccion de 7bits del dispositivo esclavo
 * @param data_out  Puntero posición de memoria para almacenar los datos recibidos
 * @param transfer_size  Tamaño de la transferencia en bytes
 * @return Estado final de la transaccion
 */
uint8_t i2c_masterRead(i2c_master_dev_t *dev, uint8_t address, uint8_t data_in[], uint8_t transfer_size){

	i2c_master_config_t		masterConfig;
	i2c_master_transfer_t	masterXfer;
	i2c_master_handle_t		masterHandle;
	uint32_t				sourceClock;
	status_t				status;

	/*
		 * masterConfig.baudRate_Bps = 100000U;
		 * masterConfig.enableStopHold = false;
		 * masterConfig.glitchFilterWidth = 0U;
		 * masterConfig.enableMaster = true;
		 */
	I2C_MasterGetDefaultConfig(&masterConfig);
	masterConfig.baudRate_Bps = dev->baudRate;

	sourceClock = I2C_MASTER_CLK_FREQ;
	I2C_MasterInit((I2C_Type *)I2C_MASTER_BASE, &masterConfig, sourceClock);

	memset(&masterHandle, 0, sizeof(masterHandle));
	memset(&masterXfer, 0, sizeof(masterXfer));
	masterXfer.slaveAddress = address;
	masterXfer.direction = kI2C_Read;
	masterXfer.subaddress = (uint32_t)NULL;
	masterXfer.subaddressSize = 0;
	masterXfer.data = data_in;
	masterXfer.dataSize = transfer_size;
	masterXfer.flags = kI2C_TransferDefaultFlag;

	I2C_MasterTransferCreateHandle((I2C_Type *)I2C_MASTER_BASE, &masterHandle, i2c_masterCallback, dev);

	dev->completionFlag=false;
	status=I2C_MasterTransferNonBlocking((I2C_Type *)I2C_MASTER_BASE, &masterHandle, &masterXfer);

	/*  Wait for transfer completed. */
	while (!dev->completionFlag)
	{
	}

	I2C_MasterDeinit((I2C_Type *)I2C_MASTER_BASE);

	return status;
}



/*!
 * @brief  Inicializa el dispositivo i2c slave dev  configurando los pines necesarios de la tarjeta.
 * Por defecto, se utiliza el I2C_1, el cual tiene  SDA asociado a el pin E0 y el SCL asociado al pin E1
 * TODO  Permitir al usuario seleccionar el I2C deseado
 * @param dev 		Puntero a un i2c_slave_dev_t
 * @param address	Direccion que se le asignará al dispositivo esclavo creado
 * @param buff		Puntero a la direccion de memoria que se usara como buffer del esclavo
 */
void i2c_slaveInit(i2c_slave_dev_t *dev, uint8_t address, uint8_t *buff){


	/* Instanciando estructura de datos en memoria previamente alocada por el usuario*/
	dev->address=address;
	dev->buffer=buff;

    NVIC_SetPriority(I2C_SLAVE_IRQN, 0);

	CLOCK_EnableClock(kCLOCK_PortE);                           /* Port E Clock Gate Control: Clock enabled */

	//Configuracion de pines
	const port_pin_config_t porte0_pin1_config = {
		kPORT_PullUp,                                            /* Internal pull-up resistor is enabled */
		kPORT_SlowSlewRate,                                      /* Slow slew rate is configured */
		kPORT_PassiveFilterDisable,                              /* Passive filter is disabled */
		kPORT_LowDriveStrength,                                  /* Low drive strength is configured */
		kPORT_MuxAlt6,                                           /* Pin is configured as I2C1_SDA */
	};
	PORT_SetPinConfig(PORTE, PIN0, &porte0_pin1_config);   /* PORTE0 (pin 1) is configured as I2C1_SDA */
	const port_pin_config_t porte1_pin2_config = {
		kPORT_PullUp,                                            /* Internal pull-up resistor is enabled */
		kPORT_SlowSlewRate,                                      /* Slow slew rate is configured */
		kPORT_PassiveFilterDisable,                              /* Passive filter is disabled */
		kPORT_LowDriveStrength,                                  /* Low drive strength is configured */
		kPORT_MuxAlt6,                                           /* Pin is configured as I2C1_SCL */
	};
	PORT_SetPinConfig(PORTE, PIN1, &porte1_pin2_config);   /* PORTE1 (pin 2) is configured as I2C1_SCL */

}

/*!
 * @brief Pone al I2C esclavo en espera de la siguiente transferencia que envie el maestro
 * @param dev 				Puntero a un i2c_slave_dev_t
 * @param transfer_size  	Tamaño de la transferencia en bytes
 * @return Estado final de la transaccion
 */
uint8_t i2c_slaveTransfer(i2c_slave_dev_t *dev, uint8_t transfer_size){
	i2c_slave_config_t		slaveConfig;
	status_t				status;

	dev->transfer_size=transfer_size;

	/*1.Set up i2c slave first*/
	/*
	 * slaveConfig.addressingMode = kI2C_Address7bit;
	 * slaveConfig.enableGeneralCall = false;
	 * slaveConfig.enableWakeUp = false;
	 * slaveConfig.enableBaudRateCtl = false;
	 * slaveConfig.enableSlave = true;
	 */
	I2C_SlaveGetDefaultConfig(&slaveConfig);

	slaveConfig.addressingMode = kI2C_Address7bit;
	slaveConfig.slaveAddress = dev->address;
	slaveConfig.upperAddress = 0; /*  not used for this example */

	//Inicializacion de dispositivo esclavo con la configuración definida anteriormente
	I2C_SlaveInit((I2C_Type *)I2C_SLAVE_BASE, &slaveConfig, I2C_SLAVE_CLK_FREQ);

	//Objeto para manejo de transferencia  se deja vacio
	memset(&(dev->slave_handle), 0, sizeof(dev->slave_handle));

	//Creacion del objeto que manejara la transferencia de datos
	I2C_SlaveTransferCreateHandle((I2C_Type *)I2C_SLAVE_BASE, &(dev->slave_handle), i2c_slaveCallback, dev);

	//Antes de iniciar la transferencia, se hace reset a la bandera de terminacion
	dev->completionFlag=false;

	//Se inicia la transferencia no bloqueante del esclavo
	status=I2C_SlaveTransferNonBlocking((I2C_Type *)I2C_SLAVE_BASE, &(dev->slave_handle), kI2C_SlaveCompletionEvent);

	//Se espera que la transacción se termine
//	while (!dev->completionFlag)
//	{
//	}

	return status;
}
