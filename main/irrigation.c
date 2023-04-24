//
//  irrigation.c
//  
//
//  Created by Kaan Arı on 15.07.2021.
//

#include "irrigation.h"

/*   */
homekit_characteristic_t irrigation_active = HOMEKIT_CHARACTERISTIC_(ACTIVE, SYSTEM_ACTIVE);
homekit_characteristic_t irrigation_state = HOMEKIT_CHARACTERISTIC_(IN_USE, IDLE);
homekit_characteristic_t irrigation_mode = HOMEKIT_CHARACTERISTIC_(PROGRAM_MODE, NO_SCHEDULE);
homekit_characteristic_t irrigation_fault = HOMEKIT_CHARACTERISTIC_(PROGRAM_MODE, NO_FAULT);
homekit_characteristic_t irrigation_duration = HOMEKIT_CHARACTERISTIC_(REMAINING_DURATION, 500);

homekit_service_t irrigation_service = HOMEKIT_SERVICE_(IRRIGATION_SYSTEM, .primary=false, .characteristics=(homekit_characteristic_t*[]){
    HOMEKIT_CHARACTERISTIC(NAME, "Irrigation System"),
    &irrigation_active,
    &irrigation_state,/*   */
    &irrigation_mode,
    &irrigation_fault,
    &irrigation_duration,
    NULL
});

homekit_service_t* init_irrigation(){
    return &(irrigation_service);
}

bool activate_irrigation(){
    irrigation_active.value = HOMEKIT_UINT8(SYSTEM_ACTIVE);
    homekit_characteristic_notify(&irrigation_active, irrigation_active.value);
    
    return true;
}

bool deactivate_irrigation(){
    irrigation_active.value = HOMEKIT_UINT8(SYSTEM_INACTIVE);
    homekit_characteristic_notify(&irrigation_active, irrigation_active.value);
    
    return true;
}

homekit_value_t activation_status(){
    return HOMEKIT_BOOL(irrigation_active.value.uint8_value == SYSTEM_ACTIVE);
}

bool run_irrigation(){
    irrigation_state.value = HOMEKIT_UINT8(RUNNING);
    homekit_characteristic_notify(&irrigation_state, irrigation_state.value);
    
    return true;
}

bool stop_irrigation(){
    irrigation_state.value = HOMEKIT_UINT8(IDLE);
    homekit_characteristic_notify(&irrigation_state, irrigation_state.value);
    
    return true;
}

bool change_mode(uint8_t mode){
    
    if(mode > 2){
        printf("Error !");
        return false;
    }
    
    irrigation_mode.value = HOMEKIT_UINT8(mode);
    homekit_characteristic_notify(&irrigation_mode, irrigation_mode.value);
    return true;
}

bool irrigation_set_fault(){
    irrigation_fault.value = HOMEKIT_UINT8(FAULT);
    homekit_characteristic_notify(&irrigation_fault, irrigation_fault.value);
    
    return true;
}

bool irrigation_reset_fault(){
    irrigation_fault.value = HOMEKIT_UINT8(NO_FAULT);
    homekit_characteristic_notify(&irrigation_fault, irrigation_fault.value);
    
    return true;
}

bool change_remaining_time(uint16_t second){
    if(second > 43200){
        printf("Maximum Time for System reached!!!");
        return false;
    }
    
    irrigation_duration.value = HOMEKIT_UINT32(second);
    homekit_characteristic_notify(&irrigation_duration, irrigation_duration.value);
    return true;
}
