 //
//  valve.c
//  
//
//  Created by Kaan Arı on 16.07.2021.
//

#include "valve.h"

const int pump_en_pin = 33;
const int led_pin   = 2;


homekit_characteristic_t valve_name = HOMEKIT_CHARACTERISTIC_(NAME,"Sprinkler I");
homekit_characteristic_t valve_active = HOMEKIT_CHARACTERISTIC_(ACTIVE, VALVE_ACTIVE,.setter = valve_active_setter);
homekit_characteristic_t valve_state = HOMEKIT_CHARACTERISTIC_(IN_USE, IDLE);
homekit_characteristic_t valve_type = HOMEKIT_CHARACTERISTIC_(VALVE_TYPE, SPRINKLER);
homekit_characteristic_t valve_fault = HOMEKIT_CHARACTERISTIC_(STATUS_FAULT, NO_FAULT);
homekit_characteristic_t valve_remaining_duration = HOMEKIT_CHARACTERISTIC_(REMAINING_DURATION, 500);
homekit_characteristic_t valve_set_duration = HOMEKIT_CHARACTERISTIC_(SET_DURATION, 10, .getter = remaining_duration, .setter = set_duration);
homekit_characteristic_t valve_configured = HOMEKIT_CHARACTERISTIC_(IS_CONFIGURED, NOT_CONFIGURED);

homekit_service_t valve_service = HOMEKIT_SERVICE_(VALVE, .primary=false, .characteristics=(homekit_characteristic_t*[]){
    &valve_name,
    &valve_active,
    &valve_state,/*   */
    &valve_type,
    &valve_fault,
    &valve_remaining_duration,
    &valve_set_duration,
    NULL
});
//void *_args
void prepare_irrigation(void *_args){
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    
    open_valve();
    
    led_irrigation_start();
    
    vTaskDelete(NULL);
}
//void *_args
void prepare_closing(void *_args){
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    close_valve();
    
    led_irrigation_start();
    
    vTaskDelete(NULL);
}


void valve_active_setter(homekit_value_t value){
    
    if(value.bool_value){
        if(!check_water_level()) // Sulama suyu hacmine gore uyari verip sulamayi basllatmayabilir!!!
            set_low_battery();
        else
            set_normal_battery();
            
        
        activate_valve(); // IDLE
        printf("Open Valve\n");
        xTaskCreate(prepare_irrigation, "Opening Valve", 512, NULL, 2, NULL);

    }
    else{
        deactivate_valve(); // STOPPING
        printf("Close Valve\n");
        xTaskCreate(prepare_closing, "Closing Valve", 512, NULL, 2, NULL);
        //prepare_closing();
        
        if(!check_water_level())
            set_low_battery();
        else
            set_normal_battery();
    }
}

bool activate_valve(){
    valve_active.value = HOMEKIT_UINT8(VALVE_ACTIVE);
    homekit_characteristic_notify(&valve_active, valve_active.value);
    //printf("Valve Activated.\n");
    
    return true;
}

bool deactivate_valve(){
    valve_active.value = HOMEKIT_UINT8(VALVE_INACTIVE);
    homekit_characteristic_notify(&valve_active, valve_active.value);
    //printf("Valve Deactivated.\n");
    return true;
}

bool open_valve(){
    uint16_t pwm_val =50;
    pwm_set_duty(pwm_val);
    gpio_set_level(pump_en_pin, 1);
    
    
    valve_state.value = HOMEKIT_UINT8(RUNNING);
    homekit_characteristic_notify(&valve_state, valve_state.value);
    
    //printf("Open Valve 2\n");
    
    return true;
}

bool close_valve(){
    valve_state.value = HOMEKIT_UINT8(IDLE);
    homekit_characteristic_notify(&valve_state, valve_state.value);
    
    
    //printf("Close Valve 2\n");
    pwm_set_duty(0);
    gpio_set_level(pump_en_pin, 0);
    
    return true;
}

bool valve_set_fault(){
    valve_fault.value = HOMEKIT_UINT8(FAULT);
    homekit_characteristic_notify(&valve_fault, valve_fault.value);
    
    return true;
}

bool valve_reset_fault(){
    valve_fault.value = HOMEKIT_UINT8(NO_FAULT);
    homekit_characteristic_notify(&valve_fault, valve_fault.value);
    
    return true;
}

bool configure(){
    valve_configured.value = HOMEKIT_UINT8(CONFIGURED);
    homekit_characteristic_notify(&valve_configured, valve_configured.value);
    
    return true;
}

bool reset_configuration(){
    valve_configured.value = HOMEKIT_UINT8(NOT_CONFIGURED);
    homekit_characteristic_notify(&valve_configured, valve_configured.value);
    
    return true;
}


void set_duration(homekit_value_t value){
    valve_remaining_duration.value = value;
    homekit_characteristic_notify(&valve_remaining_duration, valve_remaining_duration.value);
                  
}
homekit_value_t remaining_duration(){
    return valve_remaining_duration.value;
}

void initialize_valve(){
    pwm_init();
    pwm_set_duty(0);
    
    gpio_set_direction(pump_en_pin, GPIO_MODE_OUTPUT);
    
}
