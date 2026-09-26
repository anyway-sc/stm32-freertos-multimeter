/*
 * scale_panel.h
 *
 *  Created on: Apr 29, 2026
 *      Author: liuyu
 */

#ifndef GUI_SCALE_PANEL_H_
#define GUI_SCALE_PANEL_H_

void vRepaintScalePanel(void); // 重绘缩放面板
void vScalePanelSelect(void); // 选中缩放面板
void vScalePanelDeselect(void); // 取消选中缩放面板
void vScalePanelAdjust(int iDirection); // 调整参数
float fGetXScale(void); // 获取X轴向的缩放比
float fGetYScale(void); // 获取Y轴向的缩放比

#endif /* GUI_SCALE_PANEL_H_ */
