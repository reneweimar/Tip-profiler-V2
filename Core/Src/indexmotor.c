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
#include <stdlib.h>
//-----------------------------------------------------------------------------
//! \Global step counter
stcDCMotor gIDX_Motor;
enuIDX_Unit gIDX_Status;
uint8_t gIDX_ResetPosition;

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
  //HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin, GPIO_PIN_RESET);
  //HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = IntIndex_Pin;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(IntIndex_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pin = A_Pin;
  HAL_GPIO_Init(A_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Pin = B_Pin;
  HAL_GPIO_Init(B_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = SleepIdx_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SleepIdx_GPIO_Port, &GPIO_InitStruct);
  //TIM8
/*  GPIO_InitStruct.Pin = FaultIdx_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(FaultIdx_GPIO_Port, &GPIO_InitStruct);
  */
  //TIM8
  HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  HAL_TIM_Base_Start(&htim1);
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
  gIDX_Motor.P = 0.1;
  gIDX_Motor.I = 0;
  gIDX_Motor.D = 0;
  gIDX_Motor.PosP = 20;
  gIDX_Motor.PosI = 0.001;
  gIDX_Motor.PosD = 500;
  gIDX_Motor.MaxSpeed = 10000;
  gIDX_Motor.SpeedControl = 0;
  gIDX_Motor.PositionControl = 1;
  gIDX_Motor.MainStatus=INACTIVE;
  gIDX_Motor.Ratio = 30;
  gIDX_Motor.UmPerRev = 1500;
  gIDX_Motor.PulsesPerRevolution = 28; //(PPR = 7 * 4)
}
//-----------------------------------------------------------------------------
//! \brief      Sets the IDX_Unit status
//! \details    Sets the selected status (main or sub) and stores the 
//! previous status
//! \param[in]  newType   MainStatus or SubStatus
//! \param[in]  newStatus New status for the selected status
void IDX_SetStatus (enuType newType, enuStatus newStatus)
{
    if (newType == MainStatus)
    {
        gIDX_Status.MainStatusOld = gIDX_Status.MainStatus;
        gIDX_Status.MainStatus = newStatus;
        gIDX_Status.SubStatusOld = gIDX_Status.SubStatus;
        gIDX_Status.SubStatus = UNDEFINED;
    }
    else
    {
        gIDX_Status.SubStatusOld = gIDX_Status.SubStatus;
        gIDX_Status.SubStatus = newStatus;
    }
}

//-----------------------------------------------------------------------------
//! \brief       Sets the index motor status
//! \details     Sets the PWM with the calculated speed
//! \param [in]  enuStatus newStatus
//! \param [out] enuStatus IDX_Motor.SubStatus or READY
enuStatus IDX_Set(enuStatus newStatus)
{
  if (gIDX_Status.MainStatus == newStatus) //Task already running
  {
    if (gIDX_Status.SubStatus != READY)
    {
      if (gIDX_Status.MainStatus == gIDX_Status.SubStatus)
      {
        IDX_SetStatus(SubStatus,READY);
      }
      else
      {
        gIDX_HandleTasks();
      }
    }
  }
  else
  {
    IDX_SetStatus(MainStatus,newStatus);
  }
  return gIDX_Status.SubStatus;
}

//-----------------------------------------------------------------------------
//! \brief      Controls the index motor position PID
//! \details    Calculates the PID value for the position control
//! \param      None
static void IDX_HandlePosPID (void)
{   
    gIDX_Motor.PosErrorP = (float) (gIDX_Motor.SetPosition - gIDX_Motor.GetPosition);
    if ((abs(gIDX_Motor.PosErrorP) < 5) &&(gIDX_Status.MainStatus!=HOME))
    {
      gIDX_Motor.PosErrorP = 0; //Avoid jittering
      gIDX_Motor.PosErrorPOld = 0;
      gIDX_Motor.PosErrorI = 0;
      gIDX_Motor.PosPID = 0;
      gIDX_Motor.PosControl = 0;
    }
    gIDX_Motor.PosErrorI = gIDX_Motor.PosErrorI + gIDX_Motor.PosErrorP;
    gIDX_Motor.PosErrorD = gIDX_Motor.PosErrorP - gIDX_Motor.PosErrorPOld;
    gIDX_Motor.PosErrorPOld = gIDX_Motor.PosErrorP;
    gIDX_Motor.PosPID = gIDX_Motor.PosErrorP * gIDX_Motor.PosP + gIDX_Motor.PosErrorI * gIDX_Motor.PosI + gIDX_Motor.PosErrorD * gIDX_Motor.PosD;
    gIDX_Motor.PosControl = gIDX_Motor.PosControl + (int32_t) gIDX_Motor.PosPID;

    if (((int32_t) gIDX_Motor.PosPID < 5) && (abs(gIDX_Motor.GetPosition - gIDX_Motor.SetPosition) < 3))
    {
      gIDX_Motor.PosControl = 0;
      gIDX_Motor.PosErrorI = 0;
      
    }
    if (gIDX_Motor.PosControl < - gIDX_Motor.MaxSpeed ) //-(gIDX_Motor.MaxSpeed * 5.333))
    {
        gIDX_Motor.PosControl = - gIDX_Motor.MaxSpeed ;//  -(gIDX_Motor.MaxSpeed * 5.333);
    }
    if (gIDX_Motor.PosControl > gIDX_Motor.MaxSpeed ) //(gIDX_Motor.MaxSpeed * 5.333))
    {
        gIDX_Motor.PosControl = gIDX_Motor.MaxSpeed ; //gIDX_Motor.MaxSpeed * 5.333;
    }
    if (gIDX_Motor.SpeedControl)
    {
        gIDX_Motor.SetSpeed = gIDX_Motor.PosControl / 5.333;
    }
    else
    {
        if ((gIDX_Motor.PosControl > -1000) && (gIDX_Motor.PosControl < 1000))
        {
          gIDX_Motor.Control = 0;
        }
        else
        {
          gIDX_Motor.Control = gIDX_Motor.PosControl;
        }
    }
}
//-----------------------------------------------------------------------------
//! \brief      Controls the index motor direction and speed
//! \details    Sets the PWM with the calculated speed
//! \param      None

void IDX_SetPWM (enuStatus newStatus, uint8_t newSpeed)
{
  switch (newStatus)
  {
    case CCW:
    {
      IDX_CW() = 100 - newSpeed;
      IDX_CCW()=100;
      break;
    }
    case CW:
    {
      IDX_CW() = 100;
      IDX_CCW() = 100 - newSpeed;
      break;
    }
    default:
    {
      IDX_CCW() = 100;
      IDX_CW()=100;
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
    gIDX_Motor.ErrorP = (float) (gIDX_Motor.SetSpeed - gIDX_Motor.GetSpeed);
    gIDX_Motor.ErrorI = gIDX_Motor.ErrorI + gIDX_Motor.ErrorP;
    gIDX_Motor.ErrorD = gIDX_Motor.ErrorP - gIDX_Motor.ErrorPOld;
    gIDX_Motor.ErrorPOld = gIDX_Motor.ErrorP;
    gIDX_Motor.PID = gIDX_Motor.ErrorP * gIDX_Motor.P + gIDX_Motor.ErrorI * gIDX_Motor.I + gIDX_Motor.ErrorD * gIDX_Motor.D;
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
//! \brief      Handles the index motor position
//! \details    Controls the motor
//! \param      None
void IDX_HandleMotor (void)
{
  static float PositionOld;
  static float Position;
  
  gIDX_Motor.GetPosition = TIM8->CNT - 32767;

  Position = gIDX_Motor.GetPosition;
  gIDX_Motor.GetSpeed = (int16_t) ((Position - PositionOld) / (float) gIDX_Motor.PulsesPerRevolution * 60000);
  PositionOld = Position;
  
  if (gIDX_Motor.MainStatus==INACTIVE)
  {
    IDX_Disable();
    gIDX_Motor.ErrorI = 0;
    gIDX_Motor.PosErrorI = 0;
    gIDX_Motor.Control = 0;
    gIDX_Motor.PosControl = 0;
    gIDX_Motor.SetSpeed = 0; 
    return;
  }
  else
  {
    IDX_Enable();
  }
  if (gIDX_Motor.PositionControl)
  {
      IDX_HandlePosPID();
  }
  if (gIDX_Motor.SpeedControl)
  {
      IDX_HandleSpeedPID();
  }
  if ((gIDX_Motor.PositionControl == 0) && (gIDX_Motor.SpeedControl == 0))
  {
      gIDX_Motor.Control = gIDX_Motor.SetSpeed;
  }
  if ((gIDX_Motor.Control < 0)&&(gIDX_Motor.MainStatus==ACTIVE))
  {
      IDX_SetPWM(CW, - gIDX_Motor.Control/100);
  }
  else if ((gIDX_Motor.Control > 0)&&(gIDX_Motor.MainStatus==ACTIVE))
  {
      IDX_SetPWM(CCW, gIDX_Motor.Control/100);
  }
  else if (gIDX_Motor.MainStatus==ACTIVE)
  {
      IDX_SetPWM(CW, 0);
  }
  else
  {
    IDX_SetPWM(CW,0);
    gIDX_Motor.MainStatus = INACTIVE;
    gIDX_Motor.ErrorI = 0;
    gIDX_Motor.PosErrorD = 0;
    gIDX_Motor.PosErrorI = 0;
    gIDX_Motor.PosPID = 0;
    gIDX_Motor.Control = 0;
    IDX_CCW() = 0;
    IDX_CW() = 0;
    gIDX_Motor.PosControl = 0;
    gIDX_Motor.SetSpeed = 0;
  }
}

//-----------------------------------------------------------------------------
//! \brief       Handles the tasks of the index motor
//! \details     Evaluates the home sensor and encoder
//! \param       None
void gIDX_HandleTasks(void)
{ 
  switch (gIDX_Status.MainStatus)
  {
    case UNDEFINED:
    {
      break;
    }
    case HOME:
    {
      switch (gIDX_Status.SubStatus)
      {
        case UNDEFINED:
        {
          if (IDX_HomeOn()) //Homesensor is on, so move left
          {
            TIM8->CNT = 42767;
            gIDX_Motor.SetPosition = -1000;
            gIDX_Motor.MainStatus = ACTIVE;
            gIDX_ResetPosition = 1;
            IDX_SetStatus(SubStatus,WAITFORHOMESENSOR);
          }
          else
          {
            TIM8->CNT = 22767;
            gIDX_Motor.SetPosition = 1000;
            gIDX_Motor.MainStatus = ACTIVE;
            gIDX_ResetPosition = 1;
            IDX_SetStatus(SubStatus,WAITFORHOMESENSOR);
          }
          break;
        }
        case WAITFORHOMESENSOR:
        {
          if (abs(gIDX_Motor.GetPosition - gIDX_Motor.SetPosition) < 3)
          {
						if (gIDX_Motor.SetPosition == 0)
            {
							IDX_SetPWM(INACTIVE,0);
							gIDX_Motor.MainStatus = INACTIVE;
							gIDX_Motor.MaxSpeed = 10000;
							IDX_SetStatus(SubStatus, HOME);
						}
						else
						{
							gIDX_ResetPosition = 0;
							gIDX_Motor.SetPosition = 0;
						}
          }
          break;
        }
        default:
          break;
      }
      break;
    }
    default:
      break;
  }
}

//-----------------------------------------------------------------------------


