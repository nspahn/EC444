#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "driver/uart.h"
#include "esp_adc_cal.h"
#include "sdkconfig.h"
#include "esp_types.h"

#define ECHO_TEST_TXD  (GPIO_NUM_4)
#define ECHO_TEST_RXD  (GPIO_NUM_5)
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE (1024)

#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   20          //Multisampling

int flag = 0;

float ultraDist = 0;                                            // Ultrasonic Sensor distance.

static esp_adc_cal_characteristics_t *adc_chars;
static const adc_channel_t channel1 = ADC_CHANNEL_6;            // Ultrasonic Sensor.
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;

static void check_efuse(void)
{
    //Check TP is burned into eFuse
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
}

static void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    }
}

static void uartDis(void *arg)
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
    uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);

    while (1)
    {                                 
        char serialOut[10];                                         // Array for serial output.

        if (!flag)
            sprintf(serialOut, "%.2f\n", ultraDist);
        uart_write_bytes(UART_NUM_1, (const char*) serialOut, strlen(serialOut)); // Write back to UART.
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}

static void ultraS(void *arg)
{
    //Configure ADC
    if (unit == ADC_UNIT_1) {

        adc1_config_channel_atten(channel1, atten);
    } else {
        adc2_config_channel_atten((adc2_channel_t)channel1, atten);
    }

    //Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);

    //Continuously sample ADC1
    while (1) {
        uint32_t adc_reading = 0;
        if(flag == 0) {
            //Multisampling
            for (int i = 0; i < NO_OF_SAMPLES; i++) {               // 20 samples.
                if (unit == ADC_UNIT_1) {
                    adc_reading += adc1_get_raw((adc1_channel_t)channel1);
                } else {
                    int raw;
                    adc2_get_raw((adc2_channel_t)channel1, ADC_WIDTH_BIT_12, &raw);
                    adc_reading += raw;
                }
                vTaskDelay(100/portTICK_PERIOD_MS);                 // Sampling about every 100 ms.
            }
            adc_reading /= NO_OF_SAMPLES;                           // 20 samples in 2 seconds average.
            //Convert adc_reading to voltage in mV
            uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
            // Calculation of distance
            ultraDist = ((voltage / .6445) / 1000) * 2.3857;
            printf("0,%.2f\n", ultraDist);
            flag++;
            vTaskDelay(10/portTICK_PERIOD_MS);
        }
    }
}

void app_main(void)
{
    //Check if Two Point or Vref are burned into eFuse
    check_efuse();

    xTaskCreate(uartDis, "uartDis", 4096, NULL, configMAX_PRIORITIES, NULL);    // UART write task.
    xTaskCreate(ultraS, "ultraS", 4096, NULL, 5, NULL);                         // Ultrasonic sensor task.

}
