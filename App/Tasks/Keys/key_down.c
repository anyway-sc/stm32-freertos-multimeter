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

Key_HandleTypeDef keyDown; // KEY_DOWN的句柄

static GPIO_PinState KEYDOWN_ReadPinCallback(void);
static void KEYDOWN_ClickedCallback(void);

void KeyDown_Init(void)
{
	keyDown.Init.ReadPinCallback = KEYDOWN_ReadPinCallback;
	keyDown.Init.ClickedCallback = KEYDOWN_ClickedCallback;
	Key_Init(&keyDown);
}

void KeyDown_Scan(void)
{
	Key_Scan(&keyDown);
}

static GPIO_PinState KEYDOWN_ReadPinCallback(void)
{
	return HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin);
}

static void KEYDOWN_ClickedCallback(void)
{
	Buzzer_Beep();
	vRunModePanelAdjust(-1);
	vScalePanelAdjust(-1);
	AdjustCursorPanel(-1);
	vGeneratorPanelAdjust(-1);
}
