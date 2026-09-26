/*
 * led_task.c
 *
 *  Created on: Mar 28, 2026
 *      Author: liuyu
 */

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

void vLEDTask(void *pvParameters)
{
	for(;;)
	{
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
		vTaskDelay(pdMS_TO_TICKS(100));
		HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}
