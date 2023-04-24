//
//  valve.h
//  
//
//  Created by Kaan Arı on 16.07.2021.
//

#ifndef valve_h
#define valve_h

#include "humidity_sensor.h"
#include "water_level_sensor.h"
#include "pwm.h"
#include "led.h"

#include <homekit/types.h>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define delay(ms) (vTaskDelay(ms/portTICK_RATE_MS))

// ACTIVE CHARACTERISTIC
#define VALVE_ACTIVE 1
#define VALVE_INACTIVE 0

// IN_USE CHARACTERISTIC
#define IDLE 0
#define RUNNING 1

// VALVE TYPE
#define GENERIC 0
#define SPRINKLER 1

// STATUS_FAULT CHARACTERISTIC
#define NO_FAULT 0
#define FAULT 1

// IS_CONFIGURED CHARACTERISTIC
#define NOT_CONFIGURED 0
#define CONFIGURED 1

extern homekit_service_t valve_service;

void valve_active_setter(homekit_value_t value);

void prepare_irrigation();

void prepare_closing();

bool activate_valve();

bool deactivate_valve();

bool open_valve();

bool close_valve();

bool valve_set_fault();

bool valve_reset_fault();

bool configure();

bool reset_configuration();

void set_duration(homekit_value_t value);

homekit_value_t remaining_duration();

void initialize_valve();


#endif /* valve_h */
