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

/* USER CODE END Includes */

/* defines ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
//! \brief  DC motor structure
typedef struct
{
  enuStatus MainStatus;
  uint8_t PositionControl;
  uint8_t SpeedControl;
  uint8_t IsHomed;
  uint8_t IsInStartPosition;
  uint16_t Ratio;
  uint32_t TimeOut;
  uint8_t Direction;
  float P;
	float I;
	float D;
	float PosP;
	float PosI;
	float PosD;
  uint8_t PulsesPerRevolution;   //12
  int16_t SetSpeed;
  int16_t GetSpeed;
  int32_t SetPosition;
  int32_t GetPosition;
  int32_t GetUm;
  int16_t MaxSpeed;
  int32_t Encoder;
  uint16_t EncoderOld;
  uint16_t EncoderNew;
  int32_t EncoderOverFlow;
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
  float UmPerPulse;
  float Factor;
  int16_t GetSpeedOld;
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
