/*
 * panel_generator.h
 *
 *  Created on: Apr 27, 2026
 *      Author: liuyu
 */

#ifndef GUI_GENERATOR_PANEL_H_
#define GUI_GENERATOR_PANEL_H_

#include "FreeRTOS.h"
#include "event_groups.h"

// 波形类型
typedef enum{
	WaveformShape_Off, // 关闭波形输出
	WaveformShape_Sin, // 正弦波
	WaveformShape_Triangle, // 三角波
	WaveformShape_Square, // 方波
}
WaveformShape_t;

void vRepaintGeneratorPanel(void);
void vGeneratorPanelSelect(void); // 选中信号发生器面板
void vGeneratorPanelDeselect(void); // 取消选中信号发生器面板
void vGeneratorPanelAdjust(int iDirection); // 调整参数的值

#endif /* GUI_GENERATOR_PANEL_H_ */
