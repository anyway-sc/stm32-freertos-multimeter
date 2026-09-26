/*
 * lcd_task.c
 *
 *  Created on: Mar 26, 2026
 *      Author: liuyu
 */

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "lcd.h"
#include "lcd_task.h"
#include "event_groups.h"

#include "dmm_label.h"
#include "pwr_label.h"
#include "generator_panel.h"
#include "scale_panel.h"
#include "run_mode_panel.h"
#include "scale_label.h"
#include "cursor_panel.h"
#include "waveform_panel.h"
#include "cursor_label.h"

static SemaphoreHandle_t xSemaphoreForDMA;
EventGroupHandle_t xEventGroupForLCD;


static void BackLightControl(uint8_t On);
static void Reset(void);
static void SendCommand(uint8_t Cmd);
static void SendData(uint8_t *pData, uint16_t Size);

void vLCDTaskInit(void)
{
	xEventGroupForLCD = xEventGroupCreate();
}

void vLCDTask(void *pvParameters)
{
	xSemaphoreForDMA = xSemaphoreCreateBinary();

	BackLightControl(1);

	LCD_InitTypeDef LCD_InitStruct = {0};

	LCD_InitStruct.reset_callback = Reset;
	LCD_InitStruct.send_command_callback = SendCommand;
	LCD_InitStruct.send_data_callback = SendData;

	LCD_Init(&LCD_InitStruct);

	LCD_SetBrush(0xffffff); // 白色背景
	LCD_Clear(); // 清屏

	// 开机后的第一次，所有部件都需要重绘
	xEventGroupSetBits(xEventGroupForLCD, ALL_REPAINT_BITS);

	for(;;)
	{
		EventBits_t xBitRet;

		// xClearOnExit = pdTRUE，退出时擦除
		// xWaitForAllBits = pdFALSE，等待任意比特位
		xBitRet = xEventGroupWaitBits(xEventGroupForLCD,ALL_REPAINT_BITS, pdTRUE, pdFALSE,portMAX_DELAY);

		if(xBitRet & PWR_LABLE_REPAINT_BIT)
		{
			PwrLabel_Repaint(); // 重绘稳压电源标签
		}
		if(xBitRet & DMM_LABLE_REPAINT_BIT)
		{
			DMMLabel_Repaint(); // 重绘数字万用表标签
		}
		if(xBitRet & RUNMODE_PANEL_REPAINT_BIT)
		{
			vRepaintRunModePanel(); // 重绘运行模式面板
		}
		if(xBitRet & SCALE_PANEL_REPAINT_BIT)
		{
			vRepaintScalePanel(); // 重绘缩放面板
		}
		if(xBitRet & GENERATOR_PANEL_REPAINT_BIT)
		{
			vRepaintGeneratorPanel(); // 重绘信号发生器面板
		}
		if(xBitRet & SCALE_LABEL_REPAINT_BIT)
		{
			RepaintScaleLabel(); // 重绘缩放标签
		}
		if(xBitRet & CURSOR_PANEL_REPAINT_BIT)
		{
			RepaintCursorPanel(); // 重绘游标面板
		}
		if(xBitRet & WAVEFORM_PANEL_REPAINT_BIT)
		{
			RepaintWaveformPanel(); // 重绘波形面板
		}
		if(xBitRet & CURSOR_LABEL_REPAINT_BIT)
		{
			RepaintCursorLabel(); // 重绘游标标签
		}
	}
}

//
// @作用：控制屏幕背光的点亮和熄灭
// @参数：On - 0熄灭，非0点亮
//
static void BackLightControl(uint8_t On)
{
	if(On != 0)
	{
		HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_RESET);
	}
}

//
// @作用：使用RST引脚对屏幕进行复位
//
static void Reset(void)
{
	// 将RST引脚拉低，施加复位信号
	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
	// 最小延迟时间1ms > 10us
	vTaskDelay(pdMS_TO_TICKS(1));
	// 将RST引脚拉高，释放复位信号
	HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
	// 延迟125ms等待复位完成
	vTaskDelay(pdMS_TO_TICKS(125));
}

//
// @作用：通过4线串口向LCD发送命令
// @参数：Cmd - 要发送的命令
//
static void SendCommand(uint8_t Cmd)
{
	extern SPI_HandleTypeDef hspi1;

	// 切换到命令模式
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);
	// 选中从机
	HAL_GPIO_WritePin(LCD_NSS_GPIO_Port, LCD_NSS_Pin, GPIO_PIN_RESET);
	// 使用SPI发送命令
	HAL_SPI_Transmit(&hspi1, &Cmd, 1, HAL_MAX_DELAY);
	// 取消选中
	HAL_GPIO_WritePin(LCD_NSS_GPIO_Port, LCD_NSS_Pin, GPIO_PIN_SET);
}

//
// @作用：通过4线串口向LCD发送数据
//
static void SendData(uint8_t *pData, uint16_t Size)
{
	extern SPI_HandleTypeDef hspi1;

	// 切换到数据模式
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
	// 选中从机
	HAL_GPIO_WritePin(LCD_NSS_GPIO_Port, LCD_NSS_Pin, GPIO_PIN_RESET);
	// 使用SPI发送数据
	// HAL_SPI_Transmit(&hspi1, pData, Size, HAL_MAX_DELAY);
	HAL_SPI_Transmit_DMA(&hspi1, pData, Size);

	xSemaphoreTake(xSemaphoreForDMA, portMAX_DELAY);

	// 取消选中
	HAL_GPIO_WritePin(LCD_NSS_GPIO_Port, LCD_NSS_Pin, GPIO_PIN_SET);
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	if(hspi->Instance == SPI1)
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;

		xSemaphoreGiveFromISR(xSemaphoreForDMA, &xHigherPriorityTaskWoken);

		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}
