/*
 * WaveformCapture.h
 *
 *  Created on: May 9, 2026
 *      Author: liuyu
 */

#ifndef WAVEFORM_CAPTURE_H_
#define WAVEFORM_CAPTURE_H_

void WaveformCapture_Init(void); // 初始化波形捕捉器
void vWaveformTask(void *argument); // 波形处理任务：完成数据转换并用互斥锁保护波形缓冲区
float *WaveformCapture_GetWaveform(void); // 获取波形数组
void WaveformCapture_SetSampleRate(uint16_t Psc, uint16_t Arr); // 设置波形捕捉器的采样率
uint32_t WaveformCapture_GetSampleRate(void); // 获取波形捕捉器的采样率

#endif /* WAVEFORM_CAPTURE_H_ */
