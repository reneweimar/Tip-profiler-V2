//-----------------------------------------------------------------------------
//! \file       strokemotor.h
//! \author     R. Weimar
//! \brief      Contains routines for the stroke motor
//! \details
//! \Attention
//-----------------------------------------------------------------------------
#ifndef _STR_FUNCTIONS_H
#define _STR_FUNCTIONS_H
//-----------------------------------------------------------------------------
#define STR_GOTOSTARTSPEED 1250 //Was 1500
#define STR_HOMESPEED 1250 //Was 1500, but caused overshoot
#define STARTPOSITION 300
#define STROKEMOTORTIMEOUT 5000
#define IntEncoder_Pin GPIO_PIN_1
#define IntEncoder_GPIO_Port GPIOA
#define IntHome_Pin GPIO_PIN_2
#define IntHome_GPIO_Port GPIOA
#define SleepStr_Pin GPIO_PIN_5
#define SleepStr_GPIO_Port GPIOC
#define MotRev_Pin GPIO_PIN_0
#define MotRev_GPIO_Port GPIOB
#define MotFwd_Pin GPIO_PIN_1
#define MotFwd_GPIO_Port GPIOB
#define FaultStr_Pin GPIO_PIN_2
#define FaultStr_GPIO_Port GPIOB
#define STR_Enable() HAL_GPIO_WritePin(SleepStr_GPIO_Port, SleepStr_Pin,GPIO_PIN_SET)
#define STR_Disable() HAL_GPIO_WritePin(SleepStr_GPIO_Port, SleepStr_Pin,GPIO_PIN_RESET)
#define STR_HomeOff() HAL_GPIO_ReadPin(IntHome_GPIO_Port, IntHome_Pin)==1
#define STR_HomeOn() HAL_GPIO_ReadPin(IntHome_GPIO_Port, IntHome_Pin)==0
#define STR_CCW()		TIM3->CCR3
#define STR_CW() 		TIM3->CCR4
typedef  struct
{
  enuStatus MainStatus;
  enuStatus MainStatusOld;
  enuStatus SubStatus;
  enuStatus SubStatusOld;
} enuSTR_Unit;
//-----------------------------------------------------------------------------
//STR_exported variables
//-----------------------------------------------------------------------------
extern stcDCMotor gSTR_Motor;
extern enuSTR_Unit gSTR_Status;
extern uint16_t gSTR_PulseTime;
extern uint8_t STR_HomeFlag;
extern uint16_t gSTR_ErrorNumber;
//-----------------------------------------------------------------------------
//STR_functions
//---------------------- SYSTEM ------------------------
//Local functions
static void STR_HandleSpeedPID (void) ;
void STR_SetPWM (enuStatus newStatus, uint8_t newSpeed, uint8_t FastDecay);
//Exported functions
extern void STR_HandleMotor (void);
extern void STR_HandleTasks(void);
extern void STR_Init(void);
extern enuStatus STR_Set(enuStatus newStatus, int32_t newSpeed);
extern void STR_SetStatus (enuType newType, enuStatus newStatus);
extern void STR_Stop(void);
#endif  // _STR_FUNCTIONS_H

