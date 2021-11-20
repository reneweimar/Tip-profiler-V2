//-----------------------------------------------------------------------------
//! \file       power.c
//! \author     R. Weimar
//! \brief      Contains routines for the charging and power functions
//! \details
//! \Attention
//-----------------------------------------------------------------------------
#include "main.h"
#include "power.h"
#include "work.h"
//-----------------------------------------------------------------------------
//! \brief      Returns charging or not
//! \details    Returns charging status
//! \param[out] uint8_t Charging
uint8_t PWR_Charging(void)
{
  static  uint8_t BattFullAndPluggedIn;
  uint8_t Charging;

  if (Charging()) //Charging input is on 
  {
    if (PluggedIn()) //Unit is plugged in
    {
      if ((BattFullAndPluggedIn == 1) && (BattPercentage==100))//Battery is full
      {
        Charging = 0; 
        BattFullAndPluggedIn = 1; //Prevent charging icon showing charging after full load is reached and unit is topping up battery
      }
      else
      {
        Charging = 1;
        BattFullAndPluggedIn = 0;
      }
    }
    else
    {
      BattFullAndPluggedIn = 0; //Not plugged in anymore, so reset flag
    }
  }
  else
  {
    BattFullAndPluggedIn = 0; //Not charging anymore, so reset flag
  }
  return Charging;
}
//-----------------------------------------------------------------------------
//! \brief      Initiates the power unit
//! \details    Initiates the gpio and functions of the power unit
//! \param      None
void PWR_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(Sensors_GPIO_Port, Sensors_Pin, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = Charging_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(Charging_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = StandBy_Pin;
  HAL_GPIO_Init(StandBy_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = Power_Pin;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Power_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = Sensors_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Sensors_GPIO_Port, &GPIO_InitStruct);

  PWR_SensorsOn();
}
//-----------------------------------------------------------------------------

