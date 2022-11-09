#include <stdio.h>
// Libraries needed for FreeRTOS usage.
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Needed to configure various options for scheduling.
#include "FreeRTOSConfig.h"

// Needed for idle task hooks.
#include "esp_freertos_hooks.h"

// Needed for GPIO function.
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

// Macro defining the GPIO for our input button.
#define BUTTON_GPIO 18

// Constant parameter representing active low logic level.
const int active_low = 0;

void idleHook(void)
{
    printf("Idle hook!\n");
}

void customerComes(void* parameter)
{
    // Select pin 18 as a GPIO pin.
    gpio_pad_select_gpio(BUTTON_GPIO);

    // Specify that the pin is an input pin.
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
    // Specify the pull mode as pullup.
    gpio_set_pull_mode(BUTTON_GPIO, GPIO_PULLUP_ONLY);

    while (1)
    {
        while (gpio_get_level(BUTTON_GPIO) == active_low)
        {
            printf("A customer came!\n");
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    // Delete the task. This will never be reached.
    vTaskDelete(NULL);
}

void lookAtPhone(void* pvParameters)
{
    int counter = 50;
    while (1)
    {
        printf("The server is looking at her phone!\n");
        counter--;
        if (counter <= 0)
        {
            counter = 50;
            vTaskDelay(1000 / portTICK_RATE_MS);
        }
    }

    vTaskDelete(NULL);
}

void chitchat(void* pvParameters)
{
    while (1)
    {
        printf("The server is chit-chatting!!\n");
        if (gpio_get_level(BUTTON_GPIO) == active_low)
        {
            printf("A customer came, but the server is still chatting!\n");
        }
    }
    vTaskDelete(NULL);
}

void app_main(void)
{
    xTaskCreatePinnedToCore(lookAtPhone, "Task0", 1024, NULL, 0, NULL, 0);
    xTaskCreatePinnedToCore(chitchat, "Task1", 1024, NULL, 0, NULL, 0);
    xTaskCreatePinnedToCore(customerComes, "customerComes", 1024, NULL, 2, NULL, 0);
    //esp_register_freertos_idle_hook_for_cpu(idleHook, 0);
}
