//-----------------------------------------------------------------------------
//! \file       userinterface.h
//! \author     R. Weimar
//! \brief      Contains routines for the userinterface
//! \details    
//! \Attention  
//-----------------------------------------------------------------------------
#ifndef _USR_FUNCTIONS_H
#define _USR_FUNCTIONS_H
//-----------------------------------------------------------------------------
#include "stm32f1xx_hal.h"
#include "ssd1306.h"
//-----------------------------------------------------------------------------

#define USR_PRESSTIMEMAX 5000
#define USR_SHORTPRESSTIME 1
#define USR_REPEATDELAYFIRST 10
#define USR_REPEATDELAYSECOND 3
#define NROFBUTTONS 6
#define TOPLINE_Y 12
#define BOTTOMLINE_Y 51
#define LINE1_Y 15
#define LINE2_Y 21
#define LINE3_Y 27
#define LINE4_Y 33
#define LINE5_Y 39
#define LINESPACE 6
#define PLUGGEDIN_X 99
#define PAGE_X 80
#define SHOWINSTRUMENTNAME 0


#define BtnMenuLeft_Pin GPIO_PIN_10
#define BtnMenuLeft_GPIO_Port GPIOC
#define BtnUpDown_Pin GPIO_PIN_5
#define BtnUpDown_GPIO_Port GPIOB
#define BtnOkRight_Pin GPIO_PIN_12
#define BtnOkRight_GPIO_Port GPIOC
#define BtnCommon1_Pin GPIO_PIN_3
#define BtnCommon1_GPIO_Port GPIOB
#define BtnCommon2_Pin GPIO_PIN_11
#define BtnCommon2_GPIO_Port GPIOC
/* USER CODE BEGIN Private defines */
#define BtnCommon1() HAL_GPIO_ReadPin(BtnCommon1_GPIO_Port, BtnCommon1_Pin)
#define BtnCommon1_Low() HAL_GPIO_WritePin(BtnCommon1_GPIO_Port, BtnCommon1_Pin,GPIO_PIN_RESET);\
                       HAL_GPIO_WritePin(BtnCommon2_GPIO_Port, BtnCommon2_Pin,GPIO_PIN_SET)
#define BtnCommon2() HAL_GPIO_ReadPin(BtnCommon2_GPIO_Port, BtnCommon2_Pin)
#define BtnCommon2_Low() HAL_GPIO_WritePin(BtnCommon2_GPIO_Port, BtnCommon2_Pin,GPIO_PIN_RESET);\
                       HAL_GPIO_WritePin(BtnCommon1_GPIO_Port, BtnCommon1_Pin,GPIO_PIN_SET)
#define BtnUpDown_Pushed() HAL_GPIO_ReadPin(BtnUpDown_GPIO_Port, BtnUpDown_Pin) == GPIO_PIN_RESET
#define BtnMenuLeft_Pushed() HAL_GPIO_ReadPin(BtnMenuLeft_GPIO_Port, BtnMenuLeft_Pin) == GPIO_PIN_RESET
#define BtnOkRight_Pushed() HAL_GPIO_ReadPin(BtnOkRight_GPIO_Port, BtnOkRight_Pin) == GPIO_PIN_RESET

//-----------------------------------------------------------------------------
//! \brief  Button status enumeration
typedef enum 
{
  BtnMenu=0,
  BtnLeft,
  BtnUp,
  BtnDown,
  BtnOk,
  BtnRight
}enuButtons;
//! \brief  Button info storage structure
typedef struct
{
  uint8_t WaitForRelease;
  uint8_t WaitForReleaseOld;
  uint8_t DelayCounter;
  uint16_t TimeOn;
  uint16_t TimeOff;
} stcButtonStatus;
extern stcButtonStatus Button[NROFBUTTONS];
extern uint8_t gParameterNumber;
extern int16_t gParameterValue;
extern uint32_t gCurrentScreen;
extern uint32_t gReturnScreen;
extern uint32_t gReturnFromErrorScreen;
extern uint32_t gLastScrapeScreen;
extern uint32_t gLastScrapeScreenEndless;

//-----------------------------------------------------------------------------
//USR_functions
//---------------------- SYSTEM ------------------------
//Local functions
void USR_WriteInstrumentName (void);
void USR_WriteKeys (char* newKeys);
//Exported functions
extern uint8_t USR_ButtonPressed (enuButtons ReqButton, uint16_t ReqTime, uint8_t ReqWaitForRelease);
extern uint8_t USR_ButtonWaitForRelease (enuButtons ReqButton);
extern void USR_ClearPosition (void);
extern void USR_ClearScreen (uint8_t ShowTitle);
extern void USR_CursorDown(void);
extern void USR_CursorLeft(void);
extern void USR_CursorRight(void);
extern void USR_CursorUp(void);
extern void USR_DrawLogo (uint8_t newX, uint8_t newY,SSD1306_COLOR color);
extern void USR_DrawProgressFull (uint8_t newX, uint8_t newY, uint8_t newPercentageLeft, uint8_t newPercentageRight, uint8_t newPitch, SSD1306_COLOR color);
extern void USR_DrawProgressPartial (uint8_t newX, uint8_t newY, uint8_t newPercentageLeft, uint8_t newPercentageRight, uint8_t newLocation, SSD1306_COLOR color);
extern void USR_EnterValue(int16_t NewNumber);
extern void USR_HandleButtons (void);
extern void USR_IncreaseCounters(void);
extern void USR_Init(void);
extern void USR_ResetServiceCounter(void);
extern void USR_SaveError(uint16_t newError, uint8_t newShow);
extern void USR_SaveParameter(void);
extern void USR_SetMessage (char* newMessage0, char* newMessage1, char* newMessage2, char* newMessage3, char* newMessage4, char* newMessage5,uint8_t newScreen, uint8_t ClearScreen);
extern void USR_ShowBattery (uint8_t PercentageNew);
extern void USR_ShowPosition (int32_t newPosition);
extern void USR_ShowScreen(uint32_t NewScreen, uint8_t ClearScreen);
//-----------------------------------------------------------------------------
#endif  // _USR_FUNCTIONS_H

