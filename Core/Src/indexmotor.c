//-----------------------------------------------------------------------------
//! \file       indexmotor.c
//! \author     R. Weimar
//! \brief      Contains routines for the index motor
//! \details
//! \Attention
//-----------------------------------------------------------------------------
#include "main.h"
//-----------------------------------------------------------------------------
//! \Global step counter
stcStepperMotor gIndexMotor;
//-----------------------------------------------------------------------------
//! \brief      Initiates the index motor unit
//! \details    Defines the GPIO and interrupts related to the index motor
//! \param      None
void IDX_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  HAL_GPIO_WritePin(SleepIdx_GPIO_Port, SleepIdx_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = IntIndex_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IntIndex_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = A_Pin;
  HAL_GPIO_Init(A_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = B_Pin;
  HAL_GPIO_Init(B_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = SleepIdx_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SleepIdx_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = A1_Pin;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(A1_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = A2_Pin;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(A2_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = FaultIdx_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(FaultIdx_GPIO_Port, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
  
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  gIndexMotor.StepsPerRevolution = 200;
  gIndexMotor.StepDelay = 100; // = 100 * 100 us = 10 ms --> 200 * 10 ms --> 2 seconds per revolution
  gIndexMotor.SetPosition = 0;
  gIndexMotor.GetPosition = 0;
  gIndexMotor.NrOfSteps = 4;
}
//-----------------------------------------------------------------------------
//! \brief      Handles the stepper motor control
//! \details    Handles the stepper motor control by moving to the next step at
//!             a certain interval. switches off the motor coils when the 
//!             position is reached or if no position is controlled within 1 ms
//! \param      None
void IDX_Tick(void)
{
  /*
  static uint16_t TickTime = 0;
  int16_t StepsToGo;
  TickTime ++;
  if (TickTime == 100)// 10 ms has passed, so motor coils are switched off to save power
  {
    A1Off();
    A2Off();
    B2Off();
    B1Off();
    IDX_Disable();
  }
  if (TickTime < gIndexMotor.StepDelay) return;//Check if the delay is passed already. If not exit function
  TickTime = 0;
  
  StepsToGo = gIndexMotor.GetPosition - gIndexMotor.SetPosition;//Calculate remaining steps

  if (StepsToGo > 0) //Reverse
  {
    gIndexMotor.CurrentStep --;
    if (gIndexMotor.CurrentStep < 0) gIndexMotor.CurrentStep = gIndexMotor.NrOfSteps - 1;
    gIndexMotor.GetPosition --;
    IDX_Enable();
  }
  else if (StepsToGo < 0) //Forward
  {
    gIndexMotor.CurrentStep ++;
    if (gIndexMotor.CurrentStep >= gIndexMotor.NrOfSteps) gIndexMotor.CurrentStep = 0;
    gIndexMotor.GetPosition ++;
    IDX_Enable();
  }
  else //Nothing to do. Power off the motor to save power
  {
    A1Off();
    A2Off();
    B2Off();
    B1Off();
    IDX_Disable();
    return;
  }
    switch (gIndexMotor.CurrentStep) 
  {
    case 0:  // 1010
      A1On();
      A2Off();
      B2On();
      B1Off();
    break;
    case 1:  // 0110
      A1Off();
      A2On();
      B2On();
      B1Off();
    break;
    case 2:  //0101
      A1Off();
      A2On();
      B2Off();
      B1On();
    break;
    case 3:  //1001
      A1On();
      A2Off();
      B2Off();
      B1On();
    break;
  }
  */
}

//-----------------------------------------------------------------------------


