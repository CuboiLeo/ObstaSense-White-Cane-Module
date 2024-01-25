/**
 * @file HC_SR04.h
 * @author Leo Liu
 * @brief get distance from the ultrasonic sensor
 * @version 1.0
 * @date 2023-09-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __HC_SR04_H
#define __HC_SR04_H
 
#include <stdio.h>
#include <stdint.h>
#include <tim.h>
#include "cmsis_os.h"
#include "User_Defined_Math.h"
#include "Kalman_Filter.h"


#define HC_SR04_Func_GroundInit       		\
    {                                 		\
				&HC_SR04_Init,										\
						&HC_SR04_Start,								\
    }

typedef struct
{
	float Distance_Raw;
	float Distance_KF;
	uint32_t Counter;
	uint32_t Rising_Time;
	uint32_t Falling_Time;
}HC_SR04_t;

typedef struct
{
	void (*HC_SR04_Init)(void);
	void (*HC_SR04_Start)(void);
}HC_SR04_Func_t;

extern HC_SR04_t HC_SR04;
extern HC_SR04_Func_t HC_SR04_Func;

#endif
