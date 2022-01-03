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
  //gSTR_Motor.SetSpeed = (int16_t) (720000 / ((float) gSTR_Motor.TimePerRev * (float) gSTR_Motor.PulsesPerRevolution));
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
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
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

  gSTR_Motor.P = 1.0;
  gSTR_Motor.I = 0.005;
  gSTR_Motor.D = 0.5;
  gSTR_Motor.SpeedControl = 1;
}


//-----------------------------------------------------------------------------
//! \brief      Controls the stroke motor speed PID
//! \details    Calculates the PID value for the speed control
//! \param      None
static void STR_HandleSpeedPID (void) 
{
    gSTR_Motor.ErrorP = (float) (gSTR_Motor.SetSpeed - gSTR_Motor.GetSpeed);
    gSTR_Motor.ErrorI = gSTR_Motor.ErrorI + gSTR_Motor.ErrorP;
    gSTR_Motor.ErrorD = gSTR_Motor.ErrorP - gSTR_Motor.ErrorPOld;
    gSTR_Motor.ErrorPOld = gSTR_Motor.ErrorP;
    gSTR_Motor.PID = gSTR_Motor.ErrorP * gSTR_Motor.P + gSTR_Motor.ErrorI * gSTR_Motor.I + gSTR_Motor.ErrorD * gSTR_Motor.D;
    gSTR_Motor.Control = gSTR_Motor.Control + (int16_t) gSTR_Motor.PID;
    if (gSTR_Motor.Control < -10000)
    {
        gSTR_Motor.Control = -10000;
    }
    if (gSTR_Motor.Control > 10000)
    {
        gSTR_Motor.Control = 10000;
    }
}

//-----------------------------------------------------------------------------
//! \brief      Handles the stroke motor position
//! \details    Controls the motor
//! \param      None
void STR_HandleMotor (void)
{
  static uint8_t TickTime = 8; 
  
  if (TickTime++ < 99) return; //100ms 
  TickTime = 0;

  int Encoder = gSTR_Motor.Encoder;
  int EncoderValue = Encoder - gSTR_Motor.EncoderOld;
  gSTR_Motor.EncoderOld = Encoder;
  gSTR_Motor.GetSpeed = (int16_t) ((float) EncoderValue / (float) gSTR_Motor.PulsesPerRevolution * 600);

  if (gSTR_Motor.MainStatus==INACTIVE)
  {
    STR_Disable();
    gSTR_Motor.ErrorI = 0;
    gSTR_Motor.PosErrorI = 0;
    gSTR_Motor.Control = 0;
    gSTR_Motor.PosControl = 0;
    gSTR_Motor.SetSpeed = 0;
    return;
  }
  else
  {
    STR_Enable();
  }
 
  if (gSTR_Motor.SpeedControl)
  {
      STR_HandleSpeedPID();
  }
  if (gSTR_Motor.MainStatus==ACTIVE)
  {
      STR_SetPWM(CW, gSTR_Motor.Control/100);
  }
  else
  {
    STR_Stop();
  }
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
//! \brief       Stops the stroke motor as fast as possible
//! \details     Sets the PWM to 0 and motor to INACTIVE
//! \param       None
void STR_Stop(void)
{
  STR_SetPWM(CW,0);
  gSTR_Motor.MainStatus = INACTIVE;
  gSTR_Motor.ErrorI = 0;
  gSTR_Motor.PosErrorD = 0;
  gSTR_Motor.PosErrorI = 0;
  gSTR_Motor.PosPID = 0;
  gSTR_Motor.Control = 0;
  gSTR_Motor.PosControl = 0;
  gSTR_Motor.SetSpeed = 0;
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
            gSTR_Motor.MainStatus = ACTIVE;
            gSTR_Motor.SetSpeed = STR_HOMESPEED;//STR_SetPWM(CW,50);
            STR_SetStatus(SubStatus,WAITFORHOMESENSOR);
          }  
          break;
        }
        case WAITFORHOMESENSOR:
        {
					//TODO timeout
          if ((gSTR_Motor.Encoder == Encoder) && (STR_HomeOff()))
          {
  					gSTR_Motor.Encoder = 0;
            gSTR_Motor.EncoderOld = 0;
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
          gSTR_Motor.MainStatus = ACTIVE;
          if (STR_HomeOff()) //Homesensor is on, so stroke motor is home
          {
            gSTR_Motor.Encoder = 0;
            gSTR_Motor.EncoderOld = 0;
            gSTR_Motor.SetSpeed = STR_GOTOSTARTSPEED;//STR_SetPWM(CW,50);
            STR_SetStatus(SubStatus,WAITFORSTROKEMOTORSTART);
          }
          else //Goto home position first
          {
            gSTR_Motor.SetSpeed = STR_GOTOSTARTSPEED;//STR_SetPWM(CW,50);
            STR_SetStatus(SubStatus,WAITFORHOMESENSOR);
          }  
          break;
        }
        case WAITFORHOMESENSOR:
        {
					//TODO timeout
          if ((gSTR_Motor.Encoder == Encoder) && (STR_HomeOff()) && (gSTR_Motor.GetSpeed == 0)) //Motor has stopped and home switch is on
          {
            gSTR_Motor.Encoder = 0;
            gSTR_Motor.EncoderOld = 0;
						gSTR_Motor.MainStatus = ACTIVE;
            gSTR_Motor.SetSpeed = STR_GOTOSTARTSPEED;//STR_SetPWM(CW,50);
            STR_SetStatus(SubStatus,WAITFORSTROKEMOTORSTART);
          }
          else 
          {
            Encoder = gSTR_Motor.Encoder;
          }
          break;
        }
        case WAITFORSTROKEMOTORSTART:
        {
					//TODO timeout
          if ((gSTR_Motor.Encoder == Encoder)&& (gSTR_Motor.Encoder >= STARTPOSITION - 50)) //Motor has stopped and encoder position is aroung start position
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

