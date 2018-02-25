/*
 * pwm_robocol.c
 *
 *  Created on: 23/09/2017
 *      Author: n.rios11
 */
#include "pwm_robocol.h"
#include "movimiento.c"
#include "board.h"

#include "fsl_debug_console.h"
#include "fsl_tpm.h"

#include "fsl_common.h"
#include "pin_mux.h"
#include "clock_config.h"

/*BOARD_TPM_BASEADDR - Es la dirección a la que se envía el PWM (E.g. TPM0)*/

void pwm_init(pwm_t *pwm){

	tpm_config_t tpmInfo;
	tpm_chnl_pwm_signal_param_t tpmParam;

    /* Configure tpm params with frequency 24kHZ */
     tpmParam.chnlNumber = (tpm_chnl_t) pwm->chanel;
     tpmParam.level = kTPM_LowTrue;
     tpmParam.dutyCyclePercent = 0U;

    PORT_SetPinMux(pwm->port, pwm->pin, pwm->mux);

    TPM_GetDefaultConfig(&tpmInfo);
    /* Initialize TPM module */
    TPM_Init(pwm->base, &tpmInfo);

    TPM_SetupPwm(pwm->base, &tpmParam, 1U, kTPM_CenterAlignedPwm, 24000U, TPM_SOURCE_CLOCK);

    TPM_StartTimer(pwm->base, kTPM_SystemClock);
}

void pwm_updatedDutycycle(pwm_t *pwm, uint8_t dutyCycle){

	updatedDutycycle = dutyCycle * 10U;

	    			/* Disable channel output before updating the dutycycle */
	    			TPM_UpdateChnlEdgeLevelSelect(pwm->base,
	    					(tpm_chnl_t) pwm->chanel, 0U);

	    			/* Update PWM duty cycle */
	    			TPM_UpdatePwmDutycycle(pwm->base,
	    					(tpm_chnl_t) pwm->chanel, kTPM_CenterAlignedPwm,
	    					updatedDutycycle);

	    			/* Start channel output with updated dutycycle */
	    			TPM_UpdateChnlEdgeLevelSelect(pwm->base,
	    					(tpm_chnl_t) pwm->chanel, kTPM_HighTrue);

	    			PRINTF("The duty cycle was successfully updated!\r\n");

}
