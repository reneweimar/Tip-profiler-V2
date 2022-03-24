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
#include "power.h"
//-----------------------------------------------------------------------------
//! \Global Stroke motor container
stcDCMotor gSTR_Motor;
//! \Global Stroke motor status container
enuSTR_Unit gSTR_Status;
//! \Global Pulsetime for speed calculation
uint16_t gSTR_PulseTime;
//! \Stroke motor speed setting
int32_t STR_Speed;
//! \Stroke motor home flag
uint8_t STR_HomeFlag;
//! \Stroke motor START flag
uint8_t STR_StartFlag;
//! \Global error number for correct display of error number
uint16_t gSTR_ErrorNumber;


//-----------------------------------------------------------------------------
//! \brief      Handles the stroke motor speed
//! \details    Controls the motor speed according to SetSpeed
//! \params     None
void STR_HandleMotor (void)
{
  static uint8_t TickTime = 8; 
  static uint32_t PulseTime;
  static uint32_t PulseTimeOld;
  
  PulseTime += gSTR_PulseTime;

  if (TickTime++ < 9) return; //10ms 
  TickTime = 0;

  if (PulseTime == PulseTimeOld) //No change anymore. Motor is not rotating
  {
    PulseTime = 0;
    gSTR_PulseTime = 0;
  }
  gSTR_Motor.GetSpeed = (int16_t) (60000000 /((float) PulseTime / 10 * (float) gSTR_Motor.PulsesPerRevolution));
  PulseTimeOld = PulseTime;
  PulseTime = 0;
  
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
  if ((gSTR_Motor.Control > 2500) && (gSTR_Motor.GetSpeed == 0)) //Motor is controlled > 25%, but not turning or encoder not detecting any pulses
  {
    gSTR_Motor.TimeOut+=10;
    if (gSTR_Motor.TimeOut > STROKEMOTORTIMEOUT) 
    {
      gSTR_Motor.TimeOut = 0;
      gSTR_ErrorNumber = 23001;
      gSTR_Motor.MainStatus = INACTIVE;
      STR_SetStatus(MainStatus,UNITERROR);
    }
  }
  else
  {
    gSTR_Motor.TimeOut = 0;
  }
  if (gSTR_Motor.MainStatus==ACTIVE)
  {
      STR_SetPWM(CW, gSTR_Motor.Control/100,1);
  }
  else
  {
    STR_Stop();
  }
}
//-----------------------------------------------------------------------------
//! \brief      Controls the stroke motor speed PID
//! \details    Calculates the PID value for the speed control
//! \params     None
static void STR_HandleSpeedPID (void) 
{
    gSTR_Motor.ErrorP = (float) (gSTR_Motor.SetSpeed - gSTR_Motor.GetSpeed);
    gSTR_Motor.ErrorI = gSTR_Motor.ErrorI + gSTR_Motor.ErrorP;
    gSTR_Motor.ErrorD = gSTR_Motor.ErrorP - gSTR_Motor.ErrorPOld;
    gSTR_Motor.ErrorPOld = gSTR_Motor.ErrorP;
    gSTR_Motor.PID = gSTR_Motor.ErrorP * gSTR_Motor.P + gSTR_Motor.ErrorI * gSTR_Motor.I + gSTR_Motor.ErrorD * gSTR_Motor.D;
    gSTR_Motor.Control = gSTR_Motor.Control + (int16_t) gSTR_Motor.PID;
    if (gSTR_Motor.Control < 0)
    {
        gSTR_Motor.Control = 0;
    }
    if (gSTR_Motor.Control > 10000)
    {
        gSTR_Motor.Control = 10000;
    }
}
//-----------------------------------------------------------------------------
//! \brief       Handles the tasks of the stroke motor
//! \details     Evaluates the home sensor and encoder
//! \param       None
void STR_HandleTasks(void)
{
  static int Encoder;
  static uint32_t TimeOut;
  
  switch (gSTR_Status.MainStatus)
  {
    case UNDEFINED:
    {
      break;
    }
    case START:
    {
      gSTR_Motor.MainStatus = ACTIVE;
      PWR_SensorsOn();
      gSTR_Motor.SetSpeed = STR_Speed;
      STR_SetStatus(SubStatus, START);
      break;
    }
    case STOP:
    {
      gSTR_Motor.MainStatus = INACTIVE;
      gSTR_Motor.SetSpeed = 0;
      STR_SetStatus(SubStatus, STOP);
      break;
    }
    case HOME:
    {
      if (gSTR_ErrorNumber >= 23001) //A system error occured. HOME routine will be canceled
      {
        gSTR_Motor.MainStatus = INACTIVE;
        STR_SetStatus(MainStatus,UNITERROR);
      }
      else
      {
        switch (gSTR_Status.SubStatus)
        {
          case UNDEFINED:
          {
            PWR_SensorsOn();
            STR_SetStatus(SubStatus,WAITFORPOWERSENSOR);
            break;
          }
          case WAITFORPOWERSENSOR:
          {
            if (STR_HomeOn()) //Homesensor is on, so first see if it can work by turning the stroke motor
            {
              TimeOut = 0;
              gSTR_Motor.MainStatus = ACTIVE;
              gSTR_Motor.SetSpeed = STR_HOMESPEED;
              STR_SetStatus(SubStatus, WAITFORHOMESENSOROFF);
            }
            else
            {
              gSTR_Motor.MainStatus = ACTIVE;
              gSTR_Motor.SetSpeed = STR_HOMESPEED;
              gSTR_Motor.Encoder = 20;
              STR_SetStatus(SubStatus,WAITFORHOMESENSORON);
            }  
            break;
          }
          case WAITFORHOMESENSOROFF:
          {
            if (STR_HomeOff())
            {
              TimeOut = 0;
              gSTR_Motor.Encoder = 20;
              STR_SetStatus(SubStatus,WAITFORHOMESENSORON);
            }
            else
            {
              TimeOut += 100;
              if (TimeOut > 1500)
              {
                gSTR_ErrorNumber = 23003;
                gSTR_Motor.MainStatus = INACTIVE;
                STR_SetStatus(MainStatus,UNITERROR);
              }
            }
            break;
          }
          case WAITFORHOMESENSORON:
          {
            if (gSTR_Motor.Encoder > 600) //Home sensor not detected
            {
              gSTR_ErrorNumber = 23003;
              gSTR_Motor.MainStatus = INACTIVE;
              STR_SetStatus(MainStatus,UNITERROR);
            }
  					//TODO timeout
            else if (gSTR_Motor.Encoder == Encoder) //Motor is stopped 
            {
              gSTR_Motor.IsHomed = 1;
              gSTR_Motor.IsInStartPosition = 0;
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
      }
      break;
    }
    case GOTOSTARTPOSITION:
    {
      switch (gSTR_Status.SubStatus)
      {
        case UNDEFINED:
        {
          if (gSTR_Motor.Encoder == STARTPOSITION) //Motor is already in start position. Set encoder to enable the start position 
					{
						gSTR_Motor.Encoder = 350;
					}
					gSTR_Motor.MainStatus = ACTIVE;
					gSTR_Motor.IsInStartPosition = 0;
					
					Encoder = 0;
					PWR_SensorsOn();
					STR_SetStatus(SubStatus,WAITFORPOWERSENSOR);
          break;
        }
        case WAITFORPOWERSENSOR:
        {
          if (gSTR_Motor.IsHomed == 0)//Goto home position first
          {
            TimeOut = 0;
            gSTR_Motor.SetSpeed = STR_GOTOSTARTSPEED;
            STR_SetStatus(SubStatus,WAITFORHOMESENSOROFF);
          }  
          else
          {
						gSTR_Motor.MainStatus = ACTIVE;
            gSTR_Motor.SetSpeed = STR_GOTOSTARTSPEED;
            STR_SetStatus(SubStatus,WAITFORSTROKEMOTORSTART);
          }
          break;
        }
        case WAITFORHOMESENSOROFF:
        {
          if (STR_HomeOff())
          {
            TimeOut = 0;
            STR_SetStatus(SubStatus,WAITFORHOMESENSORON);
          }
          else
          {
            TimeOut += 100;
            if (TimeOut > 1000)
            {
              gSTR_ErrorNumber = 23004;
              gSTR_Motor.MainStatus = INACTIVE;
              STR_SetStatus(MainStatus,UNITERROR);
            }
          }
          break;
        }
        case WAITFORHOMESENSORON:
        {
          TimeOut += 100;
          if (TimeOut > 3000)
          {
            gSTR_ErrorNumber = 23003;
            gSTR_Motor.MainStatus = INACTIVE;
            STR_SetStatus(MainStatus,UNITERROR);
          }
          else if ((gSTR_Motor.Encoder == Encoder) )//&& (STR_HomeOn()) && (gSTR_Motor.GetSpeed == 0)) //Motor has stopped and home switch is on
          {
            //gSTR_Motor.Encoder = 0;
						gSTR_Motor.MainStatus = ACTIVE;
            //PWR_SensorsOn();
            gSTR_Motor.SetSpeed = STR_GOTOSTARTSPEED;
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
          if ((gSTR_Motor.Encoder == Encoder)&& (gSTR_Motor.Encoder >= STARTPOSITION - 50)) //Motor has stopped and encoder position is around start position
          {
            if (STR_HomeOn()) //Stroke is at START position, but home sensor is still on
            {
              gSTR_ErrorNumber = 23005;
              gSTR_Motor.MainStatus = INACTIVE;
              STR_SetStatus(MainStatus,UNITERROR);
            }
            else
            {
              gSTR_Motor.IsInStartPosition = 1;
              STR_SetStatus(SubStatus, GOTOSTARTPOSITION);
            }
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
//! \brief     Initiates the stroke motor unit
//! \details    Defines the GPIO and interrupts related to the stroke motor
//! \params     None
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

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_TIM_Base_Start(&htim3);
  
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);

  gSTR_Motor.P = 0.6;
  gSTR_Motor.I = 0.0001;
  gSTR_Motor.D = 3;
  gSTR_Motor.PulsesPerRevolution = 12;
  gSTR_Motor.SpeedControl = 1;
}
//-----------------------------------------------------------------------------
//! \brief       Sets the stroke motor status
//! \details     Sets the status and the speed
//! \param [in]  enuStatus newStatus
//! \param [in]  int32_t newSpeed
//! \param [out] enuStatus STR_Motor.SubStatus or READY
enuStatus STR_Set(enuStatus newStatus, int32_t newSpeed)
{
  if (gSTR_Status.MainStatus == newStatus) //Task already running
  {
    STR_Speed = newSpeed;
    if (gSTR_Status.SubStatus != READY)
    {
      if (gSTR_Status.MainStatus == gSTR_Status.SubStatus)
      {
        STR_SetStatus(SubStatus,READY);
      }
      else
      {
        STR_HandleTasks();
      }
    }
  }
  else
  {
    STR_Speed = newSpeed;
    STR_SetStatus(MainStatus,newStatus);
  }
  return gSTR_Status.SubStatus;
}
//-----------------------------------------------------------------------------
//! \brief      Controls the stroke motor direction and speed
//! \details    Sets the PWM in timer 3 with the calculated value
//! \param[in]  enuStatus newStatus CW = Clockwise, CCW = couter clockwise
//! \param[in]  uint8_t newSpeed  0 = stopped, 100 = max speed
//! \param[in]  uint8_t FastDecay 0 = slow decay, 1 = fast decay
void STR_SetPWM (enuStatus newStatus, uint8_t newSpeed, uint8_t FastDecay)
{
  switch (newStatus)
  {
    case CCW:
    {
      if (FastDecay)
      {
        STR_CW() = 100 - newSpeed;
        STR_CCW()=100;
      }
      else
      {
        STR_CW() = newSpeed;
        STR_CCW()=0;
      }  
      break;
    }
    case CW:
    {
      if (FastDecay)
      {
        STR_CW() = 100;
        STR_CCW() = 100 - newSpeed;
      }
      else
      {
        STR_CW() = 0;
        STR_CCW() = newSpeed;
      }  
      break;
    }
    default:
    {
      if (FastDecay)
      {
        STR_CCW() = 100;
        STR_CW()=100;
      }
      else
      {
        STR_CCW() = 0;
        STR_CW()= 0;
      }  
      break;
    }
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
//! \params      None
void STR_Stop(void)
{
  STR_SetPWM(CW,0,1);
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

