/*
 * WaveformCapture.c
 *
 *  Created on: May 9, 2026
 *      Author: liuyu
 */

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "main.h"
#include "lcd_task.h"
#include "timers.h"

static uint16_t rawDataBuffer[1024]; // 用于缓存ADC的原始数据
static float waveform[1024]; // 采集到的波形数据
static TimerHandle_t xHoldOffTimer; // 软件定时器，用于禁止触发
static TimerHandle_t xForceTriggerTimer; // 软件定时器，用于强制触发

static void HoldOffTimerCallback(TimerHandle_t xTimer); // 禁止触发定时器的回调函数
static void ForceTriggerTimerCallback(TimerHandle_t xTimer); // 强制触发定时器的回调函数

// @作用：初始化波形捕捉器
void WaveformCapture_Init(void)
{
	extern TIM_HandleTypeDef htim3;

	// 启动定时器3，该定时器用于ADC1常规序列的外部触发
	HAL_TIM_Base_Start(&htim3);

	// 创建软件定时器，用于禁止触发
	xHoldOffTimer = xTimerCreate("Hold Off", pdMS_TO_TICKS(10), pdFALSE, NULL, HoldOffTimerCallback);

	// 创建软件定时器，用于强制触发
	xForceTriggerTimer = xTimerCreate("Force Trigger", pdMS_TO_TICKS(100), pdTRUE, NULL,ForceTriggerTimerCallback);

	// 启动该定时器
	xTimerStart(xForceTriggerTimer, portMAX_DELAY);
}

// @作用：获取波形数组
float *WaveformCapture_GetWaveform(void)
{
	return waveform;
}

// @作用：设置波形捕捉器的采样率
// @参数：Psc - 预分频器的值
// @参数：Arr - 自动重装寄存器的值
void WaveformCapture_SetSampleRate(uint16_t Psc, uint16_t Arr)
{
	extern TIM_HandleTypeDef htim3;

	HAL_TIM_Base_Stop(&htim3); // 零食关闭定时器

	__HAL_TIM_SET_PRESCALER(&htim3, Psc); // 设置PSC的值
	__HAL_TIM_SET_AUTORELOAD(&htim3, Arr); // 设置ARR的值

	HAL_TIM_Base_Start(&htim3); // 恢复定时器
}

// @作用：获取波形捕捉器的采样率
uint32_t WaveformCapture_GetSampleRate(void)
{
	extern TIM_HandleTypeDef htim3;

	uint16_t psc = htim3.Instance->PSC; // 获取PSC的值
	uint16_t arr = htim3.Instance->ARR; // 获取ARR的值

	// 采样率 = 72MHz / (psc+1) / (arr+1)
	return (uint32_t)(72.0e6f/(psc+1)/(arr+1));
}

// EXTI中断的回调函数
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	extern ADC_HandleTypeDef hadc1;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if(GPIO_Pin == Trigger_Pin)
	{
		// 屏蔽中断，以防再次触发
		HAL_NVIC_DisableIRQ(Trigger_EXTI_IRQn);

		// 以DMA方式启动ADC
		HAL_ADC_Start_DMA(&hadc1, (uint32_t *)rawDataBuffer, 1024);

		// 启动软件定时器，到期后再恢复中断
		xTimerStartFromISR(xHoldOffTimer, &xHigherPriorityTaskWoken);

		// 复位强制触发定时器
		xTimerResetFromISR(xForceTriggerTimer, &xHigherPriorityTaskWoken);

		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

// ADC常规序列数据采集完成的回调函数
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	if(hadc->Instance == ADC1)
	{
		// 1. 对采集到的原始数据进行处理
		for(int i=0; i<1024; i++)
		{
			waveform[i] = (rawDataBuffer[i] / 4095.0f * 3.3f) * 2.0f - 2.5f;
		}

		// 2. 通知LCD任务重绘波形面板
		extern EventGroupHandle_t xEventGroupForLCD;
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;

		xEventGroupSetBitsFromISR(xEventGroupForLCD, WAVEFORM_PANEL_REPAINT_BIT, &xHigherPriorityTaskWoken);

		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

// 禁止触发定时器的回调函数
static void HoldOffTimerCallback(TimerHandle_t xTimer)
{
	// 清除中断标志
	__HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_3);
	HAL_NVIC_ClearPendingIRQ(EXTI3_IRQn);

	// 再次开启中断，禁止触发结束
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);
}

// 强制触发定时器的回调函数
static void ForceTriggerTimerCallback(TimerHandle_t xTimer)
{
	__HAL_GPIO_EXTI_GENERATE_SWIT(GPIO_PIN_3);
}
