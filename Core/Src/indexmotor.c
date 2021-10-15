//-----------------------------------------------------------------------------
//! \file       indexmotor.c
//! \author     R. Weimar
//! \brief      Contains routines for the index motor
//! \details
//! \Attention
//-----------------------------------------------------------------------------
#include "indexmotor.h"
#include "main.h"
#include "gpio.h"
#include "tim.h"
//-----------------------------------------------------------------------------
//! \Global step counter
stcDCMotor gIDX_Motor;
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
  //GPIO_InitStruct.Pin = A1_Pin;
  //GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  //HAL_GPIO_Init(A1_GPIO_Port, &GPIO_InitStruct);
  //GPIO_InitStruct.Pin = A2_Pin;
  //GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  //HAL_GPIO_Init(A2_GPIO_Port, &GPIO_InitStruct);
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

  gIDX_Motor.PulsesPerRevolution = 12;

  HAL_TIM_Base_Start(&htim1);

  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
  
}
//-----------------------------------------------------------------------------
//! \brief      Calculates the motor speed
//! \details    Counts the pulses per 100 us and converts to RPM
//! \param      None
void IDX_HandleEncoder (void)
{
  static uint8_t TickTime = 9; 
  if (TickTime++ < 99) return; //100ms 
  TickTime = 0;
  
  gIDX_Motor.GetSpeed = (int16_t) ((float) gIDX_Motor.Encoder / (float) gIDX_Motor.PulsesPerRevolution * 600);
  gIDX_Motor.Encoder = 0;
}
//-----------------------------------------------------------------------------
//! \brief      Handles the encoder
//! \details    Decreases the encoder of the index motor with 1
//! \param      None
void IDX_DecEncoder (void)
{
  gIDX_Motor.Encoder --;
}
//-----------------------------------------------------------------------------
//! \brief      Handles the encoder
//! \details    Increases the encoder of the index motor with 1
//! \param      None
void IDX_IncEncoder (void)
  {
    gIDX_Motor.Encoder ++;
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


