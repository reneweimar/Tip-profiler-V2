//-----------------------------------------------------------------------------
//! \file       power.h
//! \author     R. Weimar
//! \brief      Contains routines for the power unit
//! \details
//! \Attention
//-----------------------------------------------------------------------------
#ifndef _PWR_FUNCTIONS_H
#define _PWR_FUNCTIONS_H
//-----------------------------------------------------------------------------
#include "stm32f1xx_hal.h"
//-----------------------------------------------------------------------------
#define BattVoltage_Pin GPIO_PIN_1
#define BattVoltage_GPIO_Port GPIOC
#define Charging_Pin GPIO_PIN_11
#define Charging_GPIO_Port GPIOB
#define Power_Pin GPIO_PIN_10
#define Power_GPIO_Port GPIOB
#define StandBy_Pin GPIO_PIN_12
#define StandBy_GPIO_Port GPIOB
#define Charging() ((HAL_GPIO_ReadPin(Charging_GPIO_Port, Charging_Pin)==0) && (HAL_GPIO_ReadPin(StandBy_GPIO_Port, StandBy_Pin)==1))
#define StandBy()  ((HAL_GPIO_ReadPin(StandBy_GPIO_Port, StandBy_Pin)==0)   && (HAL_GPIO_ReadPin(Charging_GPIO_Port, Charging_Pin)==1))
#define PluggedIn() (Charging()||StandBy())
#define NotPluggedIn() (Charging()==0) && (StandBy() == 0)
#define BATTVOLTAGEMIN          3600
#define BATTVOLTAGEMAX          4125
//-----------------------------------------------------------------------------
//DSP_functions
//---------------------- SYSTEM ------------------------
extern void PWR_Init(void);
extern uint8_t PWR_Charging(void);
//-----------------------------------------------------------------------------
#endif  // _PWR_FUNCTIONS_H

