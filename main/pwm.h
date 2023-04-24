
#ifndef EXTRAS_PWM_H_
#define EXTRAS_PWM_H_

#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (32) // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY               (4095) // Set duty to 50%. ((2 ** 13) - 1) * 50% = 4095
#define LEDC_FREQUENCY          (5000) // Frequency in Hertz. Set frequency at 5 kHz


#ifdef __cplusplus
extern "C" {
#endif

//Warning: Printf disturb pwm. You can use "uart_putc" instead.

/**
 * Initialize pwm
 * @param npins Number of pwm pin used
 * @param pins Array pointer to the pins
 * @param reverse If true, the pwm work in reverse mode
 */    
void pwm_init();

/**
 * Set Duty between 0 and UINT16_MAX
 * @param duty Duty value
 */  
void pwm_set_duty(uint16_t duty);


#ifdef __cplusplus
}
#endif

#endif /* EXTRAS_PWM_H_ */
