/*
 * key_task.c
 *
 *  Created on: Mar 20, 2026
 *      Author: liuyu
 */
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "key1.h"
#include "key2.h"
#include "key3.h"
#include "key4.h"
#include "key_up.h"
#include "key_down.h"

void vKeyTask(void *pvParameters)
{
	Key1_Init();
	Key2_Init();
	Key3_Init();
	Key4_Init();
	KeyUp_Init();
	KeyDown_Init();

	for(;;)
	{
		Key1_Scan();
		Key2_Scan();
		Key3_Scan();
		Key4_Scan();
		KeyUp_Scan();
		KeyDown_Scan();
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}
