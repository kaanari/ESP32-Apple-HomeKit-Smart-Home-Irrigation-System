//
//  led.h
//  
//
//  Created by Kaan Arı on 18.07.2021.
//

#ifndef led_h
#define led_h

#include <stdio.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <homekit/homekit.h>
#include <homekit/characteristics.h>

#include "irrigation.h"
#include "valve.h"

void led_write(bool on);

void led_init();

void led_identify_task(void *_args);

void led_identify(homekit_value_t _value);

homekit_value_t led_on_get();

void led_on_set(homekit_value_t value);

void led_irrigation_finish();

void led_irrigation_start();

#endif /* led_h */
