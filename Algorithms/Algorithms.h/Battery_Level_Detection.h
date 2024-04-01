/**
 * @file Battery Level Detection.h
 * @author Leo Liu
 * @brief Detect Battery Level based on ADC
 * @version 1.0
 * @date 2024-04-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef __BATTERY_LEVEL_DETECTION
#define __BATTERY_LEVEL_DETECTION

#include "adc.h"
#include "cmsis_os.h"
#include <stdint.h>

#define BATTERY_ADC (&hadc1)
#define SAMPLE_NUM (10)

float Get_Battery_Level(void);

#endif
