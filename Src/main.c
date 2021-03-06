/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"
#include <stdio.h>
#include <stdlib.h>

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);


/* Function processing DMA Rx data. Counts how many capital and small letters are in sentence.
 * Result is supposed to be stored in global variable of type "letter_count_" that is defined in "main.h"
 *
 * @param1 - received sign
 */
void proccesDmaData(uint8_t* sign,uint16_t len);
void calculateMemory();
void sendData(uint8_t* data,uint16_t len);


/* Space for your global variables. */

	// type your global variables here:
int small=0;
int capitals =0;

int s=0;
int l=0;

int startBit=0;
int counter =0;

int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* Configure the system clock */
  SystemClock_Config();
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();

  USART2_RegisterCallback(proccesDmaData);

  while (1)
  {

	  calculateMemory();
	  LL_mDelay(1000);
  }
  /* USER CODE END 3 */
}


void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0)
  {
  Error_Handler();  
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {
    
  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {
  
  }
  LL_Init1msTick(8000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(8000000);
}

/*
 * Implementation of function processing data received via USART.
 */
void proccesDmaData(uint8_t* sign,uint16_t len)
{
	uint8_t *tx_data;
	for(int j=0;j<len;j++){
		if (*(sign+j)=='#'){
			startBit=1;
		}
		if (startBit==1 && *(sign+j)>='a' && *(sign+j)<='z'){
			s++;
			counter++;
		}
		if (startBit==1 && *(sign+j)>='A' && *(sign+j)<='Z'){
			l++;
			counter++;
		}
		if (*(sign+j)=='$'){
			capitals =l;
			small=s;
			s=0;
			l=0;
			startBit=0;
			counter=0;
			int len_data = asprintf(&tx_data, "Small letters %d and capital letters %d\n\r",small,capitals);
			sendData(tx_data,len_data);
			free(tx_data);
		}
		if(startBit==1 && counter>=35){
			l =0;
			s =0;
			counter =0;
			startBit=0;
		}
	}
}

void sendData(uint8_t* data,uint16_t len)
{
	USART2_PutBuffer(data, len);
}


void calculateMemory()
{
	uint16_t size;
	uint16_t occupied;
	uint8_t *tx_data;
	uint16_t len_data;
	float percent = 0;

	size = DMA_USART2_BUFFER_SIZE;
	occupied = DMA_USART2_BUFFER_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_6);
	percent = 100.0/size*occupied;
	len_data = asprintf(&tx_data, "Buffer capacity: %3d bytes, occupied memory: %3d bytes, load [in ~%%]: %3d%%\n\r",size,occupied,(int)percent);
	sendData(tx_data,len_data);
	free(tx_data);
}

void Error_Handler(void)
{

}

#ifdef  USE_FULL_ASSERT

void assert_failed(char *file, uint32_t line)
{ 

}

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
