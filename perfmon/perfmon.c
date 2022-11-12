#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "sdkconfig.h"

#define MAX_COUNT 70

// Counter variable for CPU0.
static int idle0Calls = 0;
// Counter variable for CPU1.
static int maxIdleCalls = 0;

// Idle task took for CPU0.
void idle_task_0()
{
	idle0Calls += 1;
}

// Idle task hook for CPU1.
void idle_task_1()
{
	maxIdleCalls += 1;
}

// Performance monitor task.
void perfmon_task(void *pvParameters)
{
	while (1)
	{
		// Save the number of cakks and reset them.
		float idle0 = (float)idle0Calls;
		float idle1 = (float)maxIdleCalls;
		idle0Calls = 0;
		maxIdleCalls = 0;

		float cpu0 = 100.f;
		if (idle1 != 0)
		{
			// CPU usage = 100% - (number of real calls/maximum number of calls)
			cpu0 = 100.f - ((idle0 / idle1) * 100.f);
		}
		float cpu1 = 0.f;
		// printf("Idle0 is: %f\n", idle0);
		// printf("Idle1 is: %f\n", idle1);
        printf("Core 0 at %f%%\n", cpu0);
        printf("Core 1 at %f%%\n", cpu1);
		// TODO configurable delay
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}

// This task is to increase the workload on CPU0.
void taskForCPU0(void)
{
	int counter = MAX_COUNT;
	while (1)
	{
		counter--;
		printf("Printing something will take time\n");
		if (counter <= 0)
		{
			counter = MAX_COUNT;
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}
	}
}

void app_main()
{
	esp_register_freertos_idle_hook_for_cpu(idle_task_0, 0);
	esp_register_freertos_idle_hook_for_cpu(idle_task_1, 1);
	// TODO calculate optimal stack size
	xTaskCreate(perfmon_task, "perfmon", 2048, NULL, 2, NULL);
	xTaskCreatePinnedToCore(taskForCPU0, "taskForCPU0", 2048, NULL, 1, NULL, 0);
}
