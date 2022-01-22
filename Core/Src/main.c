/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "strokemotor.h"
#include "indexmotor.h"
#include "tim.h"
#include "usart.h"
#include "dma.h"
#include "adc.h"
#include "i2c.h"
#include "userinterface.h"
#include "work.h"
#include "eeprom.h"
#include "cli.h"
#include "power.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	
  
  /* USER CODE END 1 */

	
  /* MCU Configuration------------------------------------------------------
	--*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

HAL_Init();


  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM5_Init();
  MX_TIM8_Init();
  MX_TIM6_Init();
  //TODO MX_USART1_UART_Init();

  /* USER CODE BEGIN 2 */
  USR_Init();
  IDX_Init();
  STR_Init();
  PWR_Init();
  WRK_Init();
  EE_Init();
  //TODO CLI_Init();
  HAL_TIM_Encoder_Start(&htim8,TIM_CHANNEL_ALL); 
  TIM8->CNT = 32767;
  HAL_ADC_Start_DMA(&hadc1,(uint32_t*) &ADC_Converted_Values,1);
  HAL_TIM_Base_Start (&htim6);
  gInitialized = 1;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (STR_HomeFlag == 1)
    {
      STR_HomeFlag = 0;
      gSTR_Motor.Encoder = 0;
      gSTR_Motor.EncoderOld = 0;
      gSTR_Motor.IsHomed = 1;
      if (gWRK_Status.MainStatus == SCRAPEREED)
      {
        gScrape.NextScrape = 1;
        gScrape.NextSideStep = 1;
      }
      else
      {
        gScrape.NextScrape = 0;
        gScrape.NextSideStep = 0;
      }
      if ((gScrape.Status == RightSideLastStep)||(gScrape.Status == LeftSideLastStep)) gSTR_Motor.SetSpeed = STR_GOTOSTARTSPEED;
      if (gSTR_Status.MainStatus == HOME)
      {
        if (gSTR_Motor.GetSpeed == 0) //Encoder signal missing but motor aparently turning
        {
          gSTR_ErrorNumber = 23002;
        }
        else
        {
          STR_Stop();
        }
      }
      if ((gSTR_Status.MainStatus == GOTOSTARTPOSITION) && (gSTR_Status.SubStatus == WAITFORHOMESENSORON))
      {
        gSTR_Motor.SetSpeed = STR_GOTOSTARTSPEED;
        STR_SetStatus(SubStatus,WAITFORSTROKEMOTORSTART);
      }
      if ((gScrape.Status == RightSidePauseRequested) || (gScrape.Status == LeftSidePauseRequested) || (gScrape.Status == NoSideStepPauseRequested)) //Reduce speed so the motor can stop in time
        gSTR_Motor.SetSpeed = STR_GOTOSTARTSPEED;
            
    }
    if ((gSTR_Status.MainStatus == GOTOSTARTPOSITION) && (gSTR_Status.SubStatus == WAITFORSTROKEMOTORSTART) && (gSTR_Motor.Encoder == 300))
    {
      STR_Stop();
    }
    if (((gWRK_Status.MainStatus == SCRAPEREED) || (gWRK_Status.MainStatus == SCRAPENOSIDESTEPS)) && ((gWRK_Status.SubStatus == WAITFORSTROKEMOTORSTOP) || (gWRK_Status.SubStatus == WAITFORUSER)))
    {
      if (gSTR_Motor.Encoder == 300)
      {
        if ((gScrape.NextScrape == 1) || (gWRK_Status.MainStatus == SCRAPENOSIDESTEPS)) //To wait for passing home switch again
        {
          gScrape.NextScrape = 0;
          if (gScrape.Status == RightSideLastStep)
          {
#ifdef IDX_SHOWREALPOSITION
            USR_ShowPosition((int32_t) ((float) gIDX_Motor.GetPosition / gIDX_Motor.UmPerPulse * gMachineType[gMachine/100].Parameters[SIDESTRATIO].Value / 1000));
#else
            USR_ShowPosition((int32_t) ((float) gIDX_Motor.SetPosition / gIDX_Motor.UmPerPulse * gMachineType[gMachine/100].Parameters[SIDESTRATIO].Value / 1000));
#endif
            WRK_SetScrapeStatus (RightSideLastScrape);
          }
          else if (gScrape.Status == RightSideLastScrape)
          {
            WRK_SetScrapeStatus (RightSideEndOfScraping);
            STR_Stop();
          }
          else if (gScrape.Status == LeftSideLastStep)
          {
#ifdef IDX_SHOWREALPOSITION
            USR_ShowPosition((int32_t) ((float) gIDX_Motor.GetPosition / gIDX_Motor.UmPerPulse * gMachineType[gMachine/100].Parameters[SIDESTRATIO].Value / 1000));
#else
            USR_ShowPosition((int32_t) ((float) gIDX_Motor.SetPosition / gIDX_Motor.UmPerPulse * gMachineType[gMachine/100].Parameters[SIDESTRATIO].Value / 1000));
#endif
            WRK_SetScrapeStatus (LeftSideLastScrape);
          }
          else if (gScrape.Status == LeftSideLastScrape)
          {
            WRK_SetScrapeStatus (LeftSideEndOfScraping);
            STR_Stop();
          }
          else if (gScrape.Status == RightSidePauseRequested)
          {
            WRK_SetScrapeStatus (RightSidePaused);
            STR_Stop();
          }
          else if (gScrape.Status == LeftSidePauseRequested)
          {
            WRK_SetScrapeStatus (LeftSidePaused);
            STR_Stop();
          }
          else if (gScrape.Status == NoSideStepPauseRequested)
          {
            WRK_SetScrapeStatus (NoSideStepPaused);
            STR_Stop();
          }
          else if (gScrape.Status != NoSideStep)
          {
#ifdef IDX_SHOWREALPOSITION
              USR_ShowPosition((int32_t) ((float) gIDX_Motor.GetPosition / gIDX_Motor.UmPerPulse * gMachineType[gMachine/100].Parameters[SIDESTRATIO].Value / 1000));
#else
              USR_ShowPosition((int32_t) ((float) gIDX_Motor.SetPosition / gIDX_Motor.UmPerPulse * gMachineType[gMachine/100].Parameters[SIDESTRATIO].Value / 1000));
#endif
          }
        }
      }
      if (gScrape.NextSideStep)
      {
        gScrape.NextSideStep = 0;
      
        if ((gScrape.Status == RightSideNormalStep)||(gScrape.Status == RightSidePauseRequested))//((WRK_SetScrapeStatus (= 0)||(WRK_SetScrapeStatus (= 6))
        {
          if ((gIDX_Motor.SetPosition - gScrape.EndPosition)> gScrape.SideStep) 
          {
            IDX_SetPosition(gIDX_Motor.SetPosition - gScrape.SideStep);
          }
          else
          {
            IDX_SetPosition(gScrape.EndPosition);
            WRK_SetScrapeStatus (RightSideLastStep);//WRK_SetScrapeStatus ( 1;
          }
        }
        else if ((gScrape.Status == LeftSideNormalStep)||(gScrape.Status == LeftSidePauseRequested))//((WRK_SetScrapeStatus (= 3)||(WRK_SetScrapeStatus (= 8))
        {
          if ((abs(gIDX_Motor.SetPosition) - abs(gScrape.EndPosition)) > gScrape.SideStep) 
          {
            IDX_SetPosition(gIDX_Motor.SetPosition + gScrape.SideStep);
          }
          else 
          {
            IDX_SetPosition(gScrape.EndPosition);
            WRK_SetScrapeStatus (LeftSideLastStep);//WRK_SetScrapeStatus ( 4;
          }
        }
      }
    }
  
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
  void SystemClock_Config(void)
  {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  
    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
      Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
      Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV8;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }
  }
/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
