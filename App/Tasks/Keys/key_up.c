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

Key_HandleTypeDef keyUp;   // KEY_UP的句柄

static GPIO_PinState KEYUP_ReadPinCallback(void);
static void KEYUP_ClickedCallback(void);

void KeyUp_Init(void)
{
	keyUp.Init.ReadPinCallback = KEYUP_ReadPinCallback;
	keyUp.Init.ClickedCallback = KEYUP_ClickedCallback;
	Key_Init(&keyUp);
}

void KeyUp_Scan(void)
{
	Key_Scan(&keyUp);
}

static GPIO_PinState KEYUP_ReadPinCallback(void)
{
	return HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin);
}

static void KEYUP_ClickedCallback(void)
{
	Buzzer_Beep();
	vRunModePanelAdjust(+1);
	vScalePanelAdjust(+1);
	AdjustCursorPanel(+1);
	vGeneratorPanelAdjust(+1);
}
