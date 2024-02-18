/**
 * @file NRF24L01.c
 * @author Leo Liu
 * @brief wireless communication module
 * @version 1.0
 * @date 2024-02-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */
 
#include "NRF24L01.h"

void NRF24L01_Write_Reg(uint8_t Reg, uint8_t Data);
void NRF24L01_Write_Multi_Reg(uint8_t Reg, uint8_t *Data, int Size);
uint8_t NRF24L01_Read_Reg(uint8_t Reg);
void NRF24L01_Read_Multi_Reg(uint8_t Reg, uint8_t *Data, int Size);
void NRF24L01_Send_Cmd(uint8_t Cmd);
void NRF24L01_Init(void);
void NRF24L01_Tx_Mode(uint8_t *Address, uint16_t Channel);
uint8_t NRF24L01_Transmit(uint8_t *data);



NRF24L01_Func_t NRF24L01_Func = NRF24L01_Func_GroundInit;

uint8_t NRF24L01_Transmit(uint8_t *data)
{
	HAL_GPIO_WritePin(SPI_CNS_GPIO_Port,SPI_CNS_Pin,GPIO_PIN_SET);
	uint8_t cmd = 0;
	cmd = W_TX_PAYLOAD;
	HAL_SPI_Transmit(&hspi1,&cmd,1,100);
	HAL_SPI_Transmit(&hspi1,data,32,1000);
	HAL_GPIO_WritePin(SPI_CNS_GPIO_Port,SPI_CNS_Pin,GPIO_PIN_RESET);
	DELAY_US(100);
	
	uint8_t fifo_status = NRF24L01_Read_Reg(FIFO_STATUS);
	if((fifo_status&(1<<4))&&(!(fifo_status&(1<<3))))
	{
		cmd = FLUSH_TX;
		NRF24L01_Send_Cmd(cmd);
		return 1;
	}
	return 0;
}

void NRF24L01_Tx_Mode(uint8_t *Address, uint16_t Channel)
{
	HAL_GPIO_WritePin(SPI_CE_GPIO_Port,SPI_CE_Pin,GPIO_PIN_RESET);
	
	NRF24L01_Write_Reg(RF_CH,Channel);
	NRF24L01_Write_Multi_Reg(TX_ADDR,Address,5);
	uint8_t config = NRF24L01_Read_Reg(CONFIG);
	config = config | (1<<1);
	NRF24L01_Write_Reg(CONFIG,config);
	
	HAL_GPIO_WritePin(SPI_CE_GPIO_Port,SPI_CE_Pin,GPIO_PIN_SET);
}

void NRF24L01_Init(void)
{
	HAL_GPIO_WritePin(SPI_CE_GPIO_Port,SPI_CE_Pin,GPIO_PIN_RESET);
	
	NRF24L01_Write_Reg(CONFIG,0);
	NRF24L01_Write_Reg(EN_AA,0);
	NRF24L01_Write_Reg(EN_RXADDR,0);
	NRF24L01_Write_Reg(SETUP_AW,0x03);
	NRF24L01_Write_Reg(SETUP_RETR,0);
	NRF24L01_Write_Reg(RF_CH,0);
	NRF24L01_Write_Reg(RF_SETUP,0x0E);
	
	HAL_GPIO_WritePin(SPI_CE_GPIO_Port,SPI_CE_Pin,GPIO_PIN_SET);
}

void NRF24L01_Write_Reg(uint8_t Reg, uint8_t Data)
{
	uint8_t buf[2];
	buf[0] = Reg|1<<5;
	buf[1] = Data;
	
	HAL_GPIO_WritePin(SPI_CNS_GPIO_Port,SPI_CNS_Pin,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,buf,2,1000);
	HAL_GPIO_WritePin(SPI_CNS_GPIO_Port,SPI_CNS_Pin,GPIO_PIN_SET);
}

void NRF24L01_Write_Multi_Reg(uint8_t Reg, uint8_t *Data, int Size)
{
	uint8_t buf[2];
	buf[0] = Reg|1<<5;
	
	HAL_GPIO_WritePin(SPI_CNS_GPIO_Port,SPI_CNS_Pin,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,buf,1,100);
	HAL_SPI_Transmit(&hspi1,Data,Size,1000);
	HAL_GPIO_WritePin(SPI_CNS_GPIO_Port,SPI_CNS_Pin,GPIO_PIN_SET);
}

uint8_t NRF24L01_Read_Reg(uint8_t Reg)
{
	uint8_t data = 0;
	
	HAL_GPIO_WritePin(SPI_CNS_GPIO_Port,SPI_CNS_Pin,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&Reg,1,100);
	HAL_SPI_Receive(&hspi1,&data,1,100);
	HAL_GPIO_WritePin(SPI_CNS_GPIO_Port,SPI_CNS_Pin,GPIO_PIN_SET);
	
	return data;
}

void NRF24L01_Read_Multi_Reg(uint8_t Reg, uint8_t *Data, int Size)
{
	HAL_GPIO_WritePin(SPI_CNS_GPIO_Port,SPI_CNS_Pin,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&Reg,1,100);
	HAL_SPI_Receive(&hspi1,Data,Size,100);
	HAL_GPIO_WritePin(SPI_CNS_GPIO_Port,SPI_CNS_Pin,GPIO_PIN_SET);
}

void NRF24L01_Send_Cmd(uint8_t Cmd)
{
	HAL_GPIO_WritePin(SPI_CNS_GPIO_Port,SPI_CNS_Pin,GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1,&Cmd,1,100);
	HAL_GPIO_WritePin(SPI_CNS_GPIO_Port,SPI_CNS_Pin,GPIO_PIN_SET);
}
