/*
 * waveform_generator.h
 *
 *  Created on: May 11, 2026
 *      Author: liuyu
 */

#ifndef WAVEFORM_GENERATOR_H_
#define WAVEFORM_GENERATOR_H_

void WaveformGenerator_Init(void); // 初始化波形发生器
void WaveformGenerator_SetShape(uint8_t Shape); // 设置波形形状
void WaveformGenerator_SetFrequency(uint32_t Frequency); // 设置波形频率

#endif /* WAVEFORM_GENERATOR_H_ */
