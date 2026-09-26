/*
 * waveform_generator.c
 *
 *  Created on: May 11, 2026
 *      Author: liuyu
 */

#include "stdint.h"
#include "math.h"
#include "main.h"

#define PI 3.14159265358979f // 圆周率

static uint16_t waveform[1000]; // 用于存储信号发生器单个周期的波形
static uint16_t waveformLength; // 波形的实际长度
static uint8_t shape = 0; // 波形的形状，0 - OFF，1 - 正弦波， 2 - 三角波， 3 - 方波
static uint32_t frequency = 1.0e3; // 波形的频率，单位Hz

static void AutoSampleRate(void);
static void PrepareWaveformData(void);
static void RestartDAC(void);

// @作用：初始化波形发生器
void WaveformGenerator_Init(void)
{
	RestartDAC(); // 重新加载DAC波形
}

// @作用：设置波形形状
// @参数：Shape - 波形形状：0 - OFF，1 - 正弦波， 2 - 三角波， 3 - 方波
void WaveformGenerator_SetShape(uint8_t Shape)
{
	shape = Shape;
	RestartDAC(); // 重新加载DAC波形
}

// @作用：设置波形频率
// @参数：Frequency - 频率，单位Hz
void WaveformGenerator_SetFrequency(uint32_t Frequency)
{
	frequency = Frequency;
	RestartDAC(); // 重新加载DAC波形
}


// @作用：根据波形频率自动调整采样率
static void AutoSampleRate(void)
{
	uint32_t sampleRate;

	sampleRate = 1000 * frequency; // 理想的采样率是波形频率*点数

	waveformLength = 1000; // 默认波形长度1000点

	if(sampleRate > 500e3) // 如果采样率大于500kHz
	{
		sampleRate = 500e3;

		// 重新计算波形长度
		waveformLength = round((float)sampleRate / frequency);
	}

	// 根据采样率计算TIM的PSC和ARR
	uint16_t psc = 0;
	uint16_t arr = 72e6 / sampleRate;

	// 将PSC和ARR的值设置到硬件当中
	extern TIM_HandleTypeDef htim5;
	__HAL_TIM_SET_PRESCALER(&htim5, psc); // 设置PSC的值
	__HAL_TIM_SET_AUTORELOAD(&htim5, arr); // 设置ARR的值
}

// @作用：准备波形数据
static void PrepareWaveformData(void)
{
	if(shape == 0) // OFF挡，输出0V电压
	{
		for(int i=0; i<waveformLength; i++)
		{
			waveform[i] = 1241;
		}
	}
	else if(shape == 1) // Sin挡，输出正弦波
	{
		for(int i=0; i<waveformLength; i++)
		{
			// 计算DAC的输出电压
			float volt = 1.0f + sinf(((float)i/waveformLength)*2*PI);
			// 将电压转换为0~4095之间的数字
			waveform[i] = roundf(volt / 3.3f * 4095);
		}
	}
	else if(shape == 2) // Triangle挡，输出三角波
	{
		for(int i=0; i<waveformLength; i++)
		{
			// 计算DAC的输出电压
			float volt = 1.0f + ((float)i/waveformLength) * (2.0 - 1.0f);
			// 将电压转换为0~4095之间的数字
			waveform[i] = roundf(volt / 3.3f * 4095);
		}
	}
	else if(shape == 3) // Square挡，输出方波
	{
		for(int i=0; i<waveformLength; i++)
		{
			if(i<waveformLength / 2)
			{
				waveform[i] = 2482;
			}
			else
			{
				waveform[i] = 1241;
			}
		}
	}
}

// @作用：将波形数据发送到DAC，并重启DAC
static void RestartDAC(void)
{
	extern DAC_HandleTypeDef hdac;
	extern TIM_HandleTypeDef htim5;

	// 先停止定时器和DAC
	HAL_TIM_Base_Stop(&htim5);
	HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);

	AutoSampleRate(); // 调整采样率
	PrepareWaveformData(); // 准备波形数据

	// 开启DAC
	HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (const uint32_t *)waveform, waveformLength, DAC_ALIGN_12B_R);
	// 开启TIM5
	HAL_TIM_Base_Start(&htim5);
}
