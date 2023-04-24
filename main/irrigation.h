//
//  irrigation.h
//  
//
//  Created by Kaan Arı on 15.07.2021.
//

#ifndef irrigation_h
#define irrigation_h

#include <homekit/types.h>
#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include <stdio.h>

// ACTIVE CHARACTERISTIC
#define SYSTEM_ACTIVE 1
#define SYSTEM_INACTIVE 0

// IN_USE CHARACTERISTIC
#define IDLE 0
#define RUNNING 1

// PROGRAM_MODE CHARACTERISTIC
#define NO_SCHEDULE 0        // RUNNING WHEN IN RUNNING - OFF WHEN NOT IN RUNNING
#define PROGRAM_SCHECULED 1  // RUNNING SCHEDULE WHEN IN RUNNNING - SCHEDULED WHEN NOT IN RUNNING
#define MANUAL_MODE 2        // RUNNING WHEN IN RUNNING - SCHEDULED WHEN NOT IN RUNNING

// STATUS_FAULT CHARACTERISTIC
#define NO_FAULT 0
#define FAULT 1

// REMAINING_DURATION
#define REMAINING_TIME(x) HOMEKIT_UINT16(x)

homekit_service_t* init_irrigation();

bool activate_irrigation();

bool deactivate_irrigation();

homekit_value_t activation_status();

bool run_irrigation();

bool stop_irrigation();

bool irrigation_set_fault();
    
bool irrigation_reset_fault();

bool change_mode(uint8_t mode);

bool change_remaining_time(uint16_t second);

extern homekit_service_t irrigation_service;


#endif /* irrigation_h */
