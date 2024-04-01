/**
 * @file Vibration_Motor.h 
 * @author Leo Liu
 * @brief vibration motor
 * @version 1.0
 * @date 2024/03/27
 * 
 * @copyright Copyright (c) 2024
 * 
 */
 
#ifndef __VIBRATION_MOTOR_H
#define __VIBRATION_MOTOR_H

#include <stdint.h>
#include "tim.h"

#define VIBRATION_MOTOR_TIM (&htim3)

#define Vibration_Motor_Func_GroundInit				\
{																							\
		&Vibration_Motor_Init,										\
				&Vibration_Motor_Actuate, 						\
}

typedef struct
{
	void (*Vibration_Motor_Init)(void);
	void (*Vibration_Motor_Actuate)(uint8_t Motor_ID, uint16_t Magnitude);
}Vibration_Motor_Func_t;

extern Vibration_Motor_Func_t Vibration_Motor_Func;

#endif
