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

  gIDX_Motor.P = 255;
  gIDX_Motor.I = 1;
  gIDX_Motor.D = 255;
  gIDX_Motor.PosP = 255;
  gIDX_Motor.PosI = 1;
  gIDX_Motor.PosD = 255;
  gIDX_Motor.MaxSpeed = 20;
  gIDX_Motor.SpeedControl = 1;
  gIDX_Motor.PositionControl = 1;
  gIDX_Motor.MainStatus=INACTIVE;
}
//-----------------------------------------------------------------------------
//! \brief      Controls the index motor position PID
//! \details    Calculates the PID value for the position control
//! \param      None
static void IDX_HandlePosPID (void)
{
    gIDX_Motor.PosErrorP = (float) (gIDX_Motor.SetPosition - gIDX_Motor.GetPosition)/10;
    gIDX_Motor.PosErrorI = gIDX_Motor.PosErrorI + gIDX_Motor.PosErrorP;
    gIDX_Motor.PosErrorD = gIDX_Motor.PosErrorP - gIDX_Motor.PosErrorPOld;
    gIDX_Motor.PosErrorPOld = gIDX_Motor.PosErrorP;
    gIDX_Motor.PosPID = gIDX_Motor.PosErrorP * gIDX_Motor.PosP + gIDX_Motor.PosErrorI * gIDX_Motor.PosI/10 + gIDX_Motor.PosErrorD * gIDX_Motor.PosD;
    gIDX_Motor.PosControl = gIDX_Motor.PosControl + (int16_t) gIDX_Motor.PosPID;
    if (gIDX_Motor.PosControl < -(gIDX_Motor.MaxSpeed * 5.333))
    {
        gIDX_Motor.PosControl = -(gIDX_Motor.MaxSpeed * 5.333);
    }
    if (gIDX_Motor.PosControl > (gIDX_Motor.MaxSpeed * 5.333))
    {
        gIDX_Motor.PosControl = gIDX_Motor.MaxSpeed * 5.333;
    }
    if (gIDX_Motor.SpeedControl)
    {
        gIDX_Motor.SetSpeed = gIDX_Motor.PosControl / 5.333;
    }
    else
    {
        gIDX_Motor.Control = gIDX_Motor.PosControl;
    }
}
//-----------------------------------------------------------------------------
//! \brief      Controls the index motor direction and speed
//! \details    Sets the PWM with the calculated speed
//! \param      None

void IDX_Set (enuStatus newStatus, uint8_t newSpeed)
{
  switch (newStatus)
  {
    case CCW:
    {
      IDX_CCW() = newSpeed;
      IDX_CW()=0;
      break;
    }
    case CW:
    {
      IDX_CCW() = 0;
      IDX_CW() = newSpeed;
      break;
    }
    default:
    {
      IDX_CCW() = 0;
      IDX_CW()=0;
      break;
    }
  }
}

//-----------------------------------------------------------------------------
//! \brief      Controls the index motor speed PID
//! \details    Calculates the PID value for the speed control
//! \param      None
static void IDX_HandleSpeedPID (void) //newname
{
    gIDX_Motor.ErrorP = (float) (gIDX_Motor.SetSpeed - gIDX_Motor.GetSpeed)/10;
    gIDX_Motor.ErrorI = gIDX_Motor.ErrorI + gIDX_Motor.ErrorP;
    gIDX_Motor.ErrorD = gIDX_Motor.ErrorP - gIDX_Motor.ErrorPOld;
    gIDX_Motor.ErrorPOld = gIDX_Motor.ErrorP;
    gIDX_Motor.PID = gIDX_Motor.ErrorP * gIDX_Motor.P + gIDX_Motor.ErrorI * gIDX_Motor.I/100 + gIDX_Motor.ErrorD * gIDX_Motor.D;
    gIDX_Motor.Control = gIDX_Motor.Control + (int16_t) gIDX_Motor.PID;
    if (gIDX_Motor.Control < -10000)
    {
        gIDX_Motor.Control = -10000;
    }
    if (gIDX_Motor.Control > 10000)
    {
        gIDX_Motor.Control = 10000;
    }
}
//-----------------------------------------------------------------------------
//! \brief      Handles the index motor speed and position
//! \details    Controls the motor
//! \param      None
void IDX_HandleMotor (void)
{
  float x;

  static uint8_t TickTime = 2; 
  if (TickTime++ < 9) return; //100ms 
  TickTime = 0;

  if (gIDX_Motor.MainStatus==INACTIVE)
  {
    gIDX_Motor.ErrorI = 0;
    gIDX_Motor.PosErrorI = 0;
    gIDX_Motor.Control = 0;
    gIDX_Motor.PosControl = 0;
    gIDX_Motor.SetSpeed = 0; 
    return;
  }
  gIDX_Motor.EncoderMax = gIDX_Motor.Encoder;  
  gIDX_Motor.Encoder = 0;
  x = ((float) gIDX_Motor.EncoderMax) * 16667 / 1000; //10ms, 3ppr, reductor 30:1 -> Encoder/PPR/4/Reduction/Time in ms*60000 -> Encoder/3/4/30/100*60000 -> 16.667
  
  gIDX_Motor.GetSpeed = (int16_t) x; //Negative = CCW, positive = CW
  gIDX_Motor.GetSpeedOld = gIDX_Motor.GetSpeed;
  //Positioning PID
  if (gIDX_Motor.PositionControl)
  {
      IDX_HandlePosPID();
  }
  //Speed PID
  if (gIDX_Motor.SpeedControl)
  {
      IDX_HandleSpeedPID();
  }
  else if (gIDX_Motor.PositionControl == 0)
  {
      gIDX_Motor.Control = gIDX_Motor.SetSpeed * 5.333;
  }
  if (
      //((gIDX_Motor.GetPosition >= gIDX_Motor.SetPosition) && (gIDX_Motor.PositionControl) && (gIDX_Motor.SetSpeed > 0)) ||
      //((gIDX_Motor.GetPosition <= gIDX_Motor.SetPosition) && (gIDX_Motor.PositionControl) && (gIDX_Motor.SetSpeed < 0)) ||
      ((gIDX_Motor.GetPosition == gIDX_Motor.SetPosition) && (gIDX_Motor.PositionControl)) ||
      ((gIDX_Motor.GetSpeed == 0) && (gIDX_Motor.SetSpeed == 0) && (gIDX_Motor.PositionControl == 0))
     )
  {
      IDX_Set(CW,0);
      gIDX_Motor.MainStatus = INACTIVE;
      gIDX_Motor.ErrorI = 0;
      gIDX_Motor.PosErrorI = 0;
      gIDX_Motor.Control = 0;
      IDX_CCW() = 0;
      IDX_CW() = 0;
      gIDX_Motor.PosControl = 0;
      gIDX_Motor.SetSpeed = 0;
  }
  else if (gIDX_Motor.Control < 0)
  {
      IDX_Set(CCW, - gIDX_Motor.Control/100);
  }
  else if (gIDX_Motor.Control > 0)
  {
      IDX_Set(CW, gIDX_Motor.Control/100);
  }
  else
  {
      IDX_Set(CCW, 0);
  }
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
  gIDX_Motor.GetPosition --;
}
//-----------------------------------------------------------------------------
//! \brief      Handles the encoder
//! \details    Increases the encoder of the index motor with 1
//! \param      None
void IDX_IncEncoder (void)
  {
    gIDX_Motor.Encoder ++;
    gIDX_Motor.GetPosition ++;
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


