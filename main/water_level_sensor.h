//
//  water_level_sensor.h
//  
//
//  Created by Kaan Arı on 16.07.2021.
//

#ifndef water_level_sensor_h
#define water_level_sensor_h

#include <stdio.h>
#include <stdlib.h>

#include <homekit/types.h>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#define delay(ms) (vTaskDelay(ms/portTICK_RATE_MS))

#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling

#define LEVEL_THRESHOLD 20


// ACTIVE CHARACTERISTIC
#define LEVEL_SENSOR_ACTIVE 1
#define LEVEL_SENSOR_INACTIVE 0

extern homekit_service_t humidity_service;

bool initialize_water_level_sensor();

uint32_t read_water_level(); 

bool check_water_level();

#endif /* water_level_sensor_h */
