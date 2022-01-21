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
#define Sensors_Pin GPIO_PIN_4
#define Sensors_GPIO_Port GPIOA
#define BattVoltage_Pin GPIO_PIN_1
#define BattVoltage_GPIO_Port GPIOC
#define Charging_Pin GPIO_PIN_6
#define Charging_GPIO_Port GPIOA
#define Power_Pin GPIO_PIN_10
#define Power_GPIO_Port GPIOB
#define StandBy_Pin GPIO_PIN_7
#define StandBy_GPIO_Port GPIOA
#define Charging() ((HAL_GPIO_ReadPin(Charging_GPIO_Port, Charging_Pin)==0) && (HAL_GPIO_ReadPin(StandBy_GPIO_Port, StandBy_Pin)==1))
#define StandBy()  ((HAL_GPIO_ReadPin(StandBy_GPIO_Port, StandBy_Pin)==0)   && (HAL_GPIO_ReadPin(Charging_GPIO_Port, Charging_Pin)==1))
#define PluggedIn() (Charging()||StandBy())
#define NotPluggedIn() (Charging()==0) && (StandBy() == 0)
#define PWR_SensorsOn() HAL_GPIO_WritePin(Sensors_GPIO_Port, Sensors_Pin,GPIO_PIN_SET)
#define PWR_SensorsOff() HAL_GPIO_WritePin(Sensors_GPIO_Port, Sensors_Pin,GPIO_PIN_RESET)
#define BATTVOLTAGEMIN          3600
#define BATTVOLTAGEMAX          4050
//-----------------------------------------------------------------------------
//DSP_functions
//---------------------- SYSTEM ------------------------
extern void PWR_Init(void);
extern uint8_t PWR_Charging(void);
//-----------------------------------------------------------------------------
#endif  // _PWR_FUNCTIONS_H

