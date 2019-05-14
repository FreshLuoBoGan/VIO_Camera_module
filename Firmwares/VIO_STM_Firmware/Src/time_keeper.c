#include "main.h"

uint64_t tick=0,rpu_tick=0,camera_tick=0,imu_tick=0;
uint32_t tick_index=0;
TIM_HandleTypeDef *timer;
void  timeManagerInit(TIM_HandleTypeDef *tmr)
{
	timer=tmr;
	HAL_TIM_Base_Start_IT(timer);
}
void time_keeper_interrupt_rutine(void)
{
	tick_index++;
}
uint64_t Micros(void)
{
	return (uint64_t)(tick_index*65000)+(uint64_t)(timer->Instance->CNT);
}

void RPU_Tick(void)
{
	rpu_tick=Micros();
}

void IMU_Tick(void)
{
	imu_tick=Micros();
}

void Camera_Tick(void)
{
	camera_tick=Micros();
}
uint64_t RPU_TimeStamp(void)
{
	return rpu_tick;
}
uint64_t IMU_TimeStamp(void)
{
	return imu_tick;
}
uint64_t Camera_TimeStamp(void)
{
	return camera_tick;
}
