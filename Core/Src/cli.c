//-----------------------------------------------------------------------------
//! \file       cli.c
//! \author     R. Weimar
//! \brief      Contains routines for the cli handling
//! \details    
//! \Attention  
//-----------------------------------------------------------------------------
#include "main.h"
#include "usart.h"
//-----------------------------------------------------------------------------
uint8_t rxBuffer = '\000';
uint8_t rxCounter;
uint8_t rxString[100]={0};
//-----------------------------------------------------------------------------
//! \brief      Initiates the commandline interface unit
//! \details    Starts the UART DMA
//! \param      None
void CLI_Init (void)
{
  HAL_UART_Receive_DMA(&huart1, rxString, 100); //Start Uart listening
}
//-----------------------------------------------------------------------------
//! \brief      Catches the UART receiving interrupt
//! \details    Is triggered after a line from Putty is received.
//! \param      UART_HandleTypeDef *huart  --> pointer to the uart receiving
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  //WRK_HandleUARTCommand();
}

