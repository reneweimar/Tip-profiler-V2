//-----------------------------------------------------------------------------
//! \file       userinterface.c
//! \author     R. Weimar
//! \brief      Contains routines for the userinterface
//! \details    
//! \Attention  
//-----------------------------------------------------------------------------
#include "main.h"
#include "userinterface.h"
#include "stdlib.h"
#include "stdio.h"
#include "work.h"
#include "fonts.h"
#include "i2c.h"
#include "ssd1306.h"
#include "power.h"
#include "eeprom.h"
#include "string.h"
//-----------------------------------------------------------------------------
stcButtonStatus Button[NROFBUTTONS];
uint16_t gCurrentScreen = 65535; //Undefined
uint8_t gParameterNumber;
int16_t gParameterValue;
uint8_t CursorPosition;
uint8_t gParameterDigits;
uint8_t gParameterDecimals;

//-----------------------------------------------------------------------------
//! \brief      Fills the values for entering values and starts the screen
//! \details    Sets the max and min values, value, and curserlocation
//! \param[in]  int16_t NewNumber
void USR_EnterValue(int16_t NewNumber)
{
  if (NewNumber > 100) NewNumber = gParameterNumber - 80;//Parameter 1-20 user parameter, Parameter 21 - 120 service parameter
  gParameterNumber = NewNumber; //Store the current parameter number
  gParameterValue = gMachineType[gMachine/100].Parameters[NewNumber].Value; //Get the current parameter value
  gParameterDecimals = gMachineType[gMachine/100].Parameters[NewNumber].Decimals;
  gParameterDigits = gMachineType[gMachine/100].Parameters[NewNumber].Digits - gMachineType[gMachine/100].Parameters[NewNumber].Decimals;
  CursorPosition = 24 - (gParameterDigits *6);
  if (CursorPosition == 24) CursorPosition = 30;
  USR_ClearScreen(2);
  USR_ShowScreen (2); 
}
//-----------------------------------------------------------------------------
//! \brief      Saves the modified parameter
//! \details    Stores the saved parameter in the parameter array
//! \param      None  
void USR_SaveParameter(void)
{
  
  if (gMachineType[gMachine/100].Parameters[gParameterNumber].Global == 1)
  {
    for (uint8_t i = 0; i < NROFMACHINETYPES; i++)
    {
      gMachineType[i].Parameters[gParameterNumber].Value = gParameterValue;    
      EE_WriteVariable(gParameterNumber+100*i, gParameterValue);
    }
  }
  else
  {
    gMachineType[gMachine/100].Parameters[gParameterNumber].Value = gParameterValue;
    EE_WriteVariable(gParameterNumber+gMachine, gParameterValue);
  }
  //Temp!!!!
  
  if (gParameterNumber == 5) gMachine = gParameterValue;
}
//-----------------------------------------------------------------------------
//! \brief      Calculates the new cursor position
//! \details    Calculates the new cursor position
//! \param      None  
void USR_CursorRight(void)
{
  CursorPosition += 6;
  if (gParameterDecimals == 0)
  {
    if (CursorPosition == 24) CursorPosition = 18;
  }
  else if (gParameterDecimals == 1)
  {
    if ((CursorPosition == 24) || (CursorPosition > 30)) CursorPosition = 30;
  }
  else
  {
    if (CursorPosition == 24) CursorPosition = 30;
    if (CursorPosition > 36) CursorPosition = 36;
  }
  USR_ShowScreen (2); 
}
//-----------------------------------------------------------------------------
//! \brief      Calculates the new cursor position
//! \details    Calculates the new cursor position
//! \param      None  
void USR_CursorLeft(void)
{
  CursorPosition -= 6;
  if (CursorPosition == 24)
  {
    if (gParameterDigits > 0)
    {
      CursorPosition = 18;
    }
    else
    {
      CursorPosition = 30;
    }
  }
  else if ((CursorPosition == 12) && (gParameterDigits == 1))
    CursorPosition = 18;
  else if (CursorPosition < 12)
    CursorPosition = 12;
  USR_ShowScreen (2); 
}
//-----------------------------------------------------------------------------
//! \brief      Handles the up button
//! \details    Calculates the new value based on extremes and cursorposition
//! \param      None  
void USR_CursorUp(void)
{
  int16_t NewValue;
  if (CursorPosition == 36) NewValue = 1;
  if (CursorPosition == 30) NewValue = 10;
  if (CursorPosition == 18) NewValue = 100;
  if (CursorPosition == 12) NewValue = 1000;
  if (gParameterValue + NewValue <= gMachineType[gMachine/100].Parameters[gParameterNumber].Max)
  {
    gParameterValue += NewValue ;
    USR_ShowScreen (2); 
  }
}
//-----------------------------------------------------------------------------
//! \brief      Handles the down button
//! \details    Calculates the new value based on extremes and cursorposition
//! \param      None  
void USR_CursorDown(void)
{
  int16_t NewValue;
  if (CursorPosition == 36) NewValue = 1;
  if (CursorPosition == 30) NewValue = 10;
  if (CursorPosition == 18) NewValue = 100;
  if (CursorPosition == 12) NewValue = 1000;
  if (gParameterValue - NewValue >= gMachineType[gMachine/100].Parameters[gParameterNumber].Min)
  {
    gParameterValue -= NewValue ;
    USR_ShowScreen (2); 
  }
}
//-----------------------------------------------------------------------------
//! \brief      Initiates the OLED display
//! \details    Initiates the low level driver of the OLED and clears the screen
//! \param      None
void USR_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Pin = BtnMenuLeft_Pin;
    HAL_GPIO_Init(BtnMenuLeft_GPIO_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = BtnUpDown_Pin;
    HAL_GPIO_Init(BtnUpDown_GPIO_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = BtnOkRight_Pin;
    HAL_GPIO_Init(BtnOkRight_GPIO_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Pin = BtnCommon1_Pin;
    HAL_GPIO_Init(BtnCommon1_GPIO_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = BtnCommon2_Pin;
    HAL_GPIO_Init(BtnCommon2_GPIO_Port, &GPIO_InitStruct);
    
    if (ssd1306_Init(&hi2c1) != 0) 
    {
        Error_Handler();
    }
    HAL_Delay(100);
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen(&hi2c1);
    HAL_Delay(100);
}
//-----------------------------------------------------------------------------
//! \brief      Displays a message on the screen
//! \details    Displays a message on the OLED display
//! \param[in]  uint16_t NewMessage
void USR_ShowMessage(uint16_t NewMessage)
{
  switch (NewMessage)
  {   
    case 10201: //Factory reset.
    {
      ssd1306_SetCursor(0, 0);
      ssd1306_WriteStringEightBitFont("FACTORY RESET", Font_6x7, White);
      ssd1306_SetCursor(0, 16);
      ssd1306_WriteStringEightBitFont("THIS WILL ERASE ALL  ", Font_6x7, White);
      ssd1306_SetCursor(0, 28);
      ssd1306_WriteStringEightBitFont("CHANGES! PRESS OK TO ", Font_6x7, White);
      ssd1306_SetCursor(0, 40);
      ssd1306_WriteStringEightBitFont("RESET OR # TO CANCEL ", Font_6x7, White);
      ssd1306_SetCursor(0, 57);
      ssd1306_WriteStringEightBitFont("#OK                  ", Font_6x7, White);
      break;
    }
    case 10202: //Stroke motor
    {
      ssd1306_SetCursor(0, 0);
      ssd1306_WriteStringEightBitFont("STROKE MOTOR ", Font_6x7, White);
      ssd1306_SetCursor(0, 16);
      ssd1306_WriteStringEightBitFont("      $ FASTER        ", Font_6x7, White);
      ssd1306_SetCursor(0, 28);
      ssd1306_WriteStringEightBitFont("# ON             , OFF", Font_6x7, White);
      ssd1306_SetCursor(0, 40);
      ssd1306_WriteStringEightBitFont("      & SLOWER        ", Font_6x7, White);
      ssd1306_SetCursor(0, 57);
      ssd1306_WriteStringEightBitFont("#,$&*OK               ", Font_6x7, White);
      break;
    }
    default:
      break;
  }
  ssd1306_UpdateScreen(&hi2c1);
}

//-----------------------------------------------------------------------------
//! \brief      Displays screens
//! \details    Displays the corrsponding screen on the OLED display
//! \param[in]  uint16_t NewScreen
void USR_ShowScreen(uint16_t NewScreen)
{
    char strValue[10];
    int16_t Dig3, Dig2, Dig1, Dig0;  
    uint16_t TopPage;
    gCurrentScreen = NewScreen;
    if ((gCurrentScreen == 9)||(gCurrentScreen == 11)||(gCurrentScreen == 40)) gCurrentScreen = 10;
    if ((gCurrentScreen == 19)||(gCurrentScreen == 21)) gCurrentScreen = 20;
    if ((gCurrentScreen == 0)||(gCurrentScreen == 34)) gCurrentScreen = 30;
    if (gCurrentScreen == 29) gCurrentScreen = 33;
    if (gCurrentScreen == 100) gCurrentScreen = 102;
    if (gServiceMenu)
    {
      if (gCurrentScreen == 104) gCurrentScreen = 101;
      if (gCurrentScreen == (10101 + gParameterMaxService + 1)) gCurrentScreen = 10101;
      if (gCurrentScreen == 10100) gCurrentScreen = 10101 + gParameterMaxService;
      if ((gCurrentScreen >= (10201 + gCommandMaxService + 1))&&(gCurrentScreen <10300)) gCurrentScreen = 10201;
      if (gCurrentScreen == 10200) gCurrentScreen = 10201 + gCommandMaxService;
    }
    else
    {
      if (gCurrentScreen == 103) gCurrentScreen = 101;
      if (gCurrentScreen == 10101 + gParameterMaxUser + 1) gCurrentScreen = 10101;
      if (gCurrentScreen == 10100) gCurrentScreen = 10101 + gParameterMaxUser;
      if ((gCurrentScreen >= 10201 + gCommandMaxUser + 1)&&(gCurrentScreen <10300)) gCurrentScreen = 10201;
      if (gCurrentScreen == 10200) gCurrentScreen = 10201 + gCommandMaxUser;
    }

    switch (gCurrentScreen)
    {   
      case 1: //Splash screen
      {
        USR_ClearScreen(0);
        ssd1306_SetCursor(15, 5);
        ssd1306_DrawLogo(White);
        ssd1306_SetCursor(60, 5);
        ssd1306_WriteStringEightBitFont("VERSION:", Font_6x7, White);
        ssd1306_SetCursor(60, 15);
        sprintf(strValue, "%02u.%02u", VERSIONMAJOR, VERSIONMINOR);
        ssd1306_WriteStringEightBitFont(strValue, Font_6x7, White);
        ssd1306_SetCursor(60, 30);
        ssd1306_WriteStringEightBitFont("TYPE:", Font_6x7, White);
        ssd1306_SetCursor(60, 40);
        if (gMachine==0)
        {
          ssd1306_WriteStringEightBitFont("OBOE    ", Font_6x7, White);
        }
        else
        {
          ssd1306_WriteStringEightBitFont("BASSOON ", Font_6x7, White);
        }
        ssd1306_SetCursor(15, 55);
        ssd1306_WriteString("reed machines", Font_7x10, White);
        break;
      }
      case 2: //Enter value
      {
        ssd1306_SetCursor(0, 0);
        ssd1306_WriteStringEightBitFont("ENTER VALUE  ", Font_6x7, White);
        ssd1306_SetCursor(12, 16);
        ssd1306_WriteStringEightBitFont(gMachineType[gMachine/100].Parameters[gParameterNumber].Name, Font_6x7, White);
        ssd1306_SetCursor(12, 28);
        Dig3 = gParameterValue / 1000;
        Dig2 = (gParameterValue - (Dig3*1000)) / 100;
        Dig1 = (gParameterValue - (Dig3*1000) - (Dig2*100)) / 10;
        Dig0 = gParameterValue - (Dig3*1000) - (Dig2*100) - (Dig1 * 10);
        if (gParameterValue < 0) //Negative
          if (gParameterDecimals == 0)
          {
            if (gParameterDigits <= 1)
              sprintf(strValue, "-  %u    %s", abs(Dig2),gMachineType[gMachine/100].Parameters[gParameterNumber].Unit);
            else
              sprintf(strValue, "- %u%u    %s", abs(Dig3), abs(Dig2),gMachineType[gMachine/100].Parameters[gParameterNumber].Unit);
          }
          else if (gParameterDecimals == 1)
          {
            if (gParameterDigits <= 1)
              sprintf(strValue, "-  %u.%u  %s", abs(Dig2), abs(Dig1),gMachineType[gMachine/100].Parameters[gParameterNumber].Unit);
            else
              sprintf(strValue, "- %u%u.%u  %s", abs(Dig3), abs(Dig2), abs(Dig1),gMachineType[gMachine/100].Parameters[gParameterNumber].Unit);
          }
          else
          {
            if (gParameterDigits <= 1)
              sprintf(strValue, "-  %u.%u%u %s", abs(Dig2), abs(Dig1), abs(Dig0),gMachineType[gMachine/100].Parameters[gParameterNumber].Unit);
            else
              sprintf(strValue, "- %u%u.%u%u %s", abs(Dig3), abs(Dig2), abs(Dig1), abs(Dig0),gMachineType[gMachine/100].Parameters[gParameterNumber].Unit);
          }
        else
          if (gParameterDecimals == 0)
          {
            if (gParameterDigits <= 1)
              sprintf(strValue, "   %u    %s", abs(Dig2),gMachineType[gMachine/100].Parameters[gParameterNumber].Unit);  
            else
              sprintf(strValue, "  %u%u    %s", abs(Dig3), abs(Dig2),gMachineType[gMachine/100].Parameters[gParameterNumber].Unit);
          }
          else if (gParameterDecimals == 1)
          {
            if (gParameterDigits <= 1)
              sprintf(strValue, "   %u.%u  %s", abs(Dig2), abs(Dig1),gMachineType[gMachine/100].Parameters[gParameterNumber].Unit);
            else
              sprintf(strValue, "  %u%u.%u  %s", abs(Dig3), abs(Dig2), abs(Dig1),gMachineType[gMachine/100].Parameters[gParameterNumber].Unit);
          }
          else
          {
            if (gParameterDigits <= 1)
              sprintf(strValue, "   %u.%u%u %s", abs(Dig2), abs(Dig1), abs(Dig0),gMachineType[gMachine/100].Parameters[gParameterNumber].Unit);
            else
              sprintf(strValue, "  %u%u.%u%u %s", abs(Dig3), abs(Dig2), abs(Dig1), abs(Dig0),gMachineType[gMachine/100].Parameters[gParameterNumber].Unit);
          }
        ssd1306_WriteStringEightBitFont(strValue, Font_6x7, White);
        ssd1306_SetCursor(CursorPosition, 40);
        ssd1306_WriteStringEightBitFont("  $  ", Font_6x7, White);
        ssd1306_SetCursor(0, 57);
        ssd1306_WriteStringEightBitFont("#,$&*OK", Font_6x7, White);
        break;
      }
      case 10: //Home
      {
        ssd1306_DrawRectangle(White,0,15,55,35,2);
        ssd1306_SetCursor(65, 28);
        ssd1306_WriteStringEightBitFont("          ", Font_6x7, White);
        ssd1306_SetCursor(65, 22);
        ssd1306_WriteStringEightBitFont("FIND HOME ", Font_6x7, White);
        ssd1306_SetCursor(65, 34);
        ssd1306_WriteStringEightBitFont("POSITION  ", Font_6x7, White);
        ssd1306_SetCursor(9, 24);
        ssd1306_WriteString("HOME", Font_11x18, Black);
        ssd1306_SetCursor(0, 57);
        ssd1306_WriteStringEightBitFont("#,*OK  ", Font_6x7, White);
        ssd1306_SetCursor(80, 57);
        ssd1306_WriteStringEightBitFont(gMachineType[gMachine/100].Name, Font_6x7, White);
        break;
      }
      case 20: //Start
      {
        ssd1306_DrawRectangle(White,0,15,55,35,2);
        ssd1306_SetCursor(65, 28);
        ssd1306_WriteStringEightBitFont("          ", Font_6x7, White);
        ssd1306_SetCursor(65, 22);
        ssd1306_WriteStringEightBitFont("GOTO START", Font_6x7, White);
        ssd1306_SetCursor(65, 34);
        ssd1306_WriteStringEightBitFont("POSITION  ", Font_6x7, White);
        ssd1306_SetCursor(5, 24);
        ssd1306_WriteString("START", Font_11x18, Black);
        ssd1306_SetCursor(0, 57);
        ssd1306_WriteStringEightBitFont("#,*OK  ", Font_6x7, White);
        ssd1306_SetCursor(80, 57);
        ssd1306_WriteStringEightBitFont(gMachineType[gMachine/100].Name, Font_6x7, White);
        break;
      }
      case 30: //Scrape
      {
        ssd1306_DrawRectangle(White,0,15,55,35,2);
        ssd1306_SetCursor(65, 22);
        ssd1306_WriteStringEightBitFont("          ", Font_6x7, White);
        ssd1306_SetCursor(65, 34);
        ssd1306_WriteStringEightBitFont("          ", Font_6x7, White);
        ssd1306_SetCursor(65, 28);
        ssd1306_WriteStringEightBitFont("WHOLE REED", Font_6x7, White);
        ssd1306_SetCursor(0, 24);
        ssd1306_WriteString("SCRAPE", Font_11x18, Black);
        ssd1306_SetCursor(0, 57);
        ssd1306_WriteStringEightBitFont("#,$&*OK", Font_6x7, White);
        ssd1306_SetCursor(80, 57);
        ssd1306_WriteStringEightBitFont(gMachineType[gMachine/100].Name, Font_6x7, White);
        break;
      }
      case 33: //ScrapeNoIndex
      {

        ssd1306_DrawRectangle(White,0,15,55,35,2);
        ssd1306_SetCursor(65, 28);
        ssd1306_WriteStringEightBitFont("          ", Font_6x7, White);
        ssd1306_SetCursor(65, 22);
        ssd1306_WriteStringEightBitFont("NO SIDE   ", Font_6x7, White);
        ssd1306_SetCursor(65, 34);
        ssd1306_WriteStringEightBitFont("STEPS     ", Font_6x7, White);
        ssd1306_SetCursor(0, 24);
        ssd1306_WriteString("SCRAPE", Font_11x18, Black);
        ssd1306_SetCursor(0, 57);
        ssd1306_WriteStringEightBitFont("#,$&*OK", Font_6x7, White);
        ssd1306_SetCursor(80, 57);
        ssd1306_WriteStringEightBitFont(gMachineType[gMachine/100].Name, Font_6x7, White);        
        break;
      }
      case 32: //ScrapeInner
      {
        ssd1306_DrawRectangle(White,0,15,55,35,2);
        ssd1306_SetCursor(65, 28);
        ssd1306_WriteStringEightBitFont("          ", Font_6x7, White);
        ssd1306_SetCursor(65, 22);
        ssd1306_WriteStringEightBitFont("INNER     ", Font_6x7, White);
        ssd1306_SetCursor(65, 34);
        ssd1306_WriteStringEightBitFont("SECTIONS  ", Font_6x7, White);
        ssd1306_SetCursor(0, 24);
        ssd1306_WriteString("SCRAPE", Font_11x18, Black);
        ssd1306_SetCursor(0, 57);
        ssd1306_WriteStringEightBitFont("#,$&*OK", Font_6x7, White);
        ssd1306_SetCursor(80, 57);
        ssd1306_WriteStringEightBitFont(gMachineType[gMachine/100].Name, Font_6x7, White);
        break;
      }
      
      case 31: //ScrapeOuter
      {
        ssd1306_DrawRectangle(White,0,15,55,35,2);
        ssd1306_SetCursor(65, 28);
        ssd1306_WriteStringEightBitFont("          ", Font_6x7, White);
        ssd1306_SetCursor(65, 22);
        ssd1306_WriteStringEightBitFont("OUTER     ", Font_6x7, White);
        ssd1306_SetCursor(65, 34);
        ssd1306_WriteStringEightBitFont("SECTIONS  ", Font_6x7, White);
        ssd1306_SetCursor(0, 24);
        ssd1306_WriteString("SCRAPE", Font_11x18, Black);
        ssd1306_SetCursor(0, 57);
        ssd1306_WriteStringEightBitFont("#,$&*OK", Font_6x7, White);
        ssd1306_SetCursor(80, 57);
        ssd1306_WriteStringEightBitFont(gMachineType[gMachine/100].Name, Font_6x7, White);        
        break;
      }
      //User main menu
      case 101:
      case 102:
      case 103:
      {
        ssd1306_SetCursor(0, 0);
        if (gServiceMenu)
          ssd1306_WriteStringEightBitFont("SERVICE      ", Font_6x7, White);  
        else
          ssd1306_WriteStringEightBitFont("             ", Font_6x7, White);
        ssd1306_SetCursor(0, 16);
        ssd1306_WriteStringEightBitFont("  SETTINGS          ", Font_6x7, White);
        ssd1306_SetCursor(0, 28);
        ssd1306_WriteStringEightBitFont("  COMMANDS          ", Font_6x7, White);
        ssd1306_SetCursor(0, 40);
        if (gServiceMenu)
          ssd1306_WriteStringEightBitFont("  DATA              ", Font_6x7, White);
        else
          ssd1306_WriteStringEightBitFont("                    ", Font_6x7, White);
        ssd1306_SetCursor(0, 57);
        ssd1306_WriteStringEightBitFont("&$,*   ", Font_6x7, White);
        ssd1306_SetCursor(80, 57);
        ssd1306_WriteStringEightBitFont("    1-1", Font_6x7, White);
        ssd1306_SetCursor(0, 16 + (gCurrentScreen - 101) * 12);
        ssd1306_WriteStringEightBitFont(",", Font_6x7,White);
        break;
      }

      case 10201:
      case 10202:
      case 10203:
      case 10204:
      case 10205:
      case 10206:
      case 10207:
      case 10208:
      case 10209:
      case 10210:
      {
        //Find the top of the page
        TopPage = gCurrentScreen - (gCurrentScreen-1)%3;   //10202 -> 10202 - 1 = 10201, 10204 -> 10204 - 0 = 10204
        ssd1306_SetCursor(0, 0);
        if (gServiceMenu)
          ssd1306_WriteStringEightBitFont("SERVICE      ", Font_6x7, White);  
        else
          ssd1306_WriteStringEightBitFont("             ", Font_6x7, White);
        for (uint8_t i = 0;i<3;i++)
        {
          ssd1306_SetCursor(0, 16 + (12 * i));
          if (i + TopPage - 10201 < gCommandMaxService + 1)
          {
            if ((gServiceMenu) || (gCommands[i + TopPage - 10201].UserAccess))
            {
              sprintf(strValue, "  %s", gCommands[i + TopPage - 10201].Name);
              ssd1306_WriteStringEightBitFont(strValue, Font_6x7, White); 
            }
            else
              ssd1306_WriteStringEightBitFont("                    ", Font_6x7, White);
          }
          else
              ssd1306_WriteStringEightBitFont("                    ", Font_6x7, White);
        }
        ssd1306_SetCursor(0, 57);
        ssd1306_WriteStringEightBitFont("&$*OK  ", Font_6x7, White);
        ssd1306_SetCursor(80, 57);
        if (gServiceMenu)
        {
          sprintf(strValue, "    %u-%01u", ((TopPage-10201)/3)+1,(gCommandMaxService+3)/3);
          ssd1306_WriteStringEightBitFont(strValue, Font_6x7, White);
        }
        else
        {
          sprintf(strValue, "    %u-%01u", ((TopPage-10201)/3)+1,(gCommandMaxUser+3)/3);
          ssd1306_WriteStringEightBitFont(strValue, Font_6x7, White);
        }
        ssd1306_SetCursor(0, 16 + (gCurrentScreen - TopPage) * 12);
        ssd1306_WriteStringEightBitFont(",", Font_6x7,White);
        break;
      }
      default:
      {
        //Find the top of the page
        TopPage = gCurrentScreen - gCurrentScreen%3;   //10102 -> 10102 - 1 = 10101, 10104 -> 10104 - 0 = 10104
        ssd1306_SetCursor(0, 0);
        if (gServiceMenu)
          ssd1306_WriteStringEightBitFont("SERVICE     ", Font_6x7, White);  
        else
          ssd1306_WriteStringEightBitFont("            ", Font_6x7, White);
        for (uint8_t i = 0;i<3;i++)
        {
          ssd1306_SetCursor(0, 16 + (12 * i));
          if (i + TopPage - 10101 < gParameterMaxService + 1)
          {
            if ((gServiceMenu) || (gMachineType[gMachine/100].Parameters[i + TopPage - 10101].UserAccess))
            {
              sprintf(strValue, "  %s", gMachineType[gMachine/100].Parameters[i + TopPage - 10101].Name);
              ssd1306_WriteStringEightBitFont(strValue, Font_6x7, White); 
            }
            else
              ssd1306_WriteStringEightBitFont("                    ", Font_6x7, White);
          }
          else
              ssd1306_WriteStringEightBitFont("                    ", Font_6x7, White);
        }
        ssd1306_SetCursor(0, 57);
        //if (gServiceMenu)
          ssd1306_WriteStringEightBitFont("&$,*   ", Font_6x7, White);
        //else
        //  ssd1306_WriteStringEightBitFont("&$*    ", Font_6x7, White);
        ssd1306_SetCursor(80, 57);
        if (gServiceMenu)
        {
          sprintf(strValue, "    %u-%01u", ((TopPage-10101)/3)+1,(gParameterMaxService+3)/3);
          ssd1306_WriteStringEightBitFont(strValue, Font_6x7, White);
        }
        else
        {
          sprintf(strValue, "    %u-%01u", ((TopPage-10101)/3)+1,(gParameterMaxUser+3)/3);
          ssd1306_WriteStringEightBitFont(strValue, Font_6x7, White);
        }
        ssd1306_SetCursor(0, 16 + (gCurrentScreen - TopPage) * 12);
        ssd1306_WriteStringEightBitFont(",", Font_6x7,White);
        break;
      }
    }
    ssd1306_UpdateScreen(&hi2c1);
}
//-----------------------------------------------------------------------------
//! \brief      Shows the battery percentage and symbol
//! \details    Calculates the percentage of the battery and displays the
//!             correct battery indicator
//! \param[in]  newPercentage   Battery percentage
void USR_ShowBattery (uint8_t PercentageNew)
{
    static uint8_t Bars = 0;
    static int8_t BatteryPercentage=-1;//Initially -1
    static uint8_t BatteryPercentageOld=0;
    static uint8_t CntBatteryPercentage;
    static uint16_t BatteryPercentageAverage;
    uint8_t XPos = 84;
    char Percentage[4];

    if (gCurrentScreen == 1) return;  
    CntBatteryPercentage++;
    BatteryPercentageAverage += PercentageNew;
    if (CntBatteryPercentage >= 10)
    {
      BatteryPercentageOld = BatteryPercentage;
      CntBatteryPercentage = 0;
      BatteryPercentage= BatteryPercentageAverage/10;
      BatteryPercentageAverage = 0;
    }
    if ((BatteryPercentage > BatteryPercentageOld)&& (NotPluggedIn()))
    {
      BatteryPercentage = BatteryPercentageOld;
      return; //Prevent jittering of percentages
    }
    if (StandBy()) // full battery
    {
      Bars = 3;
      sprintf(Percentage,"    %s","/");
    }
    else if (PWR_Charging() == 1) //Charging
    {
      Bars++;
      if (Bars == 4) Bars = 0;
      sprintf(Percentage,"    %s","/");
    }
    else //Not plugged in sow percentage
    {
      if (BatteryPercentage==-1)
      {
        Bars = 0;
      }    
      else if (BatteryPercentage >= 66)
        Bars = 3;
      else if (BatteryPercentage < 33)
        Bars = 1;
      else
        Bars = 2; 
      if (BatteryPercentage == -1)
      {
        sprintf(Percentage,"    %s"," ");
      }
      else
      {
        if ((BatteryPercentage < 100)&&(BatteryPercentage > 9)) 
          sprintf(Percentage,"  %u%%",BatteryPercentage);
        else if (BatteryPercentage < 10) 
          sprintf(Percentage,"   %u%%",BatteryPercentage);
        else
          sprintf(Percentage," %u%%",BatteryPercentage);
      }
    }
    ssd1306_DrawRectangle(Black,78, 0,24,7,0);
    ssd1306_SetCursor(XPos, 0);
    ssd1306_WriteStringEightBitFont(Percentage, Font_6x7, White);
    ssd1306_DrawBattery(White,Bars,115,0);
    ssd1306_UpdateScreen(&hi2c1);
}
//-----------------------------------------------------------------------------
//! \brief      Clears the screen
//! \details    Clears the screen buffer by filling with black
//! \param      uint8_t Mode: 0 = All, 1 = Title, 2 = Screen
void USR_ClearScreen (uint8_t ShowTitle)
{
  if (ShowTitle == 0)
    ssd1306_DrawRectangle(Black,0,0,128,64,0);
  else if (ShowTitle == 1)
    ssd1306_DrawRectangle(Black,0,0,128,11,0);
  else if (ShowTitle == 2)
  {
    ssd1306_DrawRectangle(Black,0,12,128,52,0);
    ssd1306_DrawRectangle(White,0,10,128,1,0);
    ssd1306_DrawRectangle(White,0,54,128,1,0);
  }
  
  ssd1306_UpdateScreen(&hi2c1);
}
//-----------------------------------------------------------------------------
//! \brief      Handles the button TimeOn and WaitForRelease counter
//! \details    Increases the TimeOn parameter of each button if this button is
//!             pressed, the WaitForRelease flag is 0 and the TimeOn parameter
//!             is smaller than or equal to USR_PRESSTIMEMAX. If this button is 
//!             not pressed, the WaitForRelease flag and the TimeOn parameter 
//!             are reset to 0.

//!             0 = Menu, 1 = Left, 2 = Up, 3 = Down, 4 = Ok, 5 = Right
//!             Common1 --> Left, Down, Right
//!             Common2 --> Menu, Up, Ok
//! \param      None
void USR_HandleButtons (void)
{
  static uint16_t TickTime = 0; 
  static uint8_t Pushed[6];
  TickTime++;
  if (TickTime == 1)
  {
    BtnCommon1_Low();
  }
  else if (TickTime == 5)
  {
    Pushed[1] = BtnMenuLeft_Pushed();
    Pushed[3] = BtnUpDown_Pushed();
    Pushed[5] = BtnOkRight_Pushed();
  }
  else if (TickTime == 6)
  {
    BtnCommon2_Low();
  }
  else if (TickTime == 10)
  {
    Pushed[0] = BtnMenuLeft_Pushed();
    Pushed[2] = BtnUpDown_Pushed();
    Pushed[4] = BtnOkRight_Pushed();
  }
  else if (TickTime > 10)
  {
    TickTime = 0;
    //Handle the buttons
    for(uint8_t i=0;i<NROFBUTTONS;i++)
    {
      if (Pushed[i]==1) //Button is pushed
      {
        gCounter.User = 0;
        ssd1306_SetContrast(&hi2c1,HIGHCONTRAST);
        if ((Button[i].TimeOn <= USR_PRESSTIMEMAX) && (Button[i].WaitForRelease == 0))
        {
          Button[i].TimeOn += 10;
        }
      }
      else
      {
        Button[i].WaitForRelease = 0;
        if (Button[i].TimeOff < 65520) Button[i].TimeOff += 10;
        Button[i].TimeOn = 0;
      }
    }
  }
}
//-----------------------------------------------------------------------------
//! \brief      Gets the button status of the requested button
//! \details    Returns 1 if the TimeOn parameter of the requested button
//!             equals or is larger than the required time, otherwise returns 0.
//!             Sets the WaitForRelease flag if this is requested by the
//!             ReqWaitForRelease flag
//! \param[in]  enuButtons ReqButton      --> Requested button 
//! \param[in]  uint16_t ReqTime          --> The minimum time the button needs
//!                                           to be pressed by the user
//! \param[in]  uint8_t ReqWaitForRelease --> 0 = Don't wait for button to be
//!                                               released
//!                                           1 = wait for button to be released
//! \param[out] uint8_t ReturnValue       --> 0 = Not pressed or not long enough
//!                                           1 = Pressed for ReqTime or more ms
uint8_t USR_ButtonPressed (enuButtons ReqButton, uint16_t ReqTime, uint8_t ReqWaitForRelease)
{
  uint8_t ReturnValue = 0;
  if (Button[(uint8_t) ReqButton].TimeOn >= ReqTime)
  {
    Button[(uint8_t) ReqButton].TimeOn = 0;
    Button[(uint8_t) ReqButton].WaitForRelease = ReqWaitForRelease;
    ReturnValue = 1;
  }
  return ReturnValue;
}
//-----------------------------------------------------------------------------
//! \brief      Gets the button waitforrelease flag of the requested button
//! \details    Returns the WaitForRelease parameter of the requested button
//! \param[in]  enuButtons ReqButton      --> Requested button 
//! \param[out] uint8_t ReturnValue       --> 0 = WaitForRelease flag is on
//!                                           1 = WaitForRelease flag is off
uint8_t USR_ButtonWaitForRelease (enuButtons ReqButton)
{
  return Button[(uint8_t) ReqButton].WaitForRelease;
}


//-----------------------------------------------------------------------------


