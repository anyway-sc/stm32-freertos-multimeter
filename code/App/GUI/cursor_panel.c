/*
 * cursor_panel.c
 *
 *  Created on: May 1, 2026
 *      Author: liuyu
 */
#include "FreeRTOS.h"
#include "lcd.h"
#include "lcd_task.h"

static int cursorPos = -1; // 光标位置，负数表示未选中，0表示第1个参数，1表示第2个参数，....
static int8_t cursor_x1_pct = 0; // 游标x1的当前位置，以百分比表示，范围0%~100%
static int8_t cursor_x2_pct = 0; // 游标x2的当前位置，以百分比表示，范围0%~100%
static int8_t cursor_y1_pct = 0; // 游标y1的当前位置，以百分比表示，范围-100%~+100%
static int8_t cursor_y2_pct = 0; // 游标y2的当前位置，以百分比表示，范围-100%~+100%

// @作用：重绘游标面板
void RepaintCursorPanel(void)
{
	// 1. 洋红色标题栏，坐标（260，127），宽60、高15，颜色#FF7BFF
	LCD_SetBrush(0xFF7BFF);
	LCD_SetCursor(260, 127);
	LCD_FillRect(60, 15);

	// 2. 标题文字，坐标（272，140），颜色#000000
	LCD_SetPen(0x000000);
	LCD_SetCursor(272, 140);
	LCD_PrintString("Cursor");

	// 3. 浅灰色面板，坐标（260，142），宽60，高35，颜色#CCCCCC
	LCD_SetCursor(260, 142);
	LCD_SetBrush(0xCCCCCC);
	LCD_FillRect(60, 35);

	// 4. 参数1，游标x1，坐标（272，158）
	if(cursorPos == 0) // 参数X1被选中
	{
		// 绘制光标（271，148），宽16、高10
		LCD_SetBrush(0x00FFFF);
		LCD_SetCursor(271, 148);
		LCD_FillRect(16, 10);
	}
	else
	{
		LCD_SetBrush(0xCCCCCC);
	}
	LCD_SetCursor(272, 158);
	LCD_PrintString("x1");

	// 5. 参数2，游标x2，坐标（293，158）
	if(cursorPos == 1) // 参数X2被选中
	{
		// 绘制光标（292，148），宽16、高10
		LCD_SetBrush(0x00FFFF);
		LCD_SetCursor(292, 148);
		LCD_FillRect(16, 10);
	}
	else
	{
		LCD_SetBrush(0xCCCCCC);
	}
	LCD_SetCursor(293, 158);
	LCD_PrintString("x2");

	// 6. 参数3，游标y1，坐标（272，171）
	if(cursorPos == 2) // 参数y1被选中
	{
		// 绘制光标（271，161），宽16、高10
		LCD_SetBrush(0x00FFFF);
		LCD_SetCursor(271, 161);
		LCD_FillRect(16, 10);
	}
	else
	{
		LCD_SetBrush(0xCCCCCC);
	}
	LCD_SetCursor(272, 171);
	LCD_PrintString("y1");

	// 7. 参数4，游标y2，坐标（293，171）
	if(cursorPos == 3) // 参数y2被选中
	{
		// 绘制光标（292，161），宽16、高10
		LCD_SetBrush(0x00FFFF);
		LCD_SetCursor(292, 161);
		LCD_FillRect(16, 10);
	}
	else
	{
		LCD_SetBrush(0xCCCCCC);
	}
	LCD_SetCursor(293, 171);
	LCD_PrintString("y2");
}

// @作用：选中游标面板
void SelectCursorPanel(void)
{
	if(cursorPos < 0) // 未选中状态，按钮按下后光标选中第一个参数
	{
		cursorPos = 0;
	}
	else // 已经选中，光标在参数之间切换
	{
		cursorPos = (cursorPos + 1 + 4) % 4;
	}

	extern EventGroupHandle_t xEventGroupForLCD;

	// 通过设置事件组的比特位，来触发信号发生器面板重绘
	xEventGroupSetBits(xEventGroupForLCD, CURSOR_PANEL_REPAINT_BIT);
}

// @作用：取消选中游标面板
void DeselectCursorPanel(void)
{
	if(cursorPos >= 0)
	{
		// 1. 将变量iSelectedIndex的值设置为负数，表示取消选中
		cursorPos = -1;

		extern EventGroupHandle_t xEventGroupForLCD;

		// 2. 通过设置事件组的比特位，来触发信号发生器面板重绘
		xEventGroupSetBits(xEventGroupForLCD, CURSOR_PANEL_REPAINT_BIT);
	}
}

// @作用：调整参数
// @参数：iDirection - 调整方向。+1 - 向上，对应KEY_UP，-1 - 向下，对应KEY_DOWN
void AdjustCursorPanel(int iDirection)
{
	if(cursorPos < 0) return; // 光标未选中，不做任何操作

	// 1. 如果参数1被选中 cursor_x1
	if(cursorPos == 0)
	{
		cursor_x1_pct -= iDirection;

		if(cursor_x1_pct > 100) cursor_x1_pct = 100;
		if(cursor_x1_pct < 0) cursor_x1_pct = 0;
	}

	// 2. 如果参数2被选中 cursor_x2
	if(cursorPos == 1)
	{
		cursor_x2_pct -= iDirection;

		if(cursor_x2_pct > 100) cursor_x2_pct = 100;
		if(cursor_x2_pct < 0) cursor_x2_pct = 0;
	}

	// 3. 如果参数3被选中 cursor_y1
	if(cursorPos == 2)
	{
		cursor_y1_pct += iDirection;

		if(cursor_y1_pct > 100) cursor_y1_pct = 100;
		if(cursor_y1_pct < -100) cursor_y1_pct = -100;
	}

	// 4. 如果参数4被选中 cursor_y2
	if(cursorPos == 3)
	{
		cursor_y2_pct += iDirection;

		if(cursor_y2_pct > 100) cursor_y2_pct = 100;
		if(cursor_y2_pct < -100) cursor_y2_pct = -100;
	}

	// 5. 通过设置事件组的比特位，来触发游标面板重绘
	extern EventGroupHandle_t xEventGroupForLCD;
	xEventGroupSetBits(xEventGroupForLCD, CURSOR_PANEL_REPAINT_BIT | WAVEFORM_PANEL_REPAINT_BIT | CURSOR_LABEL_REPAINT_BIT);
}

// 获取游标X1的百分比
int8_t GetCursorX1Pct(void)
{
	return cursor_x1_pct;
}

// 获取游标X2的百分比
int8_t GetCursorX2Pct(void)
{
	return cursor_x2_pct;
}

// 获取游标Y1的百分比
int8_t GetCursorY1Pct(void)
{
	return cursor_y1_pct;
}

// 获取游标Y2的百分比
int8_t GetCursorY2Pct(void)
{
	return cursor_y2_pct;
}
