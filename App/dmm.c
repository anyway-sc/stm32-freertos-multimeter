/*
 * dmm.c
 *
 *  Created on: Apr 5, 2026
 *      Author: liuyu
 */
#include "dmm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "lcd_task.h"

static QueueHandle_t xMailbox; // 邮箱句柄，静态全局变量，仅文件内部可见

static DMMRange_t DMM_GetRange(void);
static float prvCalcDMMValue(float fVadc, DMMRange_t eRange);


//
// @作用：初始化数字万用表模块
//
void vDMMInit(void)
{
	// 创建邮箱，队列项的尺寸为DMMDat_t
	xMailbox = xQueueCreate(1, sizeof(DMMData_t));

	extern TIM_HandleTypeDef htim2;
	HAL_TIM_Base_Start(&htim2);

	extern ADC_HandleTypeDef hadc1;
	HAL_ADCEx_Calibration_Start(&hadc1);
	HAL_ADCEx_InjectedStart_IT(&hadc1);
}

//
// @作用：获取数字万用表的测量结果
//
DMMData_t xDMMGetData(void)
{
	DMMData_t xResult;

	// 从邮箱里把数据读出来
	xQueuePeek(xMailbox, &xResult, portMAX_DELAY);

	return xResult;
}

//
// @作用：ADC的注入序列转换结束后会触发此回调函数
//
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if(hadc->Instance == ADC1) // 仅处理ADC1的结果
	{
		// 采集Vrefint，反推VDDA
		float fVdda = 4095.0f / HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_2) * 1.205f;
		// 万用表模块的运算放大电路的输出电压
		float fVdmm_adc = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_1) / 4095.0f * fVdda;

		// 稳压电源模块的输出电压
		float fVpwr = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_3) / 4095.0f * fVdda * 13.0f;

		DMMRange_t eDMMRange = DMM_GetRange(); // 获取当前挡位
		float fDMMValue = prvCalcDMMValue(fVdmm_adc, eDMMRange); // 计算结果

		// 计算触发电压的大小
		float triggerLevel = HAL_ADCEx_InjectedGetValue(hadc, ADC_INJECTED_RANK_4) / 4095.0f * 3.3f * 2.0f - 2.5f;

		DMMData_t xDmmData;

		xDmmData.eDMMRange = eDMMRange;
		xDmmData.fDMMValue = fDMMValue;
		xDmmData.fVpwr = fVpwr;
		xDmmData.fTriggerLevel = triggerLevel;

		xQueueOverwriteFromISR(xMailbox, &xDmmData, &xHigherPriorityTaskWoken);

		extern EventGroupHandle_t xEventGroupForLCD;

		xEventGroupSetBitsFromISR(xEventGroupForLCD,
				PWR_LABLE_REPAINT_BIT | DMM_LABLE_REPAINT_BIT, // 设置PWR和DMM的重绘事件比特
				&xHigherPriorityTaskWoken);

		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

//
// @作用：获取万用表的当前挡位
// @返回：挡位值。eDMMRange2V - 2V电压挡 eDMMRange10V - 10V电压挡 eDMMRange50V - 50V电压挡
//           eDMMRange1kOhm - 1k电阻挡 eDMMRange10kOhm - 10k电阻挡 eDMMRange100kOhm - 100k电阻挡
//           eDMMRange1MOhm - 1M电阻挡
//
static DMMRange_t DMM_GetRange(void)
{
	DMMRange_t range = eDMMRange2V;

	if(HAL_GPIO_ReadPin(DMM50V_GPIO_Port, DMM50V_Pin) == GPIO_PIN_SET)
	{
		range = eDMMRange50V; // 50V
	}
	else if(HAL_GPIO_ReadPin(DMM10V_GPIO_Port, DMM10V_Pin) == GPIO_PIN_SET)
	{
		range = eDMMRange10V; // 10V
	}
	else if(HAL_GPIO_ReadPin(DMM2V_GPIO_Port, DMM2V_Pin) == GPIO_PIN_SET)
	{
		range = eDMMRange2V; // 2V
	}
	else if(HAL_GPIO_ReadPin(DMM10kOhm_GPIO_Port, DMM10kOhm_Pin) == GPIO_PIN_SET)
	{
		range = eDMMRange10kOhm; // 10kOhm
	}
	else if(HAL_GPIO_ReadPin(DMM100kOhm_GPIO_Port, DMM100kOhm_Pin) == GPIO_PIN_SET)
	{
		range = eDMMRange100kOhm; // 100kOhm
	}
	else if(HAL_GPIO_ReadPin(DMM1MOhm_GPIO_Port, DMM1MOhm_Pin) == GPIO_PIN_SET)
	{
		range = eDMMRange1MOhm; // 1MOhm
	}
	else
	{
		range = eDMMRange1kOhm; // 1kOhm
	}

	return range;
}

//
// @作用：根据ADC的结果和万用表的挡位计算最终结果
//
static float prvCalcDMMValue(float fVadc, DMMRange_t eRange)
{
	float result;

	float fVin = fVadc * 2.0f - 2.0f;

	if(eRange == eDMMRange2V) // 电压档2V
	{
		result = fVin * 1.0f;
	}
	else if(eRange == eDMMRange10V) // 电压档10V
	{
		result = fVin * 5.0f;
	}
	else if(eRange == eDMMRange50V) // 电压档50V
	{
		result = fVin * 25.0f;
	}
	else // 以下为电阻档
	{
		float R0, R1, Rx;

		if(eRange == eDMMRange1kOhm) // 电阻档1k
		{
			R0 = 200.0f;
		}
		else if(eRange == eDMMRange10kOhm) // 电阻档10k
		{
			R0 = 2000.0f;
		}
		else if(eRange == eDMMRange100kOhm) // 电阻档100k
		{
			R0 = 2.0e4f;
		}
		else  // 电阻档1M
		{
			R0 = 2.0e5f;
		}

		R1 = 2.0e6f;
		Rx = fVin / (2.5 - fVin) * R0;

		result = R1 * Rx / (R1 - Rx);
	}

	return result;
}
