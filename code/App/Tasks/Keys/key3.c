/*
 * key1.c
 *
 *  Created on: Mar 21, 2026
 *      Author: liuyu
 */

#include "key.h"
#include <string.h>
#include "main.h"
#include "buzzer_task.h"
#include "run_mode_panel.h"
#include "scale_panel.h"
#include "generator_panel.h"
#include "cursor_panel.h"

Key_HandleTypeDef key3; // KEY3的句柄

static GPIO_PinState KEY3_ReadPinCallback(void);
static void KEY3_ClickedCallback(void);

void Key3_Init(void)
{
	key3.Init.ReadPinCallback = KEY3_ReadPinCallback;
	key3.Init.ClickedCallback = KEY3_ClickedCallback;
	Key_Init(&key3);
}

void Key3_Scan(void)
{
	Key_Scan(&key3);
}

static GPIO_PinState KEY3_ReadPinCallback(void)
{
	return HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin);
}

static void KEY3_ClickedCallback(void)
{
	Buzzer_Beep();
	vRunModePanelDeselect();
	vScalePanelDeselect();
	SelectCursorPanel();
	vGeneratorPanelDeselect();
}
