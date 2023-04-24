//
//  humidity_sensor.h
//  
//
//  Created by Kaan Arı on 16.07.2021.
//

#ifndef humidity_sensor_h
#define humidity_sensor_h

#include "water_level_sensor.h"

#include <homekit/types.h>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#define delay(ms) (vTaskDelay(ms/portTICK_RATE_MS))

#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

extern homekit_service_t humidity_sensor_service;

#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling

#define LEVEL_THRESHOLD 50

// ACTIVE CHARACTERISTIC
#define HUMIDITY_SENSOR_ACTIVE 1
#define HUMIDITY_SENSOR_INACTIVE 0

// STATUS_FAULT CHARACTERISTIC
#define NO_FAULT 0
#define FAULT 1

// STATUS_LOW_BATTERY CHARACTERISTIC
#define NORMAL_BATTERY 0
#define LOW_BATTERY 1

// STATUS_TAMPERED CHARACTERISTIC
#define NOT_TAMPERED 0
#define TAMPERED 1

bool check_humidity();

uint32_t read_humidity();

bool initialize_humidity_sensor();

homekit_value_t read_humidity_homekit();

bool set_low_battery();

bool set_normal_battery();

#endif /* humidity_sensor_h */
