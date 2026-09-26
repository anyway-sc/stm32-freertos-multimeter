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

Key_HandleTypeDef key2; // KEY2的句柄

static GPIO_PinState KEY2_ReadPinCallback(void);
static void KEY2_ClickedCallback(void);

void Key2_Init(void)
{
	key2.Init.ReadPinCallback = KEY2_ReadPinCallback;
	key2.Init.ClickedCallback = KEY2_ClickedCallback;
	Key_Init(&key2);
}

void Key2_Scan(void)
{
	Key_Scan(&key2);
}

static GPIO_PinState KEY2_ReadPinCallback(void)
{
	return HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin);
}

static void KEY2_ClickedCallback(void)
{
	Buzzer_Beep();
	vRunModePanelDeselect();
	vScalePanelSelect();
	DeselectCursorPanel();
	vGeneratorPanelDeselect();
}
