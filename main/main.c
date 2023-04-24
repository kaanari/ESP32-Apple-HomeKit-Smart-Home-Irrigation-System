#include <stdio.h>
#include <stddef.h>
#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>


#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include "wifi.h"
#include "captdns.h"

#include "esp_wifi_types.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "freertos/portmacro.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "tcpip_adapter.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/api.h"
#include "lwip/err.h"
#include "string.h"
#include "cJSON.h"
#include "lwip/dns.h"
#include "captdns.h"

#include "wifi_manager.h"
#include "irrigation.h"
#include "valve.h"
#include "water_level_sensor.h"
#include "humidity_sensor.h"
#include "led.h"

#define AUTOMATIC_HUMIDITY 1
#define AUTOMATIC_SCHEDULE 2

#define delay(ms) (vTaskDelay(ms/portTICK_RATE_MS))

uint8_t user_permission = 0;

char* json_unformatted;


//const static char http_html_hdr[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
const uint8_t indexHtmlStart[] asm("_binary_index_html_start"); //uint8_t
const uint8_t indexHtmlEnd[] asm("_binary_index_html_end");     //uint8_t

void on_wifi_ready();

void irr_set(homekit_value_t value) {
    if (value.format != homekit_format_bool) {
        printf("Invalid value format: %d\n", value.format);
        return;
    }
    
    if(value.bool_value){
        set_low_battery();

    }else{
        set_normal_battery();
    }
}

homekit_accessory_t *accessories[] = {
    HOMEKIT_ACCESSORY(.id=1, .category=homekit_accessory_category_lightbulb, .services=(homekit_service_t*[]){
        HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]){
            HOMEKIT_CHARACTERISTIC(NAME, "Irrigation System"),
            HOMEKIT_CHARACTERISTIC(MANUFACTURER, "Kaan ARI"),
            HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, "037A2BABF19D"),
            HOMEKIT_CHARACTERISTIC(MODEL, "WaterFlow 1"),
            HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, "0.1"),
            HOMEKIT_CHARACTERISTIC(IDENTIFY, led_identify),
            NULL
        }),
        HOMEKIT_SERVICE(LIGHTBULB, .primary=true, .characteristics=(homekit_characteristic_t*[]){
            HOMEKIT_CHARACTERISTIC(NAME, "Sample LED"),
            HOMEKIT_CHARACTERISTIC(
                ON, false,
                .getter=led_on_get,
                .setter=led_on_set
            ),
            NULL
        }),
        HOMEKIT_SERVICE(LIGHTBULB, .primary=false, .characteristics=(homekit_characteristic_t*[]){
            HOMEKIT_CHARACTERISTIC(NAME, "Activate"),
            HOMEKIT_CHARACTERISTIC(
                ON, false,
                .setter=irr_set
            ),
            NULL
        }),
        //&irrigation_service,
        &valve_service,
        //&humidity_service,
        &humidity_sensor_service,
        NULL
    }),
    NULL
};

homekit_server_config_t config = {
    .accessories = accessories,
    .password = "111-11-222"
};

void on_wifi_ready(void *pvParameter) {
    homekit_server_init(&config);
}

void automatic_irrigation_humidity(){
    if(!check_humidity()){
        valve_active_setter(HOMEKIT_BOOL(true));
    }
}

void automatic_irrigation_schedule(){
    
}

void monitoring_task()
{
    for(;;){
        if(user_permission == AUTOMATIC_HUMIDITY)
            automatic_irrigation_humidity();
        else if(user_permission == AUTOMATIC_SCHEDULE)
            automatic_irrigation_schedule(); // It may be changed to RTC interrupt method! Check it
        
        vTaskDelay(600000 / portTICK_PERIOD_MS); // Check Every Ten minute
    }
}

void app_main(void) {
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
    
    led_init();
    initialize_water_level_sensor();
    initialize_humidity_sensor();
    initialize_valve();
    
    wifi_manager_start();

    wifi_manager_set_callback(WM_EVENT_STA_GOT_IP, &on_wifi_ready);
    xTaskCreatePinnedToCore(&monitoring_task, "monitoring_task", 512, NULL, 1, NULL, 1); // Pin a task to Core 1 (0-1). 512 byte Stack size

}
