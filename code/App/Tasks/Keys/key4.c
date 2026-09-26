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

Key_HandleTypeDef key4; // KEY4的句柄

static GPIO_PinState KEY4_ReadPinCallback(void);
static void KEY4_ClickedCallback(void);

void Key4_Init(void)
{
	key4.Init.ReadPinCallback = KEY4_ReadPinCallback;
	key4.Init.ClickedCallback = KEY4_ClickedCallback;
	Key_Init(&key4);
}

void Key4_Scan(void)
{
	Key_Scan(&key4);
}

static GPIO_PinState KEY4_ReadPinCallback(void)
{
	return HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY4_Pin);
}

//
// @作用：KEY4的回调函数
//
static void KEY4_ClickedCallback(void)
{
	Buzzer_Beep();
	vRunModePanelDeselect();
	vScalePanelDeselect();
	DeselectCursorPanel();
	vGeneratorPanelSelect();
}
