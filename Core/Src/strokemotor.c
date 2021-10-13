//-----------------------------------------------------------------------------
//! \file       strokemotor.c
//! \author     R. Weimar
//! \brief      Contains routines for the stroke motor
//! \details
//! \Attention
//-----------------------------------------------------------------------------
#include "main.h"
//-----------------------------------------------------------------------------
stcDCMotor gSTR_Motor;
//-----------------------------------------------------------------------------
//! \brief      Calculates the motor speed
//! \details    Counts the pulses per 100 us and converts to RPM
//! \param      None
void STR_HandleEncoder (void)
{
  static uint8_t TickTime = 8; 
  if (TickTime++ < 99) return; //100ms 
  TickTime = 0;
  
  gSTR_Motor.GetSpeed = (uint16_t) ((float) gSTR_Motor.Encoder / (float) gSTR_Motor.PulsesPerRevolution * 600);
  gSTR_Motor.SetSpeed = (uint16_t) (720000 / ((float) gSTR_Motor.TimePerRev * (float) gSTR_Motor.PulsesPerRevolution));
  gSTR_Motor.Encoder = 0;
}
//-----------------------------------------------------------------------------
 //! \brief      Initiates the stroke motor unit
//! \details    Defines the GPIO and interrupts related to the stroke motor
//! \param      None
void STR_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  HAL_GPIO_WritePin(SleepStr_GPIO_Port, SleepStr_Pin, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = IntEncoder_Pin;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  HAL_GPIO_Init(IntEncoder_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = IntHome_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  HAL_GPIO_Init(IntHome_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = SleepStr_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(SleepStr_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = FaultStr_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(FaultStr_GPIO_Port, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  gSTR_Motor.PulsesPerRevolution = 12;

  HAL_TIM_Base_Start(&htim6);
}
//-----------------------------------------------------------------------------

