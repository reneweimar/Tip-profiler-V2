//-----------------------------------------------------------------------------
//! \file       indexmotor.h
//! \author     R. Weimar
//! \brief      Contains routines for the index motor
//! \details    
//! \Attention  
//-----------------------------------------------------------------------------
#ifndef _IDX_FUNCTIONS_H
#define _IDX_FUNCTIONS_H
//-----------------------------------------------------------------------------
#include "main.h"
#include "stm32f1xx_hal.h"
//-----------------------------------------------------------------------------
#define FaultIdx_Pin GPIO_PIN_8
#define FaultIdx_GPIO_Port GPIOA
#define SleepIdx_Pin GPIO_PIN_15
#define SleepIdx_GPIO_Port GPIOB
#define IntIndex_Pin GPIO_PIN_0
#define IntIndex_GPIO_Port GPIOA
#define IntIndex_EXTI_IRQn EXTI0_IRQn
#define A1_Pin GPIO_PIN_14
#define A1_GPIO_Port GPIOB
#define A2_Pin GPIO_PIN_13
#define A2_GPIO_Port GPIOB
#define A_Pin GPIO_PIN_8
#define A_GPIO_Port GPIOA
#define B_Pin GPIO_PIN_11
#define B_GPIO_Port GPIOA
#define A1On()HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin,GPIO_PIN_SET)
#define A1Off()HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin,GPIO_PIN_RESET)
#define A2On()HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin,GPIO_PIN_SET)
#define A2Off()HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin,GPIO_PIN_RESET)
#define IDX_Enable() HAL_GPIO_WritePin(SleepIdx_GPIO_Port, SleepIdx_Pin,GPIO_PIN_SET)
#define IDX_Disable() HAL_GPIO_WritePin(SleepIdx_GPIO_Port, SleepIdx_Pin,GPIO_PIN_RESET)
#define IDX_A() HAL_GPIO_ReadPin(A_GPIO_Port, A_Pin)
#define IDX_B() HAL_GPIO_ReadPin(B_GPIO_Port, B_Pin)
#define IDX_HomeOn() HAL_GPIO_ReadPin(IntIndex_GPIO_Port, IntIndex_Pin)==1
#define IDX_HomeOff() HAL_GPIO_ReadPin(IntIndex_GPIO_Port, IntIndex_Pin)==0
#define IDX_CCW()		TIM1->CCR1
#define IDX_CW() 		TIM1->CCR2
typedef  struct
{
  enuStatus MainStatus;
  enuStatus MainStatusOld;
  enuStatus SubStatus;
  enuStatus SubStatusOld;
} enuIDX_Unit;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//IDX_functions
//---------------------- SYSTEM ------------------------
extern stcDCMotor gIDX_Motor;
extern uint8_t gIDX_ResetPosition;
extern void IDX_HandleEncoder (void);
extern void IDX_DecEncoder (void);
extern void IDX_IncEncoder (void);
extern void IDX_Init(void);
extern void IDX_Tick(void);
extern void IDX_HandleMotor (void);
extern enuStatus IDX_Set(enuStatus newStatus);
extern void gIDX_HandleTasks(void);

#endif  // _IDX_FUNCTIONS_H

