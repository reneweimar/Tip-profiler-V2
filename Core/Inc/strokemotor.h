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

//-----------------------------------------------------------------------------
extern stcDCMotor gSTR_Motor;
//-----------------------------------------------------------------------------
//STR_functions
//---------------------- SYSTEM ------------------------
extern void STR_Init(void);
extern void STR_HandleEncoder (void);

#endif  // _STR_FUNCTIONS_H

