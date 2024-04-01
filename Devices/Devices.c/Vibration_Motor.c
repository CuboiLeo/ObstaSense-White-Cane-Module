/**
 * @file Vibration_Motor.c
 * @author Leo Liu
 * @brief vibration motor
 * @version 1.0
 * @date 2024/03/27
 * 
 * @copyright Copyright (c) 2024
 * 
 */
 
 #include "Vibration_Motor.h"
 
 void Vibration_Motor_Init(void);
 void Vibration_Motor_Actuate(uint8_t Motor_ID, uint16_t Magnitude);
 
 Vibration_Motor_Func_t Vibration_Motor_Func = Vibration_Motor_Func_GroundInit;
 
 void Vibration_Motor_Init(void)
 {
	 HAL_TIM_PWM_Start(VIBRATION_MOTOR_TIM, TIM_CHANNEL_1);
	 HAL_TIM_PWM_Start(VIBRATION_MOTOR_TIM, TIM_CHANNEL_2);
	 HAL_TIM_PWM_Start(VIBRATION_MOTOR_TIM, TIM_CHANNEL_3);
 }
 
 void Vibration_Motor_Actuate(uint8_t Motor_ID, uint16_t Magnitude)
 {
	 switch (Motor_ID){
		 case 1: {
			 __HAL_TIM_SetCompare(VIBRATION_MOTOR_TIM, TIM_CHANNEL_1, Magnitude);
			 break;
		 }
		 case 2: {
			 __HAL_TIM_SetCompare(VIBRATION_MOTOR_TIM, TIM_CHANNEL_2, Magnitude);
			 break;
		 }
		 case 3: {
			 __HAL_TIM_SetCompare(VIBRATION_MOTOR_TIM, TIM_CHANNEL_3, Magnitude);
			 break;
		 }
	 }
 }
