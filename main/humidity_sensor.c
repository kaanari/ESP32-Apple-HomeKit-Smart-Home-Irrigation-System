//
//  humidity_sensor.c
//  
//
//  Created by Kaan Arı on 16.07.2021.
//

#include "humidity_sensor.h"


static esp_adc_cal_characteristics_t *adc_chars;
#if CONFIG_IDF_TARGET_ESP32
static const adc_channel_t channel = ADC_CHANNEL_7;     //GPIO35 if ADC1, GPIO27 if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
#define ADC_MAX 4095
#elif CONFIG_IDF_TARGET_ESP32S2
static const adc_channel_t channel = ADC_CHANNEL_7;     // GPIOx if ADC1, GPIOy if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_13;
#define ADC_MAX 8191
#endif
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;

#define SENSOR_POWER_PIN 26 // GPIO26

homekit_characteristic_t humidity_sensor_name = HOMEKIT_CHARACTERISTIC_(NAME,"Humidity Sensor");
homekit_characteristic_t humidity_sensor_active = HOMEKIT_CHARACTERISTIC_(ACTIVE, HUMIDITY_SENSOR_ACTIVE);
homekit_characteristic_t humidity_sensor_value = HOMEKIT_CHARACTERISTIC_(CURRENT_RELATIVE_HUMIDITY, 0, .getter = read_humidity_homekit);
homekit_characteristic_t humidity_sensor_fault = HOMEKIT_CHARACTERISTIC_(STATUS_FAULT, NO_FAULT);
homekit_characteristic_t humidity_sensor_battery = HOMEKIT_CHARACTERISTIC_(STATUS_LOW_BATTERY, NORMAL_BATTERY);
homekit_characteristic_t humidity_sensor_tampered = HOMEKIT_CHARACTERISTIC_(STATUS_TAMPERED, NOT_TAMPERED);


homekit_service_t humidity_sensor_service = HOMEKIT_SERVICE_(HUMIDITY_SENSOR, .primary=false, .characteristics=(homekit_characteristic_t*[]){
    &humidity_sensor_name,
    &humidity_sensor_active,/*   */
    &humidity_sensor_value,
    &humidity_sensor_fault,
    &humidity_sensor_battery,
    &humidity_sensor_tampered,
    NULL
});

static void open_sensor() {
    gpio_set_level(SENSOR_POWER_PIN, 1);
}

static void close_sensor() {
    gpio_set_level(SENSOR_POWER_PIN, 0);
}

static void check_efuse(void)
{
#if CONFIG_IDF_TARGET_ESP32
    //Check if TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("eFuse Two Point: NOT supported\n");
    }
    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref: Supported\n");
    } else {
        printf("eFuse Vref: NOT supported\n");
    }
#elif CONFIG_IDF_TARGET_ESP32S2
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("Cannot retrieve eFuse Two Point calibration values. Default calibration values will be used.\n");
    }
#else
#error "This example is configured for ESP32/ESP32S2."
#endif
}

static void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
}

bool initialize_humidity_sensor(){
    gpio_set_direction(SENSOR_POWER_PIN, GPIO_MODE_OUTPUT); // Initialize Sensor Power Pin
    open_sensor();
    delay(200);
    
    //Check if Two Point or Vref are burned into eFuse
    check_efuse();

    //Configure ADC
    if (unit == ADC_UNIT_1) {
        adc1_config_width(width);
        adc1_config_channel_atten(channel, atten);
    } else {
        adc2_config_channel_atten((adc2_channel_t)channel, atten);
    }
    
    //Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);
    
    close_sensor();
    return true;

}

uint32_t read_humidity(){
    open_sensor();
    delay(200);
    
    uint32_t adc_reading = 0;
    
    //Multisampling
    int raw;
    
    for (int i = 0; i < NO_OF_SAMPLES; i++) {
        if (unit == ADC_UNIT_1) {
            adc_reading += adc1_get_raw((adc1_channel_t)channel);
        } else {
            adc2_get_raw((adc2_channel_t)channel, width, &raw);
            adc_reading += raw;
        }
    }
    adc_reading /= NO_OF_SAMPLES;
    //Convert adc_reading to voltage in mV
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
    
    uint32_t percentage = ((float)adc_reading)/((float)ADC_MAX) * 100;
    
    percentage = 100-percentage;
    
    printf("Raw: %d\tVoltage: %dmV\tPercentage: %d\n", adc_reading, voltage, percentage);
    
    close_sensor();
    return percentage;
}

bool check_humidity(){
    uint32_t raw_level = read_humidity();
    
    return raw_level > LEVEL_THRESHOLD; // Return True if level is enough, False otherwise.
}

homekit_value_t read_humidity_homekit(){
    
    if(!check_water_level()) // Sulama suyu hacmine gore uyari verip sulamayi basllatmayabilir!!!
        set_low_battery();
    else
        set_normal_battery();
    
    uint32_t raw_level = read_humidity();
    return HOMEKIT_FLOAT(raw_level);
}

bool set_low_battery(){
    humidity_sensor_battery.value = HOMEKIT_UINT8(LOW_BATTERY);
    homekit_characteristic_notify(&humidity_sensor_battery, humidity_sensor_battery.value);
    
    return true;
}

bool set_normal_battery(){
    humidity_sensor_battery.value = HOMEKIT_UINT8(NORMAL_BATTERY);
    homekit_characteristic_notify(&humidity_sensor_battery, humidity_sensor_battery.value);
    
    return true;
}

