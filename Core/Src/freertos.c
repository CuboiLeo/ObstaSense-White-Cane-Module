/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "HC_SR04.h"
#include "Serial.h"
#include "NRF24L01.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId Task_InitHandle;
osThreadId Task_SensingHandle;
osThreadId Task_SerialHandle;
osThreadId Task_RemoteHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void Initialization(void const * argument);
void Sensing(void const * argument);
void Serial_Send(void const * argument);
void Wireless_Transmit(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of Task_Init */
  osThreadDef(Task_Init, Initialization, osPriorityHigh, 0, 128);
  Task_InitHandle = osThreadCreate(osThread(Task_Init), NULL);

  /* definition and creation of Task_Sensing */
  osThreadDef(Task_Sensing, Sensing, osPriorityNormal, 0, 128);
  Task_SensingHandle = osThreadCreate(osThread(Task_Sensing), NULL);

  /* definition and creation of Task_Serial */
  osThreadDef(Task_Serial, Serial_Send, osPriorityNormal, 0, 128);
  Task_SerialHandle = osThreadCreate(osThread(Task_Serial), NULL);

  /* definition and creation of Task_Remote */
  osThreadDef(Task_Remote, Wireless_Transmit, osPriorityNormal, 0, 128);
  Task_RemoteHandle = osThreadCreate(osThread(Task_Remote), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Initialization */
/**
  * @brief  Function implementing the Task_Init thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Initialization */
void Initialization(void const * argument)
{
  /* USER CODE BEGIN Initialization */
	
  /* Infinite loop */
  for(;;)
  {
		HC_SR04_Func.HC_SR04_Init();
		uint8_t Tx_Address[] = {0xEE,0xDD,0xCC,0xBB,0xAA};
		NRF24L01_Func.NRF24L01_Init();
		NRF24L01_Func.NRF24L01_Tx_Mode(Tx_Address,10);
		vTaskDelete(NULL);
  }
  /* USER CODE END Initialization */
}

/* USER CODE BEGIN Header_Sensing */
/**
* @brief Function implementing the Task_Sensing thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Sensing */
void Sensing(void const * argument)
{
  /* USER CODE BEGIN Sensing */
	portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  const TickType_t TimeIncrement = pdMS_TO_TICKS(25);
  /* Infinite loop */
  for(;;)
  {
		HC_SR04_Func.HC_SR04_Start();
    vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
  }
  /* USER CODE END Sensing */
}

/* USER CODE BEGIN Header_Serial_Send */
/**
* @brief Function implementing the Task_Serial thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Serial_Send */
void Serial_Send(void const * argument)
{
  /* USER CODE BEGIN Serial_Send */
	portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  const TickType_t TimeIncrement = pdMS_TO_TICKS(25);
  /* Infinite loop */
  for(;;)
  {
    printf("/*%f,%f*/\n",HC_SR04.Distance_Raw,HC_SR04.Distance_KF);
		vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
  }
  /* USER CODE END Serial_Send */
}

/* USER CODE BEGIN Header_Wireless_Transmit */
/**
* @brief Function implementing the Task_Remote thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Wireless_Transmit */
void Wireless_Transmit(void const * argument)
{
  /* USER CODE BEGIN Wireless_Transmit */
  portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  const TickType_t TimeIncrement = pdMS_TO_TICKS(100);
  /* Infinite loop */
  for(;;)
  {
		uint8_t Tx_Data[] = {1};
    if(NRF24L01_Func.NRF24L01_Transmit(Tx_Data)==1)
			;
		vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
  }
  /* USER CODE END Wireless_Transmit */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

