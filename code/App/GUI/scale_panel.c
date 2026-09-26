/*
 * scale_panel.c
 *
 *  Created on: Apr 29, 2026
 *      Author: liuyu
 */

#include "lcd.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "lcd_task.h"
#include "waveform_capture.h"

int iSelectedIndex = -1; // 光标位置，负数表示未转中，0-x轴缩放比，1-y轴缩放比

// x轴缩放比
static int16_t xScaleIndex = 5; // 当前挡位
static const float xScales[] = { 10.0e-6f, 20.0e-6f, 50.0e-6f, 100.0e-6f, 200.0e-6f, 500.0e-6f, 1.0e-3f, 2.0e-3f, 5.0e-3f, 10.0e-3f, 20.0e-3f, 50.0e-3f};
static const uint16_t numOfXScales = sizeof(xScales) / sizeof(float); // 挡位总数

// y轴缩放比
static int16_t yScaleIndex = 4; // 当前挡位
static const float yScales[] = { 50.0e-3f, 100.0e-3f, 200.0e-3f, 500.0e-3f, 1.0f };
static const uint16_t numOfYScales = sizeof(yScales) / sizeof(float); // 挡位总数

//
// @作用：重绘信号发生器面板
//
void vRepaintScalePanel(void)
{
	// 1. 标题栏，坐标（260, 67），宽60、高15，颜色#FF7BFF
	LCD_SetBrush(0xFF7BFF);
	LCD_SetCursor(260, 67);
	LCD_FillRect(60, 15);

	// 2. 标题文字，坐标（275, 80），颜色#000000
	LCD_SetPen(0x000000);
	LCD_SetCursor(275, 80);
	LCD_PrintString("Scale");

	// 3. 面板背景，坐标(260, 82)，宽60、高35，颜色#CCCCCC
	LCD_SetCursor(260, 82);
	LCD_SetBrush(0xCCCCCC);
	LCD_FillRect(60, 35);

	// 4. 绘制参数1，x
	if(iSelectedIndex == 0) // 参数X1被选中
	{
		// 绘制光标，(274, 89)，宽9，高11，#00ffff
		LCD_SetBrush(0x00FFFF);
		LCD_SetCursor(274, 89);
		LCD_FillRect(9, 11);
	}
	else
	{
		LCD_SetBrush(0xCCCCCC);
	}
	LCD_SetCursor(275, 98);
	LCD_PrintString("x");

	// 5. 绘制参数2，y
	if(iSelectedIndex == 1) // 参数X1被选中
	{
		// 绘制光标，(294, 89)，宽9，高11，#00ffff
		LCD_SetBrush(0x00FFFF);
		LCD_SetCursor(294, 89);
		LCD_FillRect(9, 11);
	}
	else
	{
		LCD_SetBrush(0xCCCCCC);
	}
	LCD_SetCursor(295, 98);
	LCD_PrintString("y");
}

//
// @作用：选中信号发生器面板，再次点击时切换光标位置
//
void vScalePanelSelect(void)
{
	if(iSelectedIndex < 0) // 未选中状态，按钮按下后光标选中第一个参数
	{
		iSelectedIndex = 0;
	}
	else // 已经选中，光标在参数之间切换
	{
		iSelectedIndex = (iSelectedIndex + 1 + 2) % 2;
	}

	extern EventGroupHandle_t xEventGroupForLCD;

	// 通过设置事件组的比特位，来触发信号发生器面板重绘
	xEventGroupSetBits(xEventGroupForLCD, SCALE_PANEL_REPAINT_BIT);
}

//
// @作用：取消选中信号发生器面板
//
void vScalePanelDeselect(void)
{
	if(iSelectedIndex >= 0)
	{
		// 1. 将变量iSelectedIndex的值设置为负数，表示取消选中
		iSelectedIndex = -1;

		extern EventGroupHandle_t xEventGroupForLCD;

		// 2. 通过设置事件组的比特位，来触发信号发生器面板重绘
		xEventGroupSetBits(xEventGroupForLCD, SCALE_PANEL_REPAINT_BIT);
	}
}

//
// @作用：通过KEY_UP和KEY_DOWN按钮调整信号发生器面板的参数
// @参数：iDirection - 调整方向。+1 - 向上，对应KEY_UP，-1 - 向下，对应KEY_DOWN
//
void vScalePanelAdjust(int iDirection)
{
	if(iSelectedIndex < 0) return; // 光标未选中，不做任何操作

	// 1. 如果参数1被选中 XScale
	if(iSelectedIndex == 0)
	{
		xScaleIndex += iDirection;

		if(xScaleIndex >= numOfXScales) xScaleIndex = numOfXScales - 1;
		if(xScaleIndex < 0) xScaleIndex = 0;

		static const uint16_t PSCs[] = {0,0,0,0,0,0,0,0,0,0,0,0};
		static const uint16_t ARRs[] = {143, 143, 143, 143, 225, 561, 1124, 2251, 5624, 11249, 22549, 56249};

		// 根据xScale的值设置采样率
		WaveformCapture_SetSampleRate(PSCs[xScaleIndex], ARRs[xScaleIndex]);
	}

	// 2. 如果参数2被选中 YScale
	if(iSelectedIndex == 1)
	{
		yScaleIndex += iDirection;

		if(yScaleIndex >= numOfYScales) yScaleIndex = numOfYScales - 1;
		if(yScaleIndex < 0) yScaleIndex = 0;
	}

	// 3. 通过设置事件组的比特位，来触发信号发生器面板重绘
	extern EventGroupHandle_t xEventGroupForLCD;
	xEventGroupSetBits(xEventGroupForLCD, SCALE_PANEL_REPAINT_BIT | SCALE_LABEL_REPAINT_BIT | WAVEFORM_PANEL_REPAINT_BIT | CURSOR_LABEL_REPAINT_BIT);
}

//
// @作用：获取X轴向的缩放比
//
float fGetXScale(void)
{
	return xScales[xScaleIndex];
}

//
// @作用：获取Y轴向的缩放比
//
float fGetYScale(void)
{
	return yScales[yScaleIndex];
}
