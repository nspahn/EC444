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

float IRdist = 0;                                               // IR Rangefinder distance.

static esp_adc_cal_characteristics_t *adc_chars;
static const adc_channel_t channel2 = ADC_CHANNEL_3;            // IR Rangefinder.
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
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
            sprintf(serialOut, "%.2f\n", IRdist);

        uart_write_bytes(UART_NUM_1, (const char*) serialOut, strlen(serialOut)); // Write back to UART.
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}

static void IR(void *arg)
{
    //Configure ADC
    if (unit == ADC_UNIT_1) {
        adc1_config_width(ADC_WIDTH_BIT_12);
        adc1_config_channel_atten(channel2, atten);
    } else {
        adc2_config_channel_atten((adc2_channel_t)channel2, atten);
    }

    //Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);

    //Continuously sample ADC1
    while (1) {
        uint32_t adc_reading = 0;
        if(flag == 1) {
            //Multisampling
            for (int i = 0; i < NO_OF_SAMPLES; i++) {
                if (unit == ADC_UNIT_1) 
                {
                    adc_reading += adc1_get_raw((adc1_channel_t)channel2);
                } else {
                    int raw;
                    adc2_get_raw((adc2_channel_t)channel2, width, &raw);
                    adc_reading += raw;
                }
            }
            adc_reading /= NO_OF_SAMPLES;
            //Convert adc_reading to voltage in mV
            float voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
            voltage /= 1000;
            
            // Callibration on page 5 from https://www.sparkfun.com/datasheets/Sensors/Infrared/gp2y0a02yk_e.pdf
            if (voltage > 0.4 && voltage < 0.5)                     
                IRdist = 1.9 - voltage;                             
            else if (voltage >= 0.5 && voltage < 0.6)
                IRdist = (1.2 - voltage) / 0.5;
            else if (voltage >= 0.6 && voltage < 0.65)
                IRdist = (1.2 - voltage) / 0.5;
            else if (voltage >= 0.65 && voltage < 0.75)
                IRdist = (1.25 - voltage) / 0.5;
            else if (voltage >= 0.75 && voltage < 0.8)
                IRdist = (1.2 - voltage) / 0.5;
            else if (voltage >= 0.8 && voltage < 0.9)
                IRdist = 1.6 - voltage;
            else if (voltage >= 0.9 && voltage < 1.05)
                IRdist = (1.95 - voltage) / 1.5;
            else if (voltage >= 1.05 && voltage < 1.25)
                IRdist = (2.25 - voltage) / 2;
            else if (voltage >= 1.25 && voltage < 1.55)
                IRdist = (2.75 - voltage) / 3;
            else if (voltage >= 1.55 && voltage < 2)
                IRdist = (3.35 - voltage) / 4.5;
            else if (voltage >= 2 && voltage < 2.55)
                IRdist = (3.65 - voltage) / 5.5;
            else if (voltage >= 2.55 && voltage < 2.8)
                IRdist = (3.05 - voltage) / 2.5;
            else
                IRdist = 0;

            printf("1,%.2f\n", IRdist);
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
    xTaskCreate(IR, "IR", 4096, NULL, 4, NULL);                                 // IR Rangefinder task.

}
