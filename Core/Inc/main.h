/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "work.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/*#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "work.h"
#include "userinterface.h"
#include "cli.h"
#include "ssd1306.h"
#include "fonts.h"
#include "indexmotor.h"
#include "strokemotor.h"
#include "power.h"
#include "eeprom.h"
*/
/* USER CODE END Includes */

/* defines ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
//! \brief  Index motor structure
typedef struct
{
  enuStatus MainStatus;
  enuStatus MainStatusOld;
  enuStatus SubStatus;
  enuStatus SubStatusOld;
  uint8_t PositionControl;
  uint8_t SpeedControl;
  uint16_t Ratio;
  uint16_t UmPerRev;
  float P;
	float I;
	float D;
	float PosP;
	float PosI;
	float PosD;
  uint8_t PulsesPerRevolution;   //12
  int16_t GetSpeed;
  int16_t GetSpeedOld;
  int16_t SetSpeed;
  int32_t SetPosition;
  int32_t GetPosition;
  float GetUm;
  int16_t MaxSpeed;
  int32_t Encoder;
  int32_t EncoderOld;
  int32_t EncoderMax;
  uint16_t TimerValue;
  uint16_t TimerValueOld;
  uint16_t TimePerRev;
  int32_t Control;
  int32_t PosControl;
  float ErrorPOld;
	float ErrorP;
	float ErrorI;
	float ErrorD;
	float PosErrorPOld;
	float PosErrorP;
	float PosErrorI;
	float PosErrorD;
	float PID;
	float PosPID;
} stcDCMotor;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/


/* USER CODE BEGIN EFP */
void Error_Handler(void);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/


/* USER CODE BEGIN Private defines */






/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
