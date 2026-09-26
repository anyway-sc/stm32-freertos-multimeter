/*
 * key.h
 *
 *  Created on: Mar 20, 2026
 *      Author: liuyu
 */

#ifndef DRIVERS_KEY_H_
#define DRIVERS_KEY_H_

#include "main.h"

typedef struct
{
	GPIO_PinState (*ReadPinCallback)(void);
	void (*ClickedCallback)(void);
} Key_InitTypeDef;

typedef struct
{
	Key_InitTypeDef Init;
	GPIO_PinState previous;
} Key_HandleTypeDef;

void Key_Init(Key_HandleTypeDef *Handle);
void Key_Scan(Key_HandleTypeDef *Handle);

#endif /* DRIVERS_KEY_H_ */
