/**
 * @file Battery Level Detection.c
 * @author Leo Liu
 * @brief Detect Battery Level based on ADC
 * @version 1.0
 * @date 2024-04-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "Battery_Level_Detection.h"

float Get_Battery_Level(void)
{
	float temp = 0;
	for(uint8_t t = 0; t < SAMPLE_NUM; t++)
	{
		HAL_ADC_Start(BATTERY_ADC);
		HAL_ADC_PollForConversion(BATTERY_ADC,1000);
		temp += HAL_ADC_GetValue(BATTERY_ADC);
		osDelay(10);
	}

	return (temp/SAMPLE_NUM*3.45f/2048.0f-3.3f)/(0.9f)*100.0f;
}
