/*
 * panel_run_mode.h
 *
 *  Created on: Apr 28, 2026
 *      Author: liuyu
 */

#ifndef GUI_RUN_MODE_PANEL_H_
#define GUI_RUN_MODE_PANEL_H_

void vRepaintRunModePanel(void);
void vRunModePanelSelect(void); // 选中运行模式面板
void vRunModePanelDeselect(void); // 取消选中运行模式面板
void vRunModePanelAdjust(int iDirection); // 调整参数

#endif /* GUI_RUN_MODE_PANEL_H_ */
