/*
 * cursor_panel.h
 *
 *  Created on: May 1, 2026
 *      Author: liuyu
 */

#ifndef GUI_CURSOR_PANEL_H_
#define GUI_CURSOR_PANEL_H_

#include <stdint.h>

void RepaintCursorPanel(void); // 重绘游标面板
void SelectCursorPanel(void); // 选中游标面板
void DeselectCursorPanel(void); // 取消选中游标面板
void AdjustCursorPanel(int iDirection); // 调整参数
int8_t GetCursorX1Pct(void); // 获取游标X1的百分比
int8_t GetCursorX2Pct(void); // 获取游标X2的百分比
int8_t GetCursorY1Pct(void); // 获取游标Y1的百分比
int8_t GetCursorY2Pct(void); // 获取游标Y2的百分比

#endif /* GUI_CURSOR_PANEL_H_ */
