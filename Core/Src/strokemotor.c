//-----------------------------------------------------------------------------
//! \file       strokemotor.c
//! \author     R. Weimar
//! \brief      Contains routines for the stroke motor
//! \details
//! \Attention
//-----------------------------------------------------------------------------
#include "main.h"
#include "strokemotor.h"
#include "tim.h"
#include "gpio.h"
//-----------------------------------------------------------------------------
stcDCMotor gSTR_Motor;
enuSTR_Unit gSTR_Status;

//-----------------------------------------------------------------------------
//! \brief      Calculates the motor speed
//! \details    Counts the pulses per 100 us and converts to RPM
//! \param      None
void STR_HandleEncoder (void)
{
  static uint8_t TickTime = 8; 
  static int EncoderOld;
  
  
  if (TickTime++ < 99) return; //100ms 
  TickTime = 0;
  int Encoder = gSTR_Motor.Encoder;
  int EncoderValue = Encoder - EncoderOld;
  EncoderOld = Encoder;
  gSTR_Motor.GetSpeed = (int16_t) ((float) EncoderValue / (float) gSTR_Motor.PulsesPerRevolution * 600);
  gSTR_Motor.SetSpeed = (int16_t) (720000 / ((float) gSTR_Motor.TimePerRev * (float) gSTR_Motor.PulsesPerRevolution));
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
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  HAL_GPIO_Init(IntEncoder_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = IntHome_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  HAL_GPIO_Init(IntHome_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = FaultStr_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(FaultStr_GPIO_Port, &GPIO_InitStruct);
  GPIO_InitStruct.Pin = SleepStr_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SleepStr_GPIO_Port, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  gSTR_Motor.PulsesPerRevolution = 12;

  HAL_TIM_Base_Start(&htim3);
  
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);

  //HAL_TIM_Base_Start(&htim6);

  STR_Enable();
}

//-----------------------------------------------------------------------------
//! \brief      Sets the STR_Unit status
//! \details    Sets the selected status (main or sub) and stores the 
//! previous status
//! \param[in]  newType   MainStatus or SubStatus
//! \param[in]  newStatus New status for the selected status
void STR_SetStatus (enuType newType, enuStatus newStatus)
{
    if (newType == MainStatus)
    {
        gSTR_Status.MainStatusOld = gSTR_Status.MainStatus;
        gSTR_Status.MainStatus = newStatus;
        gSTR_Status.SubStatusOld = gSTR_Status.SubStatus;
        gSTR_Status.SubStatus = UNDEFINED;
    }
    else
    {
        gSTR_Status.SubStatusOld = gSTR_Status.SubStatus;
        gSTR_Status.SubStatus = newStatus;
    }
}

//-----------------------------------------------------------------------------
//! \brief       Sets the stroke motor status
//! \details     Sets the PWM with the calculated speed
//! \param [in]  enuStatus newStatus
//! \param [out] enuStatus STR_Motor.SubStatus or READY
enuStatus STR_Set(enuStatus newStatus)
{
  if (gSTR_Status.MainStatus == newStatus) //Task already running
  {
    if (gSTR_Status.SubStatus != READY)
    {
      if (gSTR_Status.MainStatus == gSTR_Status.SubStatus)
      {
        STR_SetStatus(SubStatus,READY);
      }
      else
      {
        gSTR_HandleTasks();
      }
    }
  }
  else
  {
    STR_SetStatus(MainStatus,newStatus);
  }
  return gSTR_Status.SubStatus;
}

//-----------------------------------------------------------------------------
//! \brief      Controls the stroke motor direction and speed
//! \details    Sets the PWM with the calculated speed
//! \param      None

void STR_SetPWM (enuStatus newStatus, uint8_t newSpeed)
{
  switch (newStatus)
  {
    case CCW:
    {
      STR_CW() = 100 - newSpeed;
      STR_CCW()=100;
      break;
    }
    case CW:
    {
      STR_CW() = 100;
      STR_CCW() = 100 - newSpeed;
      break;
    }
    default:
    {
      STR_CCW() = 100;
      STR_CW()=100;
      break;
    }
  }
}

//-----------------------------------------------------------------------------
//! \brief       Handles the tasks of the stroke motor
//! \details     Evaluates the home sensor and encoder
//! \param       None
void gSTR_HandleTasks(void)
{
  static int Encoder;
  
  switch (gSTR_Status.MainStatus)
  {
    case UNDEFINED:
    {
      break;
    }
    case HOME:
    {
      switch (gSTR_Status.SubStatus)
      {
        case UNDEFINED:
        {
          if (STR_HomeOff()) //Homesensor is on, so nothing to do
          {
            STR_SetStatus(SubStatus, HOME);
          }
          else
          {
            STR_SetPWM(CW,50);
            STR_SetStatus(SubStatus,WAITFORHOMESENSOR);
          }  
          break;
        }
        case WAITFORHOMESENSOR:
        {
					//TODO timeout
          if (gSTR_Motor.Encoder == Encoder)
          {
  					gSTR_Motor.Encoder = 0;
            STR_SetStatus(SubStatus, HOME);
          }
          else 
          {
            Encoder = gSTR_Motor.Encoder;
          }
          break;
        }
        default:
          break;
      }
      break;
    }
    case GOTOSTARTPOSITION:
    {
      switch (gSTR_Status.SubStatus)
      {
        case UNDEFINED:
        {
          STR_SetPWM(CW,50);
          STR_SetStatus(SubStatus,WAITFORSTARTPOSITION);
          break;
        }
        case WAITFORSTARTPOSITION:
        {
					//TODO timeout
          if (gSTR_Motor.Encoder == Encoder)
          {
            STR_SetStatus(SubStatus, GOTOSTARTPOSITION);
          }
          else 
          {
            Encoder = gSTR_Motor.Encoder;
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

