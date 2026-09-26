/*
 * panel_run_mode.c
 *
 *  Created on: Apr 28, 2026
 *      Author: liuyu
 */

#include "FreeRTOS.h"
#include "event_groups.h"
#include "lcd_task.h"
#include "lcd.h"

// 当前光标所在位置
static int iSelectedIndex = -1; // 负数表示未选中，0-第1个参数被选中

// 运行模式
static uint8_t ucRunMode = 0; // 运行模式，0 - 连续模式，1 - 单次模式

//
// @作用：重绘运行模式面板
//
void vRepaintRunModePanel(void)
{
	// 1. 标题栏，坐标（260，8），宽60、高15，颜色#FF7BFF
	LCD_SetBrush(0xFF7BFF);
	LCD_SetCursor(260,8);
	LCD_FillRect(60, 15);

	// 2. 标题文字，坐标（262, 21），颜色#000000
	LCD_SetPen(0x000000);
	LCD_SetCursor(262,21);
	LCD_PrintString("Run Mode");

	// 3. 面板背景，坐标(260, 23)，宽60、高35，颜色#CCCCCC
	LCD_SetCursor(260, 23);
	LCD_SetBrush(0xCCCCCC);
	LCD_FillRect(60, 35);

	// 4. 光标,(264,34)，宽51，高11，颜色#00FFFF
	if(iSelectedIndex == 0)
	{
		LCD_SetBrush(0x00ffff);
		LCD_SetCursor(262,34);
		LCD_FillRect(51, 11);
	}

	// 5. 参数1（运行模式），坐标（265, 43）
	LCD_SetCursor(265, 43);

	if(iSelectedIndex == 0)
	{
		LCD_SetBrush(0x00ffff); // 被选中时，文字使用天蓝色背景
	}
	else
	{
		LCD_SetBrush(0xCCCCCC); // 未被选中，文字使用浅灰色背景
	}

	if(ucRunMode == 0)
	{
		LCD_PrintString("Running");
	}
	else
	{
		LCD_PrintString("One-Shot");
	}
}

//
// @作用：选中运行模式面板，再次点击时切换光标位置
//
void vRunModePanelSelect(void)
{
	iSelectedIndex = 0; // 选中唯一的参数：运行模式

	extern EventGroupHandle_t xEventGroupForLCD;

	// 通过设置事件组的比特位，来触发信号发生器面板重绘
	xEventGroupSetBits(xEventGroupForLCD, RUNMODE_PANEL_REPAINT_BIT);
}

//
// @作用：取消选中运行模式面板
//
void vRunModePanelDeselect(void)
{
	if(iSelectedIndex >= 0)
	{
		// 1. 将变量iSelectedIndex的值设置为负数，表示取消选中
		iSelectedIndex = -1;

		extern EventGroupHandle_t xEventGroupForLCD;

		// 2. 通过设置事件组的比特位，来触发信号发生器面板重绘
		xEventGroupSetBits(xEventGroupForLCD, RUNMODE_PANEL_REPAINT_BIT);
	}
}

//
// @作用：通过KEY_UP和KEY_DOWN按钮调整运行模式面板的参数
// @参数：iDirection - 调整方向。+1 - 向上，对应KEY_UP，-1 - 向下，对应KEY_DOWN
//
void vRunModePanelAdjust(int iDirection)
{
	if(iSelectedIndex < 0) return; // 光标未选中，不做任何操作

	// 1. 如果参数1被选中，更改波形的形状
	if(iSelectedIndex == 0)
	{
		ucRunMode = (ucRunMode + iDirection + 2) % 2;
	}

	// 2. 通过设置事件组的比特位，来触发信号发生器面板重绘
	extern EventGroupHandle_t xEventGroupForLCD;
	xEventGroupSetBits(xEventGroupForLCD, RUNMODE_PANEL_REPAINT_BIT);
}
