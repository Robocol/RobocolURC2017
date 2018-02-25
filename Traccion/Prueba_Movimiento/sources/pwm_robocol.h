/*
 * pwm_robocol.h
 *
 *  Created on: 23/09/2017
 *      Author: n.rios11
 */

#ifndef SOURCES_PWM_ROBOCOL_H_
#define SOURCES_PWM_ROBOCOL_H_

#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_tpm.h"
#include "fsl_common.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL46Z4.h"
#include "fsl_port.h"

typedef struct pwm{
	TPM_Type * base;
	PORT_Type * port;
	port_mux_t mux;
	tpm_interrupt_enable_t chnlinterrupt;
	tpm_status_flags_t chnlflag;
	uint32_t pin;
	uint8_t chanel;
} pwm_t;

// Pines de acuerdo a DataSheet KL46Z

//Inicia los pines
void pwm_init(pwm_t *pwm);

void pwm_updatedDutycycle(pwm_t *pwm, uint8_t dutyCycle);


#endif /* SOURCES_PWM_ROBOCOL_H_ */
