/*
 * label_dmm.c
 *
 *  Created on: Apr 7, 2026
 *      Author: liuyu
 */

#include "lcd.h"
#include "dmm.h"

//
// @作用：重绘数字万用表标签
//
void DMMLabel_Repaint(void)
{
	// 绘制矩形背景
	LCD_SetCursor(110,215); // 设置光标到矩形的左上角点
	LCD_SetBrush(0xcccccc); // 设置矩形的填充颜色
	LCD_FillRect(120,15);   // 填充矩形，宽120像素、高15像素

	// 获取数字万用表的数据
	DMMData_t xDmmData = xDMMGetData();

	// 打印挡位信息
	const char *pccRangeStrs[] = {"DMM--2V:", "DMM-10V:", "DMM-50V:", "DMM--1k:", "DMM-10k:", "DMM100k:", "DMM--1M:"};

	LCD_SetCursor(116, 226); // 将光标设置到字符串的左下角
	LCD_SetPen(0x000000); // 设置画笔颜色为黑色

	const char *pcRangeStr = pccRangeStrs[xDmmData.eDMMRange]; // 获取挡位字符串

	LCD_PrintString(pcRangeStr); // 打印挡位信息

	// 确定单位到底是“V”还是“R”
	const char *pcUnit; // 单位字符串

	if(xDmmData.eDMMRange < eDMMRange1kOhm ) // 电压档
	{
		pcUnit = "V";
	}
	else // 电阻档
	{
		pcUnit = "R";
	}

	// 打印测量结果
	LCD_PrintFloatSI(xDmmData.fDMMValue, 6, pcUnit);
}
