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
uint32_t gCurrentScreen = 0xffffffff; //Undefined
uint32_t gLastScrapeScreen = 30;
uint32_t gLastScrapeScreenEndless = 40;
uint32_t gReturnScreen;
uint32_t gReturnFromErrorScreen;
char USR_Message[6][50];
uint8_t gParameterNumber;
int16_t gParameterValue;
uint8_t CursorPosition;
uint8_t gParameterDigits;
uint8_t gParameterDecimals;
uint8_t CharWidth[8];
uint8_t CharPos[8];
uint8_t CurPos[8];
int8_t gCurrentError;
char CharValue[8];


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
  
  if ((ReqWaitForRelease==0)&&(ReturnValue == 1))
  {
    if (Button[(uint8_t) ReqButton].WaitForReleaseOld == 0) //User can hold the button (Delay 10)
    {
      Button[(uint8_t) ReqButton].WaitForReleaseOld = 1;
      Button[(uint8_t) ReqButton].DelayCounter = USR_REPEATDELAYFIRST;
    }
    else
    {
      if (Button[(uint8_t) ReqButton].DelayCounter > 0)
      {
        Button[(uint8_t) ReqButton].DelayCounter --;
        ReturnValue = 0;
      }
      else
      {
        ReturnValue = 1;
    Button[(uint8_t) ReqButton].DelayCounter = USR_REPEATDELAYSECOND;
      }
    }
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
//! \brief      Clears the position of the index
//! \details    Clears the position in um
//! \param      None
void USR_ClearPosition (void)
{
  ssd1306_WriteStringEightBitFont(64, 22,"              ",Font_6x10, White);
}
//-----------------------------------------------------------------------------
//! \brief      Clears the screen
//! \details    Clears the screen buffer by filling with black
//! \param      uint8_t newArea: 0 = All, 1 = Title whole, 2 = Screen between lines, 3 = Title without battery, 4 = Keys area, 5 = LINE1, 6 = LINE2, 7 = LINE3, 8 = LINE4, 9 = LINE5
void USR_ClearScreen (uint8_t newArea)
{
  if (newArea == 0)
    ssd1306_DrawRectangle(Black,0,0,128,64,0);
  else if (newArea == 1)
    ssd1306_DrawRectangle(Black,0,0,128,TOPLINE_Y-1,0);
  else if (newArea == 2)
  {
    ssd1306_DrawRectangle(Black,0,TOPLINE_Y+1,128,BOTTOMLINE_Y-1,0);
    ssd1306_DrawRectangle(White,0,TOPLINE_Y ,128,1,0);
    ssd1306_DrawRectangle(White,0,BOTTOMLINE_Y,128,1,0);
  }
  else if (newArea == 3)
  {
    ssd1306_DrawRectangle(Black,0,0,PLUGGEDIN_X - 1,TOPLINE_Y-1,0);
  }
  else if (newArea == 4)
  {
    ssd1306_DrawRectangle(Black,0,BOTTOMLINE_Y + 1,PAGE_X - 1,63 - BOTTOMLINE_Y,0);
  }
  else if (newArea == 5)
  {
    ssd1306_DrawRectangle(Black,0,LINE1_Y,128,LINE3_Y-LINE1_Y,0);
  }
  else if (newArea == 6)
  {
    ssd1306_DrawRectangle(Black,0,LINE2_Y,128,LINE3_Y-LINE1_Y,0);
  }
  else if (newArea == 7)
  {
    ssd1306_DrawRectangle(Black,0,LINE3_Y,128,LINE3_Y-LINE1_Y,0);
  }
  else if (newArea ==8)
  {
    ssd1306_DrawRectangle(Black,0,LINE4_Y,128,LINE3_Y-LINE1_Y,0);
  }
  else if (newArea ==9)
  {
    ssd1306_DrawRectangle(Black,0,LINE5_Y,128,LINE3_Y-LINE1_Y,0);
  }
  ssd1306_UpdateScreen();
}
//-----------------------------------------------------------------------------
//! \brief      Handles the down button
//! \details    Calculates the new value based on extremes and cursorposition
//! \param      None  
void USR_CursorDown(void)
{
  int16_t NewValue;
  int16_t Options[5];
  int8_t NewOption;
  if (gMachineType[gMachine/100].Parameters[gParameterNumber].Options == 0) //Normal editing value
  {
    /*if (CursorPosition == 36) NewValue = 1;
    if (CursorPosition == 30) NewValue = 10;
    if (CursorPosition == 18) NewValue = 100;
    if (CursorPosition == 12) NewValue = 1000;
    if (CursorPosition == 6) NewValue = 10000;
    */
    if (CursorPosition == 6) NewValue = 1;
    if (CursorPosition == 5) NewValue = 10;
    if (CursorPosition == 3) NewValue = 100;
    if (CursorPosition == 2) NewValue = 1000;
    if (CursorPosition == 1) NewValue = 10000;
    if (gParameterValue - NewValue >= gMachineType[gMachine/100].Parameters[gParameterNumber].Min)
    {
      gParameterValue -= NewValue ;
      USR_ShowScreen (2,1); 
    }
  }
  else //Select a value from the option
  {
    Options[0] = gMachineType[gMachine/100].Parameters[gParameterNumber].Option1;
    Options[1] = gMachineType[gMachine/100].Parameters[gParameterNumber].Option2;
    Options[2] = gMachineType[gMachine/100].Parameters[gParameterNumber].Option3;
    Options[3] = gMachineType[gMachine/100].Parameters[gParameterNumber].Option4;
    Options[4] = gMachineType[gMachine/100].Parameters[gParameterNumber].Option5;

    for (uint8_t i=0; i < 5; i++)
    {

      if (gParameterValue == Options[i])
      {
        NewOption = i; //Option2 -> Option1
        if (NewOption == 0)
        {
          NewOption = gMachineType[gMachine/100].Parameters[gParameterNumber].Options;
        }
    gParameterValue = Options[NewOption-1];
    USR_ShowScreen (2,1); 
        return; //Cancel evaluation of the rest
      }
    }
  }
}
//-----------------------------------------------------------------------------
//! \brief      Calculates the new cursor position
//! \details    Calculates the new cursor position
//! \param      None  
void USR_CursorLeft(void)
{
  if (CursorPosition > 1) 
  {
    if (CharWidth[CursorPosition-1] > 0)
    {
        CursorPosition --;
    }
  }
  if (CursorPosition == 4)
  {
    CursorPosition = 3;
  }
  USR_ShowScreen (2,1); 
}
//-----------------------------------------------------------------------------
//! \brief      Calculates the new cursor position
//! \details    Calculates the new cursor position
//! \param      None  
void USR_CursorRight(void)
{
  if (CursorPosition < 7) 
  {
    if (CharWidth[CursorPosition+1] > 0)
    {
        CursorPosition ++;
    }
  }
  if (CursorPosition == 4) CursorPosition = 5;
  USR_ShowScreen (2,1); 
}

//-----------------------------------------------------------------------------
//! \brief      Handles the up button
//! \details    Calculates the new value based on extremes and cursorposition
//! \param      None  
void USR_CursorUp(void)
{
  int16_t NewValue;
  int16_t Options[5];
  int8_t NewOption;
  if (gMachineType[gMachine/100].Parameters[gParameterNumber].Options == 0) //Normal editing value
  {
    /*if (CursorPosition == 36) NewValue = 1;
    if (CursorPosition == 30) NewValue = 10;
    if (CursorPosition == 18) NewValue = 100;
    if (CursorPosition == 12) NewValue = 1000;
    if (CursorPosition == 6) NewValue = 10000;
    */
    if (CursorPosition == 6) NewValue = 1;
    if (CursorPosition == 5) NewValue = 10;
    if (CursorPosition == 3) NewValue = 100;
    if (CursorPosition == 2) NewValue = 1000;
    if (CursorPosition == 1) NewValue = 10000;
    if (gParameterValue + NewValue <= gMachineType[gMachine/100].Parameters[gParameterNumber].Max)
    {
      gParameterValue += NewValue ;
      USR_ShowScreen (2,1); 
    }
  }
  else //Select a value from the option
  {
    Options[0] = gMachineType[gMachine/100].Parameters[gParameterNumber].Option1;
    Options[1] = gMachineType[gMachine/100].Parameters[gParameterNumber].Option2;
    Options[2] = gMachineType[gMachine/100].Parameters[gParameterNumber].Option3;
    Options[3] = gMachineType[gMachine/100].Parameters[gParameterNumber].Option4;
    Options[4] = gMachineType[gMachine/100].Parameters[gParameterNumber].Option5;

    for (uint8_t i=0; i < 5; i++)
    {

      if (gParameterValue == Options[i])
      {
        NewOption = i+2; //Option1 -> Option2
        if (NewOption > gMachineType[gMachine/100].Parameters[gParameterNumber].Options)
        {
          NewOption = 1;
        }
    gParameterValue = Options[NewOption-1];
    USR_ShowScreen (2,1); 
        return; //Cancel evaluation of the rest
      }
    }
  }
}
//-----------------------------------------------------------------------------
//! \brief      Draws the logo on the screen
//! \details    Draws reed machines logo in the screen buffer
//! \param[in]  uint8_t newX
//! \param[in]  uint8_t newY
//! \param[in]  SSD1306_COLOR color -> Logo color (Black or white)
void USR_DrawLogo (uint8_t newX, uint8_t newY, SSD1306_COLOR color)
{
  uint8_t i,j,k;
  uint8_t EndColumn;
  uint32_t b;

  ssd1306_SetCursor(newX, newY);
  
  for (i = 0; i < 43; i++)
  {
    for (k = 0; k<3; k++)
    {
      b = Logo37x43[i*4+k];
      if (k==2)
        EndColumn = 5;
      else
        EndColumn = 16;
      for (j = 0; j < EndColumn; j++)
      {
        if ((b << j) & 0x8000)
        {
          ssd1306_DrawPixel(SSD1306.CurrentX + j + 16*k, (SSD1306.CurrentY + i), (SSD1306_COLOR) color);
        }
        else
        {
          ssd1306_DrawPixel(SSD1306.CurrentX + j +16*k, (SSD1306.CurrentY + i), (SSD1306_COLOR)!color);
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
//! \brief      Draws the full progress bar on the screen
//! \details    Draws the progress bar for scraping in the screen buffer
//! \param[in]  uint8_t newX
//! \param[in]  uint8_t newY
//! \param[in]  uint8_t newPercentageLeft
//! \param[in]  uint8_t newPercentageRight
//! \param[in]  uint8_t newPitch //Distance between the lines (2 = Small steps, 4 = Big steps)
//! \param[in]  SSD1306_COLOR color -> Logo color (Black or white)
void USR_DrawProgressFull (uint8_t newX, uint8_t newY, uint8_t newPercentageLeft, uint8_t newPercentageRight, uint8_t newPitch, SSD1306_COLOR color)
{
    uint8_t PLeft, PRight;
    ssd1306_DrawRectangle(Black,newX,LINE1_Y,81,LINE3_Y-LINE1_Y,0);
    ssd1306_DrawRectangle(color, newX + 40, newY, 1, 6, 0);
    //LeftSide
    for (uint8_t i = 0; i<40 ; i +=newPitch)
    {   
        PLeft = (40 - i) * 100 / 40 ;
        if (PLeft <= newPercentageLeft)
            ssd1306_DrawRectangle(color, newX + i, newY + 2, 1, 2, 0);    
        PRight = (40 - i) * 100 / 40 ;
        if (PRight <= newPercentageRight)
            ssd1306_DrawRectangle(color, newX + 80 - i, newY + 2, 1, 2, 0);    
    }
}

//-----------------------------------------------------------------------------
//! \brief      Draws the partial progress bar on the screen
//! \details    Draws the progress bar for scraping in the screen buffer
//! \param[in]  uint8_t newX
//! \param[in]  uint8_t newY
//! \param[in]  uint8_t newPercentageLeft
//! \param[in]  uint8_t newPercentageRight
//! \param[in]  uint8_t newLocation //Location to be scraped (0 = Inner, 1 = Outer)
//! \param[in]  SSD1306_COLOR color -> Logo color (Black or white)
void USR_DrawProgressPartial (uint8_t newX, uint8_t newY, uint8_t newPercentageLeft, uint8_t newPercentageRight, uint8_t newLocation, SSD1306_COLOR color)
{
    uint8_t PLeft, PRight;
    ssd1306_DrawRectangle(Black,newX,LINE1_Y,81,LINE3_Y-LINE1_Y,0);
    //Draw middle line
    ssd1306_DrawRectangle(color, newX + 40, newY, 1, 6, 0);
    if (newLocation == 0) //Inner parts
    {
        ssd1306_DrawRectangle(color, newX, newY, 1, 6, 0);    
        ssd1306_DrawRectangle(color, newX + 80, newY, 1, 6, 0);
    }
    else //Outer parts
    {
        ssd1306_DrawRectangle(color, newX + 20, newY, 1, 6, 0);
        ssd1306_DrawRectangle(color, newX + 60, newY, 1, 6, 0);
    }
    //LeftSide
    for (uint8_t i = 0; i<20 ; i += 2)
    {   
        PLeft = (20 - i) * 100 / 20 ;
        if (newLocation == 0) //Inner parts
        {
            if (PLeft <= newPercentageLeft)
                ssd1306_DrawRectangle(color, newX + 20 + i, newY + 2, 1, 2, 0);    
        }
        else  //Outer parts
        {
            if (PLeft <= newPercentageLeft)
                ssd1306_DrawRectangle(color, newX + i, newY + 2, 1, 2, 0);        
        }

        PRight = (20 - i) * 100 / 20 ;
        if (newLocation == 0) //Inner parts
        {
            if (PRight <= newPercentageRight)
                ssd1306_DrawRectangle(color, newX + 60 - i, newY + 2, 1, 2, 0);                
        }
        else  //Outer parts
        {
            if (PRight <= newPercentageRight)
                ssd1306_DrawRectangle(color, newX + 80 - i, newY + 2, 1, 2, 0);                
        }
    }
}

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
  CursorPosition = 0;//24 - (gParameterDigits *6);
  //if (CursorPosition == 24) CursorPosition = 30;
  USR_ShowScreen (2,1); 
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
    Pushed[1] = BtnMenuLeft_Pushed() ;
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
      if (Pushed[i]== 1) //Button is pushed
      {
        gCounter.User = 0;
        WRK_UpdateContrast = HIGHCONTRAST;
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
        Button[i].WaitForReleaseOld = 0;
        Button[i].DelayCounter = USR_REPEATDELAYFIRST;
      }
    }
  }
}
//-----------------------------------------------------------------------------
//! \brief      Increases the counters and saves them to EEPROM
//! \details    Stores the counter in the current machine
//! \param      None  
void USR_IncreaseCounters(void)
{
  uint16_t CounterLSB;
  uint16_t CounterMSB;
  gCounter.MasterCounter ++;
  CounterLSB = (uint16_t) gCounter.MasterCounter ;
  CounterMSB = gCounter.MasterCounter / 0x10000;
  EE_WriteVariable(2000 + gMachine, CounterLSB);
  EE_WriteVariable(2001 + gMachine, CounterMSB);
  gCounter.ServiceCounter ++;
  CounterLSB = (uint16_t) gCounter.ServiceCounter ;
  CounterMSB = gCounter.ServiceCounter / 0x10000;
  EE_WriteVariable(2002 + gMachine, CounterLSB);
  EE_WriteVariable(2003 + gMachine, CounterMSB);
  
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
    
    if (ssd1306_Init() != 0) 
    {
        Error_Handler();
    }
    HAL_Delay(300);
    ssd1306_Fill();//ssd1306_Fill(Black);
    ssd1306_UpdateScreen();
    HAL_Delay(100);
}
//-----------------------------------------------------------------------------
//! \brief      Resets the service counter and saves them to EEPROM
//! \details    Resets only service counter. Master counter remains
//! \param      None  
void USR_ResetServiceCounter(void)
{
  gCounter.ServiceCounter = 0;
  gCounter.ServiceCounter = 0;
  EE_WriteVariable(2002 + gMachine, 0);        
  EE_WriteVariable(2003 + gMachine, 0);
}
//-----------------------------------------------------------------------------
//! \brief      Saves the last error
//! \details    Stores the last error in the error array
//! \param[in]  uint16_t newError
//! \param[in]  uint8_t newShow (0 = not show error number, 1 = show error number)
void USR_SaveError(uint16_t newError, uint8_t newShow)
{
  char strResult[20];
  uint16_t StoredError;

  EE_ReadVariable(Errors[0], &StoredError); //Read the error in that place

  if (StoredError!=newError) // if error is the same as the stored one, no need to write again.
  {
    EE_WriteVariable(Errors[0], newError);
    Errors[Errors[0]-1000] = newError;
  }
  Errors[0]++;
  if (Errors[0]==1100) Errors[0]=1001;
  EE_WriteVariable(1000, Errors[0]);
  if (newShow)
  {
    sprintf(strResult, ": %u",newError);
    ssd1306_WriteStringEightBitFont(30,0,strResult, Font_6x10, White);
  }
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
  //Make sure after chaning machine type new parameters and name are used
  if (gParameterNumber == MACHINETYPE)
  {
    gMachine = gParameterValue ; 
  }
}

//-----------------------------------------------------------------------------
//! \brief      Fills error message line 0 to 2
//! \details    Fills gUSR_Errormessage
//! \param[in]  char* newMessage0, newMessage1, newMessage2, newMessage3
void USR_SetMessage (char* newMessage0, char* newMessage1, char* newMessage2, char* newMessage3, char* newMessage4, char* newMessage5,uint8_t newScreen, uint8_t ClearScreen)
{
  uint8_t Different = 0;
  if (strcmp(USR_Message[0], newMessage0))
  {
    strcpy (USR_Message[0], newMessage0);
    Different = 1;
  }
  if (strcmp(USR_Message[1], newMessage1))
  {
    strcpy (USR_Message[1], newMessage1);
    Different = 1;
  }
  if (strcmp(USR_Message[2], newMessage2))
  {
    strcpy (USR_Message[2], newMessage2);
    Different = 1;
  }
  if (strcmp(USR_Message[3], newMessage3))
  {
    strcpy (USR_Message[3], newMessage3);
    Different = 1;
  }
  if (strcmp(USR_Message[4], newMessage4))
  {
    strcpy (USR_Message[4], newMessage4);
    Different = 1;
  }
  if (strcmp(USR_Message[5], newMessage5))
  {
    strcpy (USR_Message[5], newMessage5);
    Different = 1;
  }
  //Show screen only if the information is different or if the screen now shown is not 3 or 4.
  if (((newScreen) && (Different)) || (gCurrentScreen >= 10)) USR_ShowScreen(newScreen, ClearScreen);
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
    //uint8_t XPos = 84;
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
      Bars = 4;
      sprintf(Percentage,"%s","\\");
    }
    else if (PWR_Charging() == 1) //Charging
    {
      Bars++;
      if (Bars == 5) Bars = 0;
      sprintf(Percentage,"%s","\\");
    }
    else //Not plugged in show percentage
    {
      if (BatteryPercentage<=0)
      {
        Bars = 0;
      }    
      else if (BatteryPercentage >= 75)
        Bars = 4;
      else if (BatteryPercentage >= 50)
        Bars = 3;
      else if (BatteryPercentage >= 25)
        Bars = 2;
      else
        Bars = 1; 
      if (BatteryPercentage == -1)
      {
        sprintf(Percentage,"%s","  ");
      }
      /*else
      {
        if ((BatteryPercentage < 100)&&(BatteryPercentage > 9)) 
          sprintf(Percentage,"  %u%%",BatteryPercentage);
        else if (BatteryPercentage < 10) 
          sprintf(Percentage,"   %u%%",BatteryPercentage);
        else
          sprintf(Percentage," %u%%",BatteryPercentage);
      }
      */
    }
    
    ssd1306_DrawRectangle(Black,78, 0,24,7,0);
    
    ssd1306_WriteStringEightBitFont(PLUGGEDIN_X, 0,Percentage, Font_6x10, White);
    ssd1306_DrawBattery (107,0,Bars,White);//ssd1306_DrawBattery(White,Bars,115,0);
    ssd1306_UpdateScreen();
}
//-----------------------------------------------------------------------------
//! \brief      Displays the position of the index
//! \details    Displays the position in um
//! \param[in]  int32_t newPosition
void USR_ShowPosition (int32_t newPosition)
{
  char strValue[50];
  sprintf(strValue,"%d UM    ",newPosition);
  ssd1306_WriteStringEightBitFont(64, 22,strValue,Font_6x10, White);
}

//-----------------------------------------------------------------------------
//! \brief      Displays screens
//! \details    Displays the corresponding screen on the OLED display
//! \param[in]  uint16_t NewScreen
void USR_ShowScreen(uint32_t NewScreen, uint8_t ClearScreen)
{
    char strValue[50];
    int16_t Dig[11];  
    uint16_t TopPage;
    char txtSign[2];
    gCurrentScreen = NewScreen;
    if (ClearScreen) USR_ClearScreen(2);
    if (((gCurrentScreen >= 10) && (gCurrentScreen < 19)) || ((gCurrentScreen >= 50) && (gCurrentScreen < 59))) gCurrentScreen = 10;
    if ((gCurrentScreen >= 20) && (gCurrentScreen < 29)) gCurrentScreen = 20;
    if (gServiceMenu)
    {
      if (gCurrentScreen == 100) gCurrentScreen = 101 + gMainMenuMaxService;
      if (gCurrentScreen == 102 + gMainMenuMaxService) gCurrentScreen = 101;
      if (gCurrentScreen == (10101 + gParameterMaxService + 1)) gCurrentScreen = 10101;
      if (gCurrentScreen == 10100) gCurrentScreen = 10101 + gParameterMaxService;
      if ((gCurrentScreen >= (10201 + gCommandMaxService + 1))&&(gCurrentScreen <10300)) gCurrentScreen = 10201;
      if (gCurrentScreen == 10200) gCurrentScreen = 10201 + gCommandMaxService;
    }
    else
    {
      if (gCurrentScreen == 100) gCurrentScreen = 101 + gMainMenuMaxUser;
      if (gCurrentScreen == 102 + gMainMenuMaxUser) gCurrentScreen = 101;
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
        USR_DrawLogo(15,5, White);
        ssd1306_WriteStringEightBitFont(60,5,"Version:", Font_6x10, White);
        sprintf(strValue, "%02u.%02u.%04u", VERSIONMAJOR, VERSIONMINOR, VERSIONTWEAK);
        ssd1306_WriteStringEightBitFont(60,17,strValue, Font_6x10, White);
        ssd1306_WriteStringEightBitFont(60, 30,"Type:", Font_6x10, White);
        ssd1306_WriteStringEightBitFont(60, 42,gMachineType[gMachine/100].Name, Font_6x10, White);
        
        ssd1306_WriteString(15, 55,"reed machines", Font_7x10, White);
        break;
      }
      case 2: //Enter value
      {
        if (gMachineType[gMachine/100].Parameters[gParameterNumber].Options == 0) //Enter value
          ssd1306_WriteStringEightBitFont(0,0,"ENTER VALUE    ", Font_6x10, White);
        else //Select value
          ssd1306_WriteStringEightBitFont(0,0,"SELECT VALUE   ", Font_6x10, White);
        ssd1306_WriteStringEightBitFont(12, 16,gMachineType[gMachine/100].Parameters[gParameterNumber].Name, Font_6x10, White);
        Dig[4] = gParameterValue / 10000;
        Dig[3] = (gParameterValue - (Dig[4]*10000)) / 1000;
        Dig[2] = (gParameterValue - (Dig[4]*10000) - (Dig[3]*1000)) / 100;
        Dig[1] = (gParameterValue - (Dig[4]*10000) - (Dig[3]*1000) - (Dig[2]*100)) / 10;
        Dig[0] = gParameterValue - (Dig[4]*10000) - (Dig[3]*1000) - (Dig[2]*100) - (Dig[1] * 10);

        
        //gParameterDigits can be 1 or 2 or 3
        //gParameterDecimals can be 0,1,2 or 3
        for (uint8_t i; i < 8; i ++)
        {
          CharWidth[i]=0;
          CharPos[i]=0;
          CurPos[i]=0;
          CharValue[i]='0';
        }
        //Negative sign
        if (gParameterValue < 0) //Negative
        {
          CharWidth[0] = ssd1306_GetCharWidth('-',Font_6x10);
          CharValue[0] = 45;
          CharWidth[0] = ssd1306_GetCharWidth(CharValue[0],Font_6x10);
          strcpy (txtSign,"-");
        }
        else
        {
          strcpy (txtSign," ");  
        }
        //No decimals
        if (gParameterDecimals == 0)
        {
          
          
          CharValue[3]=Dig[2]+48;
          CharWidth[3]=ssd1306_GetCharWidth(CharValue[3],Font_6x10);
          
          if (gParameterDigits <= 1)
            sprintf(strValue, "%s  %u    %s", txtSign,abs(Dig[2]),gMachineType[gMachine/100].Parameters[gParameterNumber].Unit);
          else if (gParameterDigits == 2)
        {
        CharValue[2]=Dig[3]+48;
        CharWidth[2]=ssd1306_GetCharWidth(CharValue[2],Font_6x10);
        sprintf(strValue, "%s %u%u    %s", txtSign, abs(Dig[3]), abs(Dig[2]),gMachineType[gMachine/100].Parameters[gParameterNumber].Unit);
        }
        else if (gParameterDigits == 3)
        {
            CharValue[1]=Dig[4]+48;
            CharWidth[1]=ssd1306_GetCharWidth(CharValue[1],Font_6x10);
            CharValue[2]=Dig[3]+48;
            CharWidth[2]=ssd1306_GetCharWidth(CharValue[2],Font_6x10);
            sprintf(strValue, "%s%u%u%u    %s", txtSign, abs(Dig[4]), abs(Dig[3]), abs (Dig[2]),gMachineType[gMachine/100].Parameters[gParameterNumber].Unit);
        }
        }
        //1 decimal
        else if (gParameterDecimals == 1)
        {
          CharValue[3]=Dig[2]+48;
          CharWidth[3]=ssd1306_GetCharWidth(CharValue[3],Font_6x10);
          CharValue[5]=Dig[1]+48;
          CharWidth[5]=ssd1306_GetCharWidth(CharValue[5],Font_6x10);
              if (gParameterDigits <= 1)
          {
            sprintf(strValue, "%s  %u.%u  %s", txtSign, abs(Dig[2]), abs(Dig[1]),gMachineType[gMachine/100].Parameters[gParameterNumber].Unit);
          }
          else if (gParameterDigits == 2)
          {
          CharValue[2]=Dig[3]+48;
          CharWidth[2]=ssd1306_GetCharWidth(CharValue[2],Font_6x10);
                sprintf(strValue, "%s %u%u.%u  %s", txtSign, abs(Dig[3]), abs(Dig[2]), abs(Dig[1]),gMachineType[gMachine/100].Parameters[gParameterNumber].Unit);
          }
      else if (gParameterDigits == 3)
          {
      CharValue[1]=Dig[4]+48;
            CharWidth[1]=ssd1306_GetCharWidth(CharValue[1],Font_6x10);
      CharValue[2]=Dig[3]+48;
      CharWidth[2]=ssd1306_GetCharWidth(CharValue[2],Font_6x10);
      sprintf(strValue, "%s%u%u%u.%u  %s", txtSign, abs(Dig[4]), abs(Dig[3]), abs (Dig[2]), abs (Dig[1]),gMachineType[gMachine/100].Parameters[gParameterNumber].Unit);
      }
        }
        //2 or 3 decimals
        else
        {
          if (gParameterDigits <= 1)
          {
      if (gParameterDecimals <= 2)
      {
        CharValue[3]=Dig[2]+48;
        CharWidth[3]=ssd1306_GetCharWidth(CharValue[3],Font_6x10);
        CharValue[5]=Dig[1]+48;
        CharWidth[5]=ssd1306_GetCharWidth(CharValue[5],Font_6x10);
        CharValue[6]=Dig[0]+48;
        CharWidth[6]=ssd1306_GetCharWidth(CharValue[6],Font_6x10);
        sprintf(strValue, "%s  %u.%u%u %s", txtSign, abs(Dig[2]), abs(Dig[1]), abs(Dig[0]),gMachineType[gMachine/100].Parameters[gParameterNumber].Unit);
      }
      else  //3 decimals
      {
        CharValue[3]=Dig[3]+48;
        CharWidth[3]=ssd1306_GetCharWidth(CharValue[3],Font_6x10);
        CharValue[5]=Dig[2]+48;
        CharWidth[5]=ssd1306_GetCharWidth(CharValue[5],Font_6x10);
        CharValue[6]=Dig[1]+48;
        CharWidth[6]=ssd1306_GetCharWidth(CharValue[6],Font_6x10);
        CharValue[7]=Dig[0]+48;
        CharWidth[7]=ssd1306_GetCharWidth(CharValue[7],Font_6x10);

        sprintf(strValue, "%s  %u.%u%u%u %s", txtSign, abs(Dig[3]), abs(Dig[2]), abs(Dig[1]), abs(Dig[0]),gMachineType[gMachine/100].Parameters[gParameterNumber].Unit);
      }
          }
          else if (gParameterDigits == 2)
          {
            CharValue[2]=Dig[3]+48;
            CharWidth[2]=ssd1306_GetCharWidth(CharValue[2],Font_6x10);
            CharValue[3]=Dig[2]+48;
            CharWidth[3]=ssd1306_GetCharWidth(CharValue[3],Font_6x10);
            CharValue[5]=Dig[1]+48;
            CharWidth[5]=ssd1306_GetCharWidth(CharValue[5],Font_6x10);
            CharValue[6]=Dig[0]+48;
            CharWidth[6]=ssd1306_GetCharWidth(CharValue[6],Font_6x10);
            sprintf(strValue, "%s %u%u.%u%u %s", txtSign, abs(Dig[3]), abs(Dig[2]), abs(Dig[1]), abs(Dig[0]),gMachineType[gMachine/100].Parameters[gParameterNumber].Unit);
          }
        }
        uint8_t CursorWidth = ssd1306_GetCharWidth(36,Font_6x10);
        CharPos[4] = 24; //Decimal point always in the same location
    CharValue[4] = 46;
    CharWidth[4]=ssd1306_GetCharWidth(46,Font_6x10);
        CharPos[3] = CharPos[4] - CharWidth[3];
        CurPos[3] = CharPos[3] - (CursorWidth - CharWidth[3])/2;
        CharPos[2] = CharPos[3] - CharWidth[2];
        CurPos[2] = CharPos[2] - (CursorWidth - CharWidth[2])/2;
        CharPos[1] = CharPos[2] - CharWidth[1];
        CurPos[1] = CharPos[1] - (CursorWidth - CharWidth[1])/2;
        CharPos[0] = CharPos[1] - CharWidth[0];
        CurPos[0] = CharPos[0] - (CursorWidth - CharWidth[2])/2;
        CharPos[5] = CharPos[4] + CharWidth[4];
        CurPos[5] = CharPos[5] - (CursorWidth - CharWidth[5])/2;
        CharPos[6] = CharPos[5] + CharWidth[5];
        CurPos[6] = CharPos[6] - (CursorWidth - CharWidth[6])/2;
        CharPos[7] = CharPos[6] + CharWidth[6];
        CurPos[7] = CharPos[7] - (CursorWidth - CharWidth[7])/2;
        if (CursorPosition == 0)
        {
            if (gParameterDigits <= 1) 
                CursorPosition = 3;
            else 
                CursorPosition = 4 - gParameterDigits; 
        }
        for(uint8_t i = 0; i < 8; i ++)
        {
            if (CharWidth[i] > 0)
            {
        ssd1306_SetCursor(CharPos[i], 28);
                ssd1306_WriteCharEightBitFont(CharValue[i], Font_6x10, White);
            }
        }
    ssd1306_WriteStringEightBitFont(0, 40,"             ", Font_6x10, White);
        ssd1306_WriteStringEightBitFont(CurPos[CursorPosition], 40,"$", Font_6x10, White);
        sprintf(strValue, "%s", gMachineType[gMachine/100].Parameters[gParameterNumber].Unit);
        ssd1306_WriteStringEightBitFont(CharPos[gParameterDecimals + 4] + 10, 28, strValue, Font_6x10, White);
        //ssd1306_WriteStringEightBitFont(12, 28, strValue, Font_6x10, White);
        //ssd1306_WriteStringEightBitFont(CursorPosition, 40,"  $  ", Font_6x10, White);
        ssd1306_WriteStringEightBitFont(0, BOTTOMLINE_Y + 3,"                     ", Font_6x10, White);
        if (gMachineType[gMachine/100].Parameters[gParameterNumber].Options == 0) //Enter value
          ssd1306_WriteStringEightBitFont(0, BOTTOMLINE_Y + 3,"<>$&*OK", Font_6x10, White);
        else
          ssd1306_WriteStringEightBitFont(0, BOTTOMLINE_Y + 3,"$&*OK  ", Font_6x10, White);
        break;
      }
      case 3: //Error screen
      case 4: //Message screen 
      {
        USR_ClearScreen(3);
        if (gCurrentScreen == 3)
          ssd1306_WriteStringEightBitFont(0,0,"Error", Font_6x10, White);
        else
          ssd1306_WriteStringEightBitFont(0,0,"Message", Font_6x10, White);
        for (uint8_t i = 0; i<5;i++)
        {
          if (strcmp("",USR_Message[i]))
          {
            ssd1306_WriteStringEightBitFont(0, LINE1_Y + i*LINESPACE,USR_Message[i], Font_6x10, White);
          }
        }
        USR_WriteKeys(USR_Message[5]);
        USR_WriteInstrumentName();
        break;
      }
      case 9:
      case 10: //Home
      case 11:
      {
        gCurrentScreen = 10;
        USR_ClearScreen(3);
        ssd1306_WriteStringEightBitFont(0, LINE3_Y,"Go to home position ", Font_6x10, White);
        ssd1306_WriteStringEightBitFont(0, 0,"Home", Font_6x10, White);
        USR_WriteKeys("< > * OK");
        USR_WriteInstrumentName();
        break;
      }
      case 19:
      case 20: //Start
      case 21:
      {
        gCurrentScreen = 20;
        USR_ClearScreen(3);
        ssd1306_WriteStringEightBitFont(0,LINE3_Y,"Go to start position", Font_6x10, White);
        ssd1306_WriteStringEightBitFont(0,0,"Start", Font_6x10, White);
        USR_WriteKeys("< > * OK");
        USR_WriteInstrumentName();

        break;
      }
      case 34:
      case 30: //Scrape big steps
      {
        gCurrentScreen = 30;
        USR_ClearScreen(3);
        ssd1306_WriteStringEightBitFont(0, 0,"Scrape", Font_6x10, White);
        ssd1306_WriteStringEightBitFont(0,  LINE1_Y,"With big side steps", Font_6x10, White);
        //Calculate the 2 digits of the scrape speed
        Dig[0] = pSCRAPESPEED / 100; 
        Dig[1] = (pSCRAPESPEED % 100) /10;
        //Calculate the 3 digits of the scrape width
        Dig[2] = pSCRAPEWIDTH / 1000;
        Dig[3] = (pSCRAPEWIDTH % 1000)/100;
        Dig[4] = (pSCRAPEWIDTH - Dig[2] * 1000 - Dig[3] * 100)/10;
        //Calculate the 3 digits of the side step small
        Dig[5] = pSIDESTEPBIG / 100;
        Dig[6] = (pSIDESTEPBIG % 100)/10;
        Dig[7] = pSIDESTEPBIG - Dig[5] * 100 - Dig[6] * 10;
        //Create the string and send to the display
        ssd1306_WriteStringEightBitFont(0,  LINE3_Y,"SS:", Font_6x10, White);
        sprintf(strValue, "%u.%u", Dig[0], Dig[1]);
        ssd1306_WriteStringEightBitFont(0,  LINE5_Y,strValue, Font_6x10, White);
        ssd1306_WriteStringEightBitFont(31,  LINE3_Y,"SW:", Font_6x10, White);
        sprintf(strValue, "%u%u.%u", Dig[2], Dig[3], Dig[4]);
        ssd1306_WriteStringEightBitFont(31,  LINE5_Y,strValue, Font_6x10, White);
        ssd1306_WriteStringEightBitFont(68,  LINE3_Y,"BSS:", Font_6x10, White);
        sprintf(strValue, "%u.%u%u", Dig[5], Dig[6], Dig[7]);
        ssd1306_WriteStringEightBitFont(68,  LINE5_Y,strValue, Font_6x10, White);
        //Write the key options to the display
        USR_WriteKeys("< > $ & * OK");
        //Write the intrument name to the display
        USR_WriteInstrumentName();
        break;
      }
      case 31: //Scrape small steps
      {
        USR_ClearScreen(3);
        ssd1306_WriteStringEightBitFont(0, 0,"Scrape", Font_6x10, White);
        ssd1306_WriteStringEightBitFont(0,  LINE1_Y,"With small side steps", Font_6x10, White);
        //Calculate the 2 digits of the scrape speed
        Dig[0] = pSCRAPESPEED / 100; 
        Dig[1] = (pSCRAPESPEED % 100) /10;
        //Calculate the 3 digits of the scrape width
        Dig[2] = pSCRAPEWIDTH / 1000;
        Dig[3] = (pSCRAPEWIDTH % 1000)/100;
        Dig[4] = (pSCRAPEWIDTH - Dig[2] * 1000 - Dig[3] * 100)/10;
        //Calculate the 3 digits of the side step small
        Dig[5] = pSIDESTEPSMALL / 100;
        Dig[6] = (pSIDESTEPSMALL % 100)/10;
        Dig[7] = pSIDESTEPSMALL - Dig[5] * 100 - Dig[6] * 10;
        //Create the string and send to the display
        ssd1306_WriteStringEightBitFont(0,  LINE3_Y,"SS:", Font_6x10, White);
        sprintf(strValue, "%u.%u", Dig[0], Dig[1]);
        ssd1306_WriteStringEightBitFont(0,  LINE5_Y,strValue, Font_6x10, White);
        ssd1306_WriteStringEightBitFont(31,  LINE3_Y,"SW:", Font_6x10, White);
        sprintf(strValue, "%u%u.%u", Dig[2], Dig[3], Dig[4]);
        ssd1306_WriteStringEightBitFont(31,  LINE5_Y,strValue, Font_6x10, White);
        ssd1306_WriteStringEightBitFont(68,  LINE3_Y,"SSS:", Font_6x10, White);
        sprintf(strValue, "%u.%u%u", Dig[5], Dig[6], Dig[7]);
        ssd1306_WriteStringEightBitFont(68,  LINE5_Y,strValue, Font_6x10, White);
        //Write the key options to the display
        USR_WriteKeys("< > $ & * OK");
        //Write the intrument name to the display
        USR_WriteInstrumentName();
        break;
      }
      case 32: //ScrapeOuter
      {
        USR_ClearScreen(3);
        ssd1306_WriteStringEightBitFont(0, 0,"Scrape", Font_6x10, White);
        ssd1306_WriteStringEightBitFont(0,  LINE1_Y,"Outer parts", Font_6x10, White);
        //Calculate the 2 digits of the scrape speed
        Dig[0] = pSCRAPESPEED / 100; 
        Dig[1] = (pSCRAPESPEED % 100) /10;
        //Calculate the 3 digits of the scrape width
        Dig[2] = pSCRAPEWIDTH / 1000;
        Dig[3] = (pSCRAPEWIDTH % 1000)/100;
        Dig[4] = (pSCRAPEWIDTH - Dig[2] * 1000 - Dig[3] * 100)/10;
        //Calculate the 3 digits of the side step small
        Dig[5] = pSIDESTEPSMALL / 100;
        Dig[6] = (pSIDESTEPSMALL % 100)/10;
        Dig[7] = pSIDESTEPSMALL - Dig[5] * 100 - Dig[6] * 10;
        //Calculate the 3 digits of the scrape width
        Dig[8] = pSCRAPEWIDTHINNER / 1000;
        Dig[9] = (pSCRAPEWIDTHINNER % 1000)/100;
        Dig[10] = (pSCRAPEWIDTHINNER - Dig[8] * 1000 - Dig[9] * 100)/10;
        //Create the string and send to the display
        ssd1306_WriteStringEightBitFont(0,  LINE3_Y,"SS:", Font_6x10, White);
        sprintf(strValue, "%u.%u", Dig[0], Dig[1]);
        ssd1306_WriteStringEightBitFont(0,  LINE5_Y,strValue, Font_6x10, White);
        ssd1306_WriteStringEightBitFont(31,  LINE3_Y,"SW:", Font_6x10, White);
        sprintf(strValue, "%u%u.%u", Dig[2], Dig[3], Dig[4]);
        ssd1306_WriteStringEightBitFont(31,  LINE5_Y,strValue, Font_6x10, White);
        ssd1306_WriteStringEightBitFont(68,  LINE3_Y,"SSS:", Font_6x10, White);
        sprintf(strValue, "%u.%u%u", Dig[5], Dig[6], Dig[7]);
        ssd1306_WriteStringEightBitFont(68,  LINE5_Y,strValue, Font_6x10, White);
        ssd1306_WriteStringEightBitFont(105,  LINE3_Y,"OIW:", Font_6x10, White);
        sprintf(strValue, "%u%u.%u", Dig[8], Dig[9], Dig[10]);
        ssd1306_WriteStringEightBitFont(105,  LINE5_Y,strValue, Font_6x10, White);
        //Write the key options to the display
        USR_WriteKeys("< > $ & * OK");
        //Write the intrument name to the display
        USR_WriteInstrumentName();
        break;
      }
      case 29:
      case 33: //ScrapeInner
      {
        gCurrentScreen = 33;
        USR_ClearScreen(3);
        ssd1306_WriteStringEightBitFont(0, 0,"Scrape", Font_6x10, White);
        ssd1306_WriteStringEightBitFont(0,  LINE1_Y,"Inner parts", Font_6x10, White);
        //Calculate the 2 digits of the scrape speed
        Dig[0] = pSCRAPESPEED / 100; 
        Dig[1] = (pSCRAPESPEED % 100) /10;
        //Calculate the 3 digits of the scrape width
        Dig[2] = pSCRAPEWIDTH / 1000;
        Dig[3] = (pSCRAPEWIDTH % 1000)/100;
        Dig[4] = (pSCRAPEWIDTH - Dig[2] * 1000 - Dig[3] * 100)/10;
        //Calculate the 3 digits of the side step small
        Dig[5] = pSIDESTEPSMALL / 100;
        Dig[6] = (pSIDESTEPSMALL % 100)/10;
        Dig[7] = pSIDESTEPSMALL - Dig[5] * 100 - Dig[6] * 10;
        //Calculate the 3 digits of the scrape width
        Dig[8] = pSCRAPEWIDTHINNER / 1000;
        Dig[9] = (pSCRAPEWIDTHINNER % 1000)/100;
        Dig[10] = (pSCRAPEWIDTHINNER - Dig[8] * 1000 - Dig[9] * 100)/10;
        //Create the string and send to the display
        ssd1306_WriteStringEightBitFont(0,  LINE3_Y,"SS:", Font_6x10, White);
        sprintf(strValue, "%u.%u", Dig[0], Dig[1]);
        ssd1306_WriteStringEightBitFont(0,  LINE5_Y,strValue, Font_6x10, White);
        ssd1306_WriteStringEightBitFont(31,  LINE3_Y,"SW:", Font_6x10, White);
        sprintf(strValue, "%u%u.%u", Dig[2], Dig[3], Dig[4]);
        ssd1306_WriteStringEightBitFont(31,  LINE5_Y,strValue, Font_6x10, White);
        ssd1306_WriteStringEightBitFont(68,  LINE3_Y,"SSS:", Font_6x10, White);
        sprintf(strValue, "%u.%u%u", Dig[5], Dig[6], Dig[7]);
        ssd1306_WriteStringEightBitFont(68,  LINE5_Y,strValue, Font_6x10, White);
        ssd1306_WriteStringEightBitFont(105,  LINE3_Y,"OIW:", Font_6x10, White);
        sprintf(strValue, "%u%u.%u", Dig[8], Dig[9], Dig[10]);
        ssd1306_WriteStringEightBitFont(105,  LINE5_Y,strValue, Font_6x10, White);
        //Write the key options to the display
        USR_WriteKeys("< > $ & * OK");
        //Write the intrument name to the display
        USR_WriteInstrumentName();
        break;
      }
      case 43:
      case 40: //Scrape big steps endless
      {
        gCurrentScreen = 40;
        USR_ClearScreen(3);
        ssd1306_WriteStringEightBitFont(41,16,"WHOLE REED", Font_6x10, White);
        ssd1306_WriteStringEightBitFont(41,28,"BIG STEPS", Font_6x10, White);
        ssd1306_WriteStringEightBitFont(41,40,"ENDLESS", Font_6x10, White);
        ssd1306_WriteStringEightBitFont(0,0,"Scrape", Font_6x10, White);
        USR_WriteKeys("< > $ & * OK");
        USR_WriteInstrumentName();
        break;
      }
      case 41: //Scrape small steps endless
      {
        USR_ClearScreen(3);
        ssd1306_WriteStringEightBitFont(41,16,"WHOLE REED", Font_6x10, White);
        ssd1306_WriteStringEightBitFont(41,28,"SMALL STEPS", Font_6x10, White);
        ssd1306_WriteStringEightBitFont(41,40,"ENDLESS", Font_6x10, White);
        ssd1306_WriteStringEightBitFont(0,0,"Scrape", Font_6x10, White);
        USR_WriteKeys("< > $ & * OK");
        USR_WriteInstrumentName();
        break;
      }
      case 39:
      case 42: //ScrapeNoIndex
      {
        gCurrentScreen = 42;
        USR_ClearScreen(3);
        ssd1306_WriteStringEightBitFont(41,16,"NO SIDE", Font_6x10, White);
        ssd1306_WriteStringEightBitFont(41,28,"STEPS", Font_6x10, White);
        ssd1306_WriteStringEightBitFont(41,40,"ENDLESS", Font_6x10, White);
        ssd1306_WriteStringEightBitFont(0,0,"Scrape", Font_6x10, White);
        USR_WriteKeys("< > $ & * OK");
        USR_WriteInstrumentName();
        break;
      }
      //User main menu
      case 101:
      case 102:
      case 103:
      case 104:
      {
        TopPage = gCurrentScreen - (gCurrentScreen+1)%3;   //101 -> 101 - 0 = 101, 103 -> 103 - 2 = 101, 105 -> 105 -1 = 104
        USR_ClearScreen(3);
        if (gServiceMenu)
        {
            ssd1306_WriteStringEightBitFont(0,0,"Settings - RM", Font_6x10, White);  
        }
        else        
        {
            ssd1306_WriteStringEightBitFont(0,0,"Settings", Font_6x10, White);  
        }
        for (uint8_t i = 0;i<3;i++)
        {
          if (i + TopPage - 101 < gMainMenuMaxService + 1)
          {
            if ((gServiceMenu) || (MainMenu[i + TopPage - 101].UserAccess))
            {
              sprintf(strValue, "%s", MainMenu[i + TopPage - 101].Name);
              ssd1306_WriteStringEightBitFont(Font_6x10.FontWidth * 2, LINE1_Y + ((LINE3_Y-LINE1_Y) * i),strValue, Font_6x10, White); 
            }
            else
              USR_ClearScreen(7);
          }
          else
          {
            USR_ClearScreen(7);
          }
        }
        USR_WriteKeys("& $ > *");
        if (gServiceMenu)
        {
          sprintf(strValue, "%u-%01u", ((TopPage-101)/3)+1,(gMainMenuMaxService+3)/3);
          ssd1306_WriteStringEightBitFont(127 - ssd1306_GetStringWidth(strValue,Font_6x10), BOTTOMLINE_Y + 3,strValue, Font_6x10, White);
        }
        else
        {
          sprintf(strValue, "%u-%01u", ((TopPage-101)/3)+1,(gMainMenuMaxUser+3)/3);
          ssd1306_WriteStringEightBitFont(127 - ssd1306_GetStringWidth(strValue,Font_6x10), BOTTOMLINE_Y + 3,strValue, Font_6x10, White);
        }

        ssd1306_WriteStringEightBitFont(0, LINE1_Y + (gCurrentScreen - TopPage) * (LINE3_Y-LINE1_Y),">", Font_6x10,White);
        break;
      }
      case 10101:
      case 10102:
      case 10103:
      case 10104:
      case 10105:
      case 10106:
      case 10107:
      case 10108:
      case 10109:
      case 10110:
      case 10111:
      case 10112:
      case 10113:
      case 10114:
      case 10115:
      case 10116:
      case 10117:
      case 10118:
      {
        //Find the top of the page
        TopPage = gCurrentScreen - gCurrentScreen%3;   //10102 -> 10102 - 1 = 10101, 10104 -> 10104 - 0 = 10104

        if (gServiceMenu)
          ssd1306_WriteStringEightBitFont(0, 0,"SERVICE          ", Font_6x10, White);  
        else
          ssd1306_WriteStringEightBitFont(0, 0,"                 ", Font_6x10, White);
        for (uint8_t i = 0;i<3;i++)
        {
  
          if (i + TopPage - 10101 < gParameterMaxService + 1)
          {
            if ((gServiceMenu) || (gMachineType[gMachine/100].Parameters[i + TopPage - 10101].UserAccess))
            {
              sprintf(strValue, "  %s", gMachineType[gMachine/100].Parameters[i + TopPage - 10101].Name);
              ssd1306_WriteStringEightBitFont(0, 16 + (12 * i),strValue, Font_6x10, White); 
            }
            else
              ssd1306_WriteStringEightBitFont(0, 16 + (12 * i),"                    ", Font_6x10, White);
          }
          else
              ssd1306_WriteStringEightBitFont(0, 16 + (12 * i),"                    ", Font_6x10, White);
        }
        USR_WriteKeys("& $ > *");

        if (gServiceMenu)
        {
          sprintf(strValue, "    %u-%01u", ((TopPage-10101)/3)+1,(gParameterMaxService+3)/3);
          ssd1306_WriteStringEightBitFont(80, BOTTOMLINE_Y + 3,strValue, Font_6x10, White);
        }
        else
        {
          sprintf(strValue, "    %u-%01u", ((TopPage-10101)/3)+1,(gParameterMaxUser+3)/3);
          ssd1306_WriteStringEightBitFont(80, BOTTOMLINE_Y + 3,strValue, Font_6x10, White);
        }

        ssd1306_WriteStringEightBitFont(0, 16 + (gCurrentScreen - TopPage) * 12,">", Font_6x10,White);
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

        if (gServiceMenu)
          ssd1306_WriteStringEightBitFont(0, 0,"SERVICE             ", Font_6x10, White);  
        else
          ssd1306_WriteStringEightBitFont(0, 0,"                    ", Font_6x10, White);
        for (uint8_t i = 0;i<3;i++)
        {
  
          if (i + TopPage - 10201 < gCommandMaxService + 1)
          {
            if ((gServiceMenu) || (gCommands[i + TopPage - 10201].UserAccess))
            {
              sprintf(strValue, "  %s", gCommands[i + TopPage - 10201].Name);
              ssd1306_WriteStringEightBitFont(0, 16 + (12 * i),strValue, Font_6x10, White); 
            }
            else
              ssd1306_WriteStringEightBitFont(0, 16 + (12 * i),"                    ", Font_6x10, White);
          }
          else
              ssd1306_WriteStringEightBitFont(0, 16 + (12 * i),"                    ", Font_6x10, White);
        }
        USR_WriteKeys("& $ > *");

        if (gServiceMenu)
        {
          sprintf(strValue, "    %u-%01u", ((TopPage-10201)/3)+1,(gCommandMaxService+3)/3);
          ssd1306_WriteStringEightBitFont(80, BOTTOMLINE_Y + 3,strValue, Font_6x10, White);
        }
        else
        {
          sprintf(strValue, "    %u-%01u", ((TopPage-10201)/3)+1,(gCommandMaxUser+3)/3);
          ssd1306_WriteStringEightBitFont(80, BOTTOMLINE_Y + 3,strValue, Font_6x10, White);
        }

        ssd1306_WriteStringEightBitFont(0, 16 + (gCurrentScreen - TopPage) * 12,">", Font_6x10,White);
        break;
      }
      case 1020101: //Factory reset.
      {

        ssd1306_WriteStringEightBitFont(0, 0,"FACTORY RESET", Font_6x10, White);

        ssd1306_WriteStringEightBitFont(0, 16,"THIS WILL ERASE ALL  ", Font_6x10, White);

        ssd1306_WriteStringEightBitFont(0, 28,"CHANGES! PRESS OK TO ", Font_6x10, White);

        ssd1306_WriteStringEightBitFont(0, 40,"RESET OR * TO CANCEL ", Font_6x10, White);
        USR_WriteKeys("* OK");
        break;
      }
      case 1020201: //Set stroke length: Press OK for goto HOME position
      {

        ssd1306_WriteStringEightBitFont(0, 0,"STROKE LENGTH", Font_6x10, White);

        ssd1306_WriteStringEightBitFont(0, 16,"PRESS OK TO GO TO THE", Font_6x10, White);

        ssd1306_WriteStringEightBitFont(0, 28,"STROKE LENGTH ADJUST ", Font_6x10, White);

        ssd1306_WriteStringEightBitFont(0, 40,"POSITION.            ", Font_6x10, White);
        USR_WriteKeys("* OK");
        break;
      }
      case 1020202: //Set stroke length: PLEASE WAIT
      case 1020204: 
      {

        ssd1306_WriteStringEightBitFont(0, 16,"                     ", Font_6x10, White);

        ssd1306_WriteStringEightBitFont(0, 28,"     PLEASE WAIT     ", Font_6x10, White);

        ssd1306_WriteStringEightBitFont(0, 40,"                     ", Font_6x10, White);
        USR_WriteKeys("");
        break;
      }
      case 1020203: //Set stroke length
      {

        ssd1306_WriteStringEightBitFont(0, 16,"AFTER ADJUST PRESS:  ", Font_6x10, White);

        ssd1306_WriteStringEightBitFont(0, 28,"OK: START POSITION   ", Font_6x10, White);

        ssd1306_WriteStringEightBitFont(0, 40," *: CANCEL           ", Font_6x10, White);
        USR_WriteKeys("* OK");
        break;
      }

      case 1020301: //Stroke motor
      {

        ssd1306_WriteStringEightBitFont(0, 0,"STROKE MOTOR ", Font_6x10, White);

        ssd1306_WriteStringEightBitFont(0, 16,"      $ FASTER       ", Font_6x10, White);

        ssd1306_WriteStringEightBitFont(0, 28,"# ON            , OFF", Font_6x10, White);

        ssd1306_WriteStringEightBitFont(0, 40,"      & SLOWER       ", Font_6x10, White);
        USR_WriteKeys("< > $ & *");
        break;
      }
      case 10300:
      case 10301:
      case 10302:
      case 10303:  
      case 10304:
      {
        USR_ClearScreen(3);
        if (gServiceMenu)
          ssd1306_WriteStringEightBitFont(0, 0,"Errors - RM", Font_6x10, White);  
        else
          ssd1306_WriteStringEightBitFont(0, 0,"Errors", Font_6x10, White);
        uint8_t LastError = Errors[0] - 1001;
        uint8_t LastPage = (LastError+2) / 3;
        if (Errors[99] > 0) LastPage = 33;
        
        if (gCurrentScreen == 10304) //Down button pushed, display next 3 errors
        {
          gCurrentScreen = 10301;
          ErrorDisplayPage ++;
          if (ErrorDisplayPage > LastPage - 1) ErrorDisplayPage = 0;
        }
        if (gCurrentScreen == 10300)
        {
          gCurrentScreen = 10303;
          ErrorDisplayPage --;
          if (ErrorDisplayPage < 0) 
          {
            ErrorDisplayPage = LastPage - 1;
            gCurrentScreen = 10300 + LastError - (ErrorDisplayPage*3);
          }
        }
        if ((LastError == 0)&&(Errors[99] == 0))
        {
  
          ssd1306_WriteStringEightBitFont(0, 16,"                     ", Font_6x10, White);
  
          ssd1306_WriteStringEightBitFont(0, 28,"     No errors      ", Font_6x10, White);
  
          ssd1306_WriteStringEightBitFont(0, 40,"                     ", Font_6x10, White);
          USR_WriteKeys("*");
        }
        else if (LastError == 0)
        {
          LastError = 99;
        }
        else
        {
          gCurrentError = LastError - (gCurrentScreen - 10301) - (ErrorDisplayPage*3);
          if (gCurrentError <= 0) 
          {
            gCurrentError = LastError;
            ErrorDisplayPage = 0;
            gCurrentScreen = 10301;
          }
          for (uint8_t i = 0;i<3;i++)
          {
            if (gCurrentError + (gCurrentScreen - 10301) - i > 0)
            {
                //if (gCurrentError - i + (gCurrentScreen - 10301) <= 0)
                //  sprintf(strValue, "Error %02u: %u", LastError + 1 -(gCurrentError + (gCurrentScreen - 10301) - i + 99), Errors[gCurrentError + (gCurrentScreen - 10301) - i + 99]);
                //else
                  sprintf(strValue, "Error %02u: %u", LastError + 1 - (gCurrentError + (gCurrentScreen - 10301) - i), Errors[gCurrentError + (gCurrentScreen - 10301) - i]);
                ssd1306_WriteStringEightBitFont(Font_6x10.FontWidth * 2, LINE1_Y + ((LINE3_Y-LINE1_Y) * i),strValue, Font_6x10, White); 
            }    
            else
            {
                if (i == 1) USR_ClearScreen(7);
                if (i == 2) USR_ClearScreen(9);
            }
          }
          USR_WriteKeys("< & $ >");
  
          sprintf(strValue, "%u-%u", ErrorDisplayPage+1, LastPage);
          ssd1306_WriteStringEightBitFont(128 - ssd1306_GetStringWidth(strValue,Font_6x10), BOTTOMLINE_Y + 3,strValue, Font_6x10, White);
          ssd1306_WriteStringEightBitFont(0, LINE1_Y + (gCurrentScreen - 10301) * (LINE3_Y-LINE1_Y),">", Font_6x10,White);
        }
        break;
      }
      case 10400:
      case 10401:
      case 10402:
      {
        gCurrentScreen = 10401;
        if (gServiceMenu)
          ssd1306_WriteStringEightBitFont(0, 0,"SERVICE          ", Font_6x10, White);  
        else
          ssd1306_WriteStringEightBitFont(0, 0,"                 ", Font_6x10, White);
        sprintf(strValue, "MASTER CNT:  %u", gCounter.MasterCounter);
    ssd1306_WriteStringEightBitFont(0, 16,strValue, Font_6x10, White);
        sprintf(strValue, "SERVICE CNT: %u", gCounter.ServiceCounter);
    ssd1306_WriteStringEightBitFont(0, 28,strValue, Font_6x10, White);
        ssd1306_WriteStringEightBitFont(0, 40,"OK: RESET SERVICE CNT", Font_6x10, White);
        USR_WriteKeys("* OK");
        break;
      }
      case 1040101:
      {
        ssd1306_WriteStringEightBitFont(0, 0,"RESET COUNTER", Font_6x10, White);

        ssd1306_WriteStringEightBitFont(0, 16,"THIS WILL RESET THE  ", Font_6x10, White);

        ssd1306_WriteStringEightBitFont(0, 28,"COUNTERS! PRESS OK TO", Font_6x10, White);

        ssd1306_WriteStringEightBitFont(0, 40,"RESET OR * TO CANCEL ", Font_6x10, White);
        USR_WriteKeys("# OK");
        break;
      }
    default:
    {
    
        ssd1306_WriteStringEightBitFont(0, 16,"                     ", Font_6x10, White);

        ssd1306_WriteStringEightBitFont(0, 28,"SCREEN MISSING       ", Font_6x10, White);

        ssd1306_WriteStringEightBitFont(0, 40,"                     ", Font_6x10, White);
        USR_WriteKeys("");
    }
    }
    if ((gCurrentScreen < 40) && (gCurrentScreen >= 29)) gLastScrapeScreen = gCurrentScreen;
    if ((gCurrentScreen < 50) && (gCurrentScreen >= 39)) gLastScrapeScreenEndless = gCurrentScreen;
    ssd1306_UpdateScreen();
}
//-----------------------------------------------------------------------------
//! \brief      Displays the instrument
//! \details    Displays instrument name at right bottom
//! \params     None
void USR_WriteInstrumentName (void)
{
  if (SHOWINSTRUMENTNAME) 
    ssd1306_WriteStringEightBitFont(128 - ssd1306_GetStringWidth(gMachineType[gMachine/100].Name, Font_6x10), BOTTOMLINE_Y + 3,gMachineType[gMachine/100].Name, Font_6x10, White);

}

//-----------------------------------------------------------------------------
//! \brief      Displays possible keys left bottom
//! \details    Displays keys that can be used by the user
//! \param[in]  char* newKeys
void USR_WriteKeys (char* newKeys)
{
  USR_ClearScreen(4);//ssd1306_WriteStringEightBitFont(0,BOTTOMLINE_Y + 3,"          ", Font_6x10, White);
  ssd1306_WriteStringEightBitFont(0,BOTTOMLINE_Y + 3,newKeys, Font_6x10, White);  
}
//-----------------------------------------------------------------------------


