/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "main.h"
#include "stm32f4xx_it.hpp"
extern "C" {
#include "tx_api.h"
}

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{

   while (1)
  {
  }
 
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{

  while (1)
  {

  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{

  while (1)
  {

  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{

  while (1)
  {

  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{

  while (1)
  {

  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{

}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{

}

/**
  * @brief This function handles Pendable request for system service.
  */
// void PendSV_Handler(void)
// {
// }

// extern "C" void tx_timer_interrupt(void);
/**
  * @brief This function handles System tick timer.
  */
// void SysTick_Handler(void)
// {
//   // HAL_IncTick();
//   tx_timer_interrupt(); 

// }

extern ULONG _tx_timer_system_clock;

uint32_t HAL_GetTick(void)
{
    return (uint32_t)_tx_timer_system_clock;
}



/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/
#include "Stm32Uart.hpp"

extern char rxBuffer[1024];
auto g_uart2 = Stm32Uart::fromInstance(USART2);

void USART2_IRQHandler(void) {

  if (g_uart2){
    g_uart2->handleUartIrq();
  } 
 
}

void DMA1_Stream5_IRQHandler(void)
{
  if(auto s = Stm32Uart::fromDma(DMA1_Stream5)){
   s->handleDmaRxIrq();
   }
}


void DMA1_Stream6_IRQHandler(void)
{
  if(auto s = Stm32Uart::fromDma(DMA1_Stream6)){
   s->handleDmaTxIrq();
   }  
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        g_uart2->handleRxCallback();
        g_uart2->handleRxBlockCallback(reinterpret_cast<uint8_t*>(rxBuffer), 1024);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        g_uart2->handleTxCallback();
    }
}

// Optional error handler
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) {
        g_uart2->handleUartErrorCallback();
    }
}
