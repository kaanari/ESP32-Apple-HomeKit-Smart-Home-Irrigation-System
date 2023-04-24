//
//  water_level_sensor.c
//  
//
//  Created by Kaan Arı on 16.07.2021.
//

#include "water_level_sensor.h"

static esp_adc_cal_characteristics_t *adc_chars;
#if CONFIG_IDF_TARGET_ESP32
static const adc_channel_t channel = ADC_CHANNEL_6;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
#define ADC_MAX 4095
#elif CONFIG_IDF_TARGET_ESP32S2
static const adc_channel_t channel = ADC_CHANNEL_6;     // GPIO7 if ADC1, GPIO17 if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_13;
#define ADC_MAX 8191
#endif
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;

#define SENSOR_POWER_PIN 25

void set(homekit_value_t value){
    printf("Try");
}

homekit_characteristic_t level_name = HOMEKIT_CHARACTERISTIC_(NAME,"Humidity Sensor");
homekit_characteristic_t level_active = HOMEKIT_CHARACTERISTIC_(ACTIVE, LEVEL_SENSOR_ACTIVE,.setter = set);
homekit_characteristic_t level_humidity = HOMEKIT_CHARACTERISTIC_(CURRENT_RELATIVE_HUMIDITY, 50);
homekit_characteristic_t level_humidifier = HOMEKIT_CHARACTERISTIC_(CURRENT_HUMIDIFIER_DEHUMIDIFIER_STATE, 0);
homekit_characteristic_t level_target_humidifier = HOMEKIT_CHARACTERISTIC_(TARGET_HUMIDIFIER_DEHUMIDIFIER_STATE, 0);
homekit_characteristic_t level_water_level = HOMEKIT_CHARACTERISTIC_(WATER_LEVEL,30);


homekit_service_t humidity_service = HOMEKIT_SERVICE_(HUMIDIFIER_DEHUMIDIFIER, .primary=false, .characteristics=(homekit_characteristic_t*[]){
    &level_name,
    &level_active,/*   */
    &level_humidity,
    &level_humidifier,
    &level_target_humidifier,
    &level_water_level,
    NULL
});


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

static void open_sensor() {
    gpio_set_level(SENSOR_POWER_PIN, 1);
}

static void close_sensor() {
    gpio_set_level(SENSOR_POWER_PIN, 0);
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

bool initialize_water_level_sensor(){
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

uint32_t read_water_level(){
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
    
    printf("Raw: %d\tVoltage: %dmV\tPercentage: %d\n", adc_reading, voltage, percentage);
    
    close_sensor();
    return percentage;
}

bool check_water_level(){
    uint32_t raw_level = read_water_level();
    
    return raw_level > LEVEL_THRESHOLD; // Return True if level isss enough, False otherwise.
}

