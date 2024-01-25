/**
 * @file HC_SR04.c
 * @author Leo Liu
 * @brief get distance from the ultrasonic sensor
 * @version 1.0
 * @date 2023-09-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "HC_SR04.h"

void HC_SR04_Init(void);
void HC_SR04_Start(void);

HC_SR04_t HC_SR04;
HC_SR04_Func_t HC_SR04_Func = HC_SR04_Func_GroundInit;
#undef HC_SR04_Func_GroundInit

void HC_SR04_Init(void)
{
	HAL_TIM_Base_Start(&htim1);
	HAL_GPIO_WritePin(HC_SR04_Trig_GPIO_Port,HC_SR04_Trig_Pin,GPIO_PIN_RESET);
}

void HC_SR04_Start(void)
{
	HAL_GPIO_WritePin(HC_SR04_Trig_GPIO_Port,HC_SR04_Trig_Pin,GPIO_PIN_SET);
	__HAL_TIM_SET_COUNTER(&htim1,0);
	while(__HAL_TIM_GET_COUNTER(&htim1) < 20);
	HAL_GPIO_WritePin(HC_SR04_Trig_GPIO_Port,HC_SR04_Trig_Pin,GPIO_PIN_RESET);
		
	HC_SR04.Counter = HAL_GetTick();
	while(!(HAL_GPIO_ReadPin(HC_SR04_Echo_GPIO_Port, HC_SR04_Echo_Pin)));
	HC_SR04.Rising_Time = __HAL_TIM_GET_COUNTER(&htim1);
		
	HC_SR04.Counter = HAL_GetTick();
	while((HAL_GPIO_ReadPin(HC_SR04_Echo_GPIO_Port, HC_SR04_Echo_Pin)));
	HC_SR04.Falling_Time = __HAL_TIM_GET_COUNTER (&htim1);
		
	HC_SR04.Distance_Raw = (HC_SR04.Falling_Time - HC_SR04.Rising_Time)*0.034/2;
	HC_SR04.Distance_KF = Kalman_Filter_Func.First_Order_Kalman_Filter(&HC_SR04_KF,HC_SR04.Distance_Raw);
}
