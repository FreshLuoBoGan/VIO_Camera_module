#ifndef __TIME_KEEPER
#define __TIME_KEEPER
#include "main.h"

void timeManagerInit(TIM_HandleTypeDef *tmr);
void time_keeper_interrupt_rutine(void);
uint64_t Micros(void);
void RPU_Tick(void);
void IMU_Tick(void);
void Camera_Tick(void);
uint64_t RPU_TimeStamp(void);
uint64_t IMU_TimeStamp(void);
uint64_t Camera_TimeStamp(void);

#endif