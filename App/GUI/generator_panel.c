/*
 * panle_generator.c
 *
 *  Created on: Apr 27, 2026
 *      Author: liuyu
 */

#include <generator_panel.h>
#include "lcd.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "lcd_task.h"
#include "waveform_generator.h"

// 当前光标所在位置
static int iSelectedIndex = -1; // 负数表示未选中，0-第1个参数被选中，1-第2个参数被选中
// 信号发生器的波形类型
static WaveformShape_t eWaveformShape = WaveformShape_Off; // 默认关闭波形输出
// 信号发生器的频率
static float fWaveformFrequency = 1.0e3f; // 波形频率，默认1kHz

static const char* prvGetWavefromShapeString(WaveformShape_t eShape);

//
// @作用：重绘信号发生器面板
//
void vRepaintGeneratorPanel(void)
{
	// 1. 标题栏，坐标（260，184），宽60、高15，颜色#FF7BFF
	LCD_SetBrush(0xFF7BFF);
	LCD_SetCursor(260,184);
	LCD_FillRect(60, 15);

	// 2. 标题文字，坐标（262, 197），颜色#000000
	LCD_SetPen(0x000000);
	LCD_SetCursor(262,197);
	LCD_PrintString("Generato");

	// 3. 面板背景，坐标(260, 199)，宽60、高35，颜色#CCCCCC
	LCD_SetCursor(260, 199);
	LCD_SetBrush(0xCCCCCC);
	LCD_FillRect(60, 35);

	// 4. 光标，参数1被选中：(262,202)，宽58，高10，颜色#00FFFF
    //        参数2被选中：(262,212)，宽58，高10，颜色#00FFFF
	if(iSelectedIndex >= 0 && iSelectedIndex <= 1)
	{
		LCD_SetBrush(0x00ffff);

		if(iSelectedIndex == 0)
		{
			LCD_SetCursor(262,202);
		}
		else
		{
			LCD_SetCursor(262,212);
		}

		LCD_FillRect(58, 10);
	}

	// 5. 参数1，坐标（263，212）

	// 获取参数1的文本
	const char *pcParameter1String = prvGetWavefromShapeString(eWaveformShape);

	if(iSelectedIndex == 0)
	{
		LCD_SetBrush(0x00ffff); // 被选中时，文字使用天蓝色背景
	}
	else
	{
		LCD_SetBrush(0xCCCCCC); // 未被选中，文字使用浅灰色背景
	}

	LCD_SetCursor(263, 212);
	LCD_PrintString(pcParameter1String); // 打印出来

	// 6. 参数2，坐标（263，222）
	if(iSelectedIndex == 1)
	{
		LCD_SetBrush(0x00ffff); // 被选中时，文字使用天蓝色背景
	}
	else
	{
		LCD_SetBrush(0xCCCCCC); // 未被选中，文字使用浅灰色背景
	}

	LCD_SetCursor(263, 222);
	LCD_PrintFloatSI(fWaveformFrequency, 6, "Hz"); // 使用国际单位制绘制频率值
}

//
// @作用：获取波形的字符串名称
//
static const char* prvGetWavefromShapeString(WaveformShape_t eShape)
{
	const char *pcString = "Unknown";

	if(eShape == WaveformShape_Off)
	{
		pcString = "OFF"; // 关闭波形输出
	}
	else if(eShape == WaveformShape_Sin)
	{
		pcString = "Sin"; // 正弦波
	}
	else if(eShape == WaveformShape_Triangle)
	{
		pcString = "Triangle"; // 三角波
	}
	else if(eShape == WaveformShape_Square)
	{
		pcString = "Square"; // 方波
	}

	return pcString;
}

//
// @作用：选中信号发生器面板，再次点击时切换光标位置
//
void vGeneratorPanelSelect(void)
{
	if(iSelectedIndex < 0) // 未选中状态，按钮按下后光标选中第一个参数
	{
		iSelectedIndex = 0;
	}
	else // 已经选中，光标在参数之间切换
	{
		iSelectedIndex++;
		if(iSelectedIndex > 1)
		{
			iSelectedIndex = 0;
		}
	}

	extern EventGroupHandle_t xEventGroupForLCD;

	// 通过设置事件组的比特位，来触发信号发生器面板重绘
	xEventGroupSetBits(xEventGroupForLCD, GENERATOR_PANEL_REPAINT_BIT);
}

//
// @作用：取消选中信号发生器面板
//
void vGeneratorPanelDeselect(void)
{
	if(iSelectedIndex >= 0)
	{
		// 1. 将变量iSelectedIndex的值设置为负数，表示取消选中
		iSelectedIndex = -1;

		extern EventGroupHandle_t xEventGroupForLCD;

		// 2. 通过设置事件组的比特位，来触发信号发生器面板重绘
		xEventGroupSetBits(xEventGroupForLCD, GENERATOR_PANEL_REPAINT_BIT);
	}
}

//
// @作用：通过KEY_UP和KEY_DOWN按钮调整信号发生器面板的参数
// @参数：iDirection - 调整方向。+1 - 向上，对应KEY_UP，-1 - 向下，对应KEY_DOWN
//
void vGeneratorPanelAdjust(int iDirection)
{
	if(iSelectedIndex < 0) return; // 光标未选中，不做任何操作

	// 1. 如果参数1被选中，更改波形的形状
	if(iSelectedIndex == 0)
	{
		eWaveformShape = (eWaveformShape - iDirection + 4) % 4;

		WaveformGenerator_SetShape((uint8_t)eWaveformShape);
	}

	// 2. 如果参数2被选中，循环更改波形的频率
	if(iSelectedIndex == 1)
	{
		static const float freqs[] = {50.0f, 100.0f, 200.0f, 500.0f, 1.0e3f, 2.0e3f, 5.0e3f, 10.0e3f, 50.0e3f };
		static const int numOfFreqs = sizeof(freqs) / sizeof(float);

		int i = 0;
		// 通过一个for循环，定位到当前频率在数组当中的序号
		for(i=0; i<numOfFreqs; i++)
		{
			if(freqs[i] == fWaveformFrequency)
			{
				break;
			}
		}
		i = i+iDirection; // 计算下一个频率的值
		if(i>=numOfFreqs) i = numOfFreqs-1;
		if(i<0) i=0;
		fWaveformFrequency = freqs[i];

		WaveformGenerator_SetFrequency(fWaveformFrequency);
	}

	// 3. 通过设置事件组的比特位，来触发信号发生器面板重绘
	extern EventGroupHandle_t xEventGroupForLCD;
	xEventGroupSetBits(xEventGroupForLCD, GENERATOR_PANEL_REPAINT_BIT);
}
