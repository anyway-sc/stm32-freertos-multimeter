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

Key_HandleTypeDef key1; // KEY1的句柄

static GPIO_PinState KEY1_ReadPinCallback(void);
static void KEY1_ClickedCallback(void);

void Key1_Init(void)
{
	key1.Init.ReadPinCallback = KEY1_ReadPinCallback;
	key1.Init.ClickedCallback = KEY1_ClickedCallback;
	Key_Init(&key1);
}

void Key1_Scan(void)
{
	Key_Scan(&key1);
}

static void KEY1_ClickedCallback(void)
{
	Buzzer_Beep();
	vRunModePanelSelect();
	vScalePanelDeselect();
	DeselectCursorPanel();
	vGeneratorPanelDeselect();
}

static GPIO_PinState KEY1_ReadPinCallback(void)
{
	return HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin);
}
