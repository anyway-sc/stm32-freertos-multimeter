/*
 * label_pwr.c
 *
 *  Created on: Apr 9, 2026
 *      Author: liuyu
 */
#include "lcd.h"
#include "dmm.h"

// @作用：重绘稳压电源标签
void PwrLabel_Repaint(void)
{
	// 绘制矩形背景
	LCD_SetCursor(6,215); // 设置光标到矩形的左上角点
	LCD_SetBrush(0xcccccc); // 设置矩形的填充颜色
	LCD_FillRect(90,15);   // 填充矩形，宽90像素、高15像素

	// 获取数字万用表的数据
	DMMData_t xDmmData = xDMMGetData();

	// 打印测量结果
	LCD_SetPen(0x000000); // 设置黑色画笔
	LCD_SetCursor(18, 226);
	LCD_PrintString("Pwr:");
	LCD_PrintFloatSI(xDmmData.fVpwr, 5, "V");
}
