/*
 * lcd_task.h
 *
 *  Created on: Mar 26, 2026
 *      Author: liuyu
 */

#ifndef TASKS_LCD_TASK_H_
#define TASKS_LCD_TASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#define PWR_LABLE_REPAINT_BIT       (EventBits_t)(0x01 << 0) // bit0，重绘稳压电源标签
#define DMM_LABLE_REPAINT_BIT       (EventBits_t)(0x01 << 1) // bit1，重绘万用表标签
#define GENERATOR_PANEL_REPAINT_BIT (EventBits_t)(0x01 << 2) // bit2，重绘信号发生器面板
#define RUNMODE_PANEL_REPAINT_BIT   (EventBits_t)(0x01 << 3) // bit3，重绘运行模式面板
#define SCALE_PANEL_REPAINT_BIT     (EventBits_t)(0x01 << 4) // bit4，重绘缩放面板
#define SCALE_LABEL_REPAINT_BIT     (EventBits_t)(0x01 << 5) // bit5，重绘缩放标签
#define CURSOR_PANEL_REPAINT_BIT    (EventBits_t)(0x01 << 6) // bit6，重绘游标面板
#define WAVEFORM_PANEL_REPAINT_BIT  (EventBits_t)(0x01 << 7) // bit7，重绘波形面板
#define CURSOR_LABEL_REPAINT_BIT    (EventBits_t)(0x01 << 8) // bit8，重绘游标标签
#define ALL_REPAINT_BITS (PWR_LABLE_REPAINT_BIT | DMM_LABLE_REPAINT_BIT | GENERATOR_PANEL_REPAINT_BIT \
		                | RUNMODE_PANEL_REPAINT_BIT | SCALE_PANEL_REPAINT_BIT | SCALE_LABEL_REPAINT_BIT \
                        | CURSOR_PANEL_REPAINT_BIT | WAVEFORM_PANEL_REPAINT_BIT | CURSOR_LABEL_REPAINT_BIT\
						) // 所有用到的比特位

void vLCDTaskInit(void);
void vLCDTask(void *pvParameters);

#endif /* TASKS_LCD_TASK_H_ */
