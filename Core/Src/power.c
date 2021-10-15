//-----------------------------------------------------------------------------
//! \file       power.c
//! \author     R. Weimar
//! \brief      Contains routines for the charging and power functions
//! \details
//! \Attention
//-----------------------------------------------------------------------------
#include "main.h"
#include "power.h"
//-----------------------------------------------------------------------------
//! \brief      Initiates the power unit
//! \details    Initiates the gpio and functions of the power unit
//! \param      None
void PWR_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(StandBy_GPIO_Port, StandBy_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = Charging_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(Charging_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = StandBy_Pin;
  HAL_GPIO_Init(StandBy_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = Power_Pin;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Power_GPIO_Port, &GPIO_InitStruct);
}
//-----------------------------------------------------------------------------

