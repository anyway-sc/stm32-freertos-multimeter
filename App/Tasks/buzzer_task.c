/*
 * buzzer_task.c
 *
 *  Created on: Mar 21, 2026
 *      Author: liuyu
 */


#include "FreeRTOS.h"
#include "semphr.h"
#include "main.h"

static SemaphoreHandle_t xBinarySempahoreForBuzzer;

void Buzzer_Init(void)
{
	// xBinarySempahoreForBuzzer = xSemaphoreCreateBianry();
	xBinarySempahoreForBuzzer = xSemaphoreCreateCounting(5, 0);
}

void vBuzzerTask(void *pvParameters)
{
	for(;;)
	{
		xSemaphoreTake(xBinarySempahoreForBuzzer, portMAX_DELAY);

//		HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
//		vTaskDelay(pdMS_TO_TICKS(100));
//		HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
//		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

void Buzzer_Beep(void)
{
	xSemaphoreGive(xBinarySempahoreForBuzzer);
}
