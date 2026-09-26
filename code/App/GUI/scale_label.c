/*
 * scale_label.c
 *
 *  Created on: Apr 29, 2026
 *      Author: liuyu
 */

#include "lcd.h"
#include "scale_panel.h"

//
// @作用：重绘缩放标签
//
void RepaintScaleLabel(void)
{
	// 1. 暗黄色背景，坐标（187，6），宽60、高25，颜色#DAA400
	LCD_SetCursor(187, 6);
	LCD_SetBrush(0xDAA400);
	LCD_FillRect(60, 25);

	// 2. X轴向缩放比，坐标（195，18），颜色#000000
	// 通过缩放面板获取X轴向缩放比
	float xScale = fGetXScale();
	// 打印X轴向缩放比
	LCD_SetCursor(195, 18);
	LCD_SetPen(0x000000);
	LCD_PrintFloatSI(xScale, 5, "s");

	// 3. Y轴向缩放比，坐标（195，29），颜色#000000
	// 通过缩放面板获取Y轴向缩放比
	float yScale = fGetYScale();
	// 打印Y轴向缩放比
	LCD_SetCursor(195, 29);
	LCD_SetPen(0x000000);
	LCD_PrintFloatSI(yScale, 5, "V");
}
