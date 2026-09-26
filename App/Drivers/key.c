/*
 * key.c
 *
 *  Created on: Mar 20, 2026
 *      Author: liuyu
 */
#include "key.h"

void Key_Init(Key_HandleTypeDef *Handle)
{
	Handle->previous = GPIO_PIN_SET;
}

void Key_Scan(Key_HandleTypeDef *Handle)
{
	GPIO_PinState current;

	current = Handle->Init.ReadPinCallback();

	if(current == GPIO_PIN_SET && Handle->previous == GPIO_PIN_RESET)
	{
		Handle->Init.ClickedCallback();
	}

	Handle->previous = current;
}
