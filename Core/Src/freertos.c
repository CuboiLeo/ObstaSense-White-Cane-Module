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
#include "Buzzer.h"
#include "Vibration_Motor.h"
#include "nrf24l01p.h"
#include "Battery_Level_Detection.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define RX
#define TX
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
#ifdef TX
uint8_t tx_data[NRF24L01P_PAYLOAD_LENGTH] = {0};
#endif
#ifdef RX
uint8_t rx_data[NRF24L01P_PAYLOAD_LENGTH];
#endif
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
float Battery_Percent;
/* USER CODE END Variables */
osThreadId Task_InitHandle;
osThreadId Task_SensingHandle;
osThreadId Task_SerialHandle;
osThreadId Task_FeedbackHandle;
osThreadId Task_BatteryHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
#ifdef TX
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == NRF24L01P_IRQ_PIN_NUMBER)
		nrf24l01p_tx_irq(); // clear interrupt flag
}
#endif
#ifdef RX
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == NRF24L01P_IRQ_PIN_NUMBER)
		nrf24l01p_rx_receive(rx_data); // read data when data ready flag is set
}
#endif
/* USER CODE END FunctionPrototypes */

void Initialization(void const * argument);
void Sensing(void const * argument);
void Serial_Send(void const * argument);
void User_Feedback(void const * argument);
void Battery_Level(void const * argument);

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
  osThreadDef(Task_Sensing, Sensing, osPriorityAboveNormal, 0, 128);
  Task_SensingHandle = osThreadCreate(osThread(Task_Sensing), NULL);

  /* definition and creation of Task_Serial */
  osThreadDef(Task_Serial, Serial_Send, osPriorityNormal, 0, 128);
  Task_SerialHandle = osThreadCreate(osThread(Task_Serial), NULL);

  /* definition and creation of Task_Feedback */
  osThreadDef(Task_Feedback, User_Feedback, osPriorityNormal, 0, 128);
  Task_FeedbackHandle = osThreadCreate(osThread(Task_Feedback), NULL);

  /* definition and creation of Task_Battery */
  osThreadDef(Task_Battery, Battery_Level, osPriorityNormal, 0, 128);
  Task_BatteryHandle = osThreadCreate(osThread(Task_Battery), NULL);

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
		Vibration_Motor_Func.Vibration_Motor_Init();
		Buzzer_Func.Buzzer_Init();
		#ifdef TX
		nrf24l01p_tx_init(2500, _1Mbps);
		#endif
		
		#ifdef RX
		nrf24l01p_rx_init(2500, _1Mbps);
		#endif
		osDelay(1000);
		Buzzer_Func.Buzzer_Power_On();
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
    printf("/*%f,%f,%f*/\n",HC_SR04.Distance_Raw,HC_SR04.Distance_KF,Battery_Percent);
		vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
  }
  /* USER CODE END Serial_Send */
}

/* USER CODE BEGIN Header_User_Feedback */
/**
* @brief Function implementing the Task_Feedback thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_User_Feedback */
void User_Feedback(void const * argument)
{
  /* USER CODE BEGIN User_Feedback */
	portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  const TickType_t TimeIncrement = pdMS_TO_TICKS(25);
  /* Infinite loop */
  for(;;)
  {
		#ifdef TX
		if(HC_SR04.Distance_KF < 85.0f)
		{
			tx_data[0] = 3;
		}
		else if(HC_SR04.Distance_KF < 125.0f)
		{
			tx_data[0] = 2;
		}
		else if(HC_SR04.Distance_KF < 165.0f)
		{
			tx_data[0] = 1;
		}
		else
		{
			tx_data[0] = 0;
		}
		nrf24l01p_tx_transmit(tx_data);
		#endif
		
		#ifdef RX
		switch(rx_data[0])
		{
			case 0:
				HAL_TIM_PWM_Stop(VIBRATION_MOTOR_TIM, TIM_CHANNEL_1);
				break;
			case 1:
				HAL_TIM_PWM_Start(VIBRATION_MOTOR_TIM, TIM_CHANNEL_1);
				Vibration_Motor_Func.Vibration_Motor_Actuate(1,500);
				osDelay(500);
				Vibration_Motor_Func.Vibration_Motor_Actuate(1,0);
				osDelay(200);
				Vibration_Motor_Func.Vibration_Motor_Actuate(1,500);
				osDelay(500);
				Vibration_Motor_Func.Vibration_Motor_Actuate(1,0);
				osDelay(1000);
				HAL_TIM_PWM_Stop(VIBRATION_MOTOR_TIM, TIM_CHANNEL_1);
				break;
			case 2:
				HAL_TIM_PWM_Start(VIBRATION_MOTOR_TIM, TIM_CHANNEL_1);
				Vibration_Motor_Func.Vibration_Motor_Actuate(1,750);
				osDelay(500);
				Vibration_Motor_Func.Vibration_Motor_Actuate(1,0);
				osDelay(200);
				Vibration_Motor_Func.Vibration_Motor_Actuate(1,750);
				osDelay(500);
				Vibration_Motor_Func.Vibration_Motor_Actuate(1,0);
				osDelay(1000);
				HAL_TIM_PWM_Stop(VIBRATION_MOTOR_TIM, TIM_CHANNEL_1);
				break;
			case 3:
				HAL_TIM_PWM_Start(VIBRATION_MOTOR_TIM, TIM_CHANNEL_1);
				Vibration_Motor_Func.Vibration_Motor_Actuate(1,1000);
				osDelay(500);
				Vibration_Motor_Func.Vibration_Motor_Actuate(1,0);
				osDelay(200);
				Vibration_Motor_Func.Vibration_Motor_Actuate(1,1000);
				osDelay(500);
				Vibration_Motor_Func.Vibration_Motor_Actuate(1,0);
				osDelay(1000);
				HAL_TIM_PWM_Stop(VIBRATION_MOTOR_TIM, TIM_CHANNEL_1);
				break;
		}
		#endif
		
    vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
  }
  /* USER CODE END User_Feedback */
}

/* USER CODE BEGIN Header_Battery_Level */
/**
* @brief Function implementing the Task_Battery thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Battery_Level */
void Battery_Level(void const * argument)
{
  /* USER CODE BEGIN Battery_Level */
	portTickType xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();
  const TickType_t TimeIncrement = pdMS_TO_TICKS(5000);
  /* Infinite loop */
  for(;;)
  {
    Battery_Percent = Get_Battery_Level();
		if(Battery_Percent < 30.0f)
		{
			Buzzer_Func.Buzzer_Warning();
		}
		vTaskDelayUntil(&xLastWakeTime, TimeIncrement);
  }
  /* USER CODE END Battery_Level */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

