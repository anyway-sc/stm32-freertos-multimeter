/*
 * WaveformPanel.c
 *
 *  Created on: May 1, 2026
 *      Author: liuyu
 */

#include "FreeRTOS.h"
#include "semphr.h"
#include "lcd.h"
#include "scale_panel.h"
#include "cursor_panel.h"
#include "waveform_capture.h"
#include "dmm.h"

// 重绘波形面板
void RepaintWaveformPanel(void)
{
	WaveformParamTypeDef waveformParam = {0};
	extern SemaphoreHandle_t xMutexForWaveform; // 波形缓冲区互斥锁

	waveformParam.Waveform = WaveformCapture_GetWaveform();
	waveformParam.WaveLength = 1024;
	// waveformParam.SampleRate = 10e3; // 先固定10k采样率
	waveformParam.SampleRate = WaveformCapture_GetSampleRate(); // 动态获取采样率的大小
	waveformParam.Trigger = xDMMGetData().fTriggerLevel;
	waveformParam.XScale = fGetXScale();
	waveformParam.YScale = fGetYScale();
	waveformParam.CursorX1Pct = GetCursorX1Pct();
	waveformParam.CursorX2Pct = GetCursorX2Pct();
	waveformParam.CursorY1Pct = GetCursorY1Pct();
	waveformParam.CursorY2Pct = GetCursorY2Pct();

	xSemaphoreTake(xMutexForWaveform, portMAX_DELAY); // 画图之前获取锁
	LCD_DrawWaveform(&waveformParam);
	xSemaphoreGive(xMutexForWaveform); // 画图之后归还锁
}
