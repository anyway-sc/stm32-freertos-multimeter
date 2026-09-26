/*
 * buzzer_task.h
 *
 *  Created on: Mar 21, 2026
 *      Author: liuyu
 */

#ifndef TASKS_BUZZER_TASK_H_
#define TASKS_BUZZER_TASK_H_

void Buzzer_Init(void); // 蜂鸣器初始化
void vBuzzerTask(void *pvParameters); // 任务函数
void Buzzer_Beep(void); // 通过给信号的方式让蜂鸣器鸣叫

#endif /* TASKS_BUZZER_TASK_H_ */
