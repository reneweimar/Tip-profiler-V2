//-----------------------------------------------------------------------------
//! \file       work.c
//! \author     R. Weimar
//! \brief      Contains routines for the general sequence
//! \details    
//! \Attention  
//-----------------------------------------------------------------------------
#include "main.h"
#include "work.h"
#include "string.h"
#include "eeprom.h"
#include "userinterface.h"
#include "power.h"
#include "adc.h"
#include "strokemotor.h"
#include "indexmotor.h"
#include "i2c.h"
#include "ssd1306.h"
#include <stdlib.h>
//-----------------------------------------------------------------------------
//! \Global ADC result
uint16_t ADC_Converted_Values[1];
//! \Global main and sub, current and previous status of the device
stcStatus gStatus;
//! \Global counter structure
stcCounter gCounter;
//! \Global Initialization flag
uint8_t gInitialized = 0;
//! \Global temp battery level (Replace by ADC)
uint8_t BattPercentage = 50;
//! \Global machine type 0 (oboe), type 1 (Bassoon)
uint16_t gMachine;
//! \Global parameters container defaults for machine type 0 (oboe)
StcMachine gMachineType[NROFMACHINETYPES];
//! \Global commands container
StcCommands gCommands[] =
{
  {"RESET TO FACTORY  ",1,2,1,1,1,1,1},
  {"STROKE MOTOR      ",0,1,1,3,5,6,4}  
};
//! \Global User command max flag
uint8_t gCommandMaxUser;
//! \Global Service command max flag
uint16_t gCommandMaxService = sizeof(gCommands)/sizeof(gCommands[0])-1;
//! \Global parameters container
StcParameters DefaultsMachine0[20] = 
{ 
  {"SCRAPE WIDTH      ",0,1160,780,"MM",3,1,1,0},
  {"SCRAPE WIDTH INNER",0,1160,390,"MM",3,1,1,0},
  {"SCRAPE SPEED      ",0,200,200,"RPS",1,1,1,0},
  {"SIDE STEP SIZE    ",5,99,30,"MM",2,2,1,0},
  {"SCREEN SAVER      ",0,100,0,"-",1,0,0,1},
  {"MACHINE TYPE      ",0,100,0,"-",1,0,0,1},
  {"SIDE STEP OFFSET  ",-100,100,0,"MM",3,2,0,0},
  {"SCRAPE SPEED MAX  ",10,200,200,"RPS",2,1,0,0},
  {"SCRAPE SPEED MIN  ",10,100,10,"RPS",2,1,0,0},
  {"SCRAPE SPEED MAX  ",10,200,200,"RPS",2,1,0,0}  
};
StcParameters DefaultsMachine1[20] = 
{ 
  {"SCRAPE WIDTH      ",0,2400,1660,"MM",3,1,1,0},
  {"SCRAPE WIDTH INNER",0,2400,830,"MM",3,1,1,0},
  {"SCRAPE SPEED      ",0,200,200,"RPS",1,1,1,0},
  {"SIDE STEP SIZE    ",5,99,40,"MM",2,2,1,0},
  {"SCREEN SAVER      ",0,100,0,"-",1,0,0,1},
  {"MACHINE TYPE      ",0,100,0,"-",1,0,0,1},
  {"SIDE STEP OFFSET  ",-100,100,0,"MM",3,2,0,0},
  {"SCRAPE SPEED MAX  ",10,200,200,"RPS",2,1,0,0},
  {"SCRAPE SPEED MIN  ",10,100,10,"RPS",2,1,0,0},
  {"SCRAPE SPEED MAX  ",10,200,200,"RPS",2,1,0,0}      
};
//! \Global Service flag
uint8_t gServiceMenu;
//! \Global User parameter max flag
uint8_t gParameterMaxUser;
//! \Global Service parameter max flag
uint16_t gParameterMaxService = 9; //9 is last index
//! \Global virtual tab for eeprom simulation
uint16_t VirtAddVarTab[NB_OF_VAR] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119};

uint16_t ParameterTemp[20];

uint32_t HomeCnt,HomeCntDelay;


float BatteryVoltage;
//-----------------------------------------------------------------------------
//! \brief      Initiates the work unit
//! \details    Defines the parameters
//! \param      None
void WRK_ResetFactory(void)
{
  if (gMachine == 0)
    memcpy( &gMachineType[0].Parameters, &DefaultsMachine0, sizeof DefaultsMachine0);
  else
    memcpy( &gMachineType[1].Parameters, &DefaultsMachine1, sizeof DefaultsMachine1);
  
  for (uint8_t i=0; i<gParameterMaxService ;i++) //Check eeprom memory
  {
    EE_WriteVariable(i+gMachine, gMachineType[gMachine/100].Parameters[i].Value);
  }
}

//-----------------------------------------------------------------------------
//! \brief      Initiates the work unit
//! \details    Defines the parameters
//! \param      None
void WRK_Init(void)
{
  memcpy( &gMachineType[0].Parameters, &DefaultsMachine0, sizeof DefaultsMachine0);
  memcpy( &gMachineType[1].Parameters, &DefaultsMachine1, sizeof DefaultsMachine1);
  strcpy(gMachineType[0].Name, "    OBOE");
  strcpy(gMachineType[1].Name, " BASSOON");
    
  EE_ReadVariable(5, &gMachine);
  
  uint8_t NotEmpty = 0;

  //uint16_t ParameterTemp[gParameterMaxService];
  
  for (uint8_t i=0; i<gParameterMaxService ;i++) //Check eeprom memory
  {
    EE_ReadVariable(i + gMachine, &ParameterTemp[i]);
    if (ParameterTemp[i]>0) NotEmpty = 1;
  }
  if (NotEmpty == 1) //Copy new values over gParameter
  {
    for (uint8_t i=0; i<gParameterMaxService ;i++) //Check eeprom memory
    {
      gMachineType[gMachine/100].Parameters[i].Value = ParameterTemp[i];
    }
  }
  else //Store default values from gParameter
  {
    for (uint8_t i=0; i<gParameterMaxService ;i++) //Check eeprom memory
    {
      EE_WriteVariable(i+gMachine, gMachineType[gMachine/100].Parameters[i].Value);
    }
  }
  for (uint8_t i=1; i<gParameterMaxService ;i++) //Always assume at least 1 User parameter
  {
    if (gMachineType[gMachine/100].Parameters[i].UserAccess == 1)
    {
      gParameterMaxUser = i;
    }
  }
  for (uint8_t i=1; i<gCommandMaxService ;i++) //Always assume at least 1 User parameter
  {
    if (gCommands[i].UserAccess == 1)
    {
      gCommandMaxUser = i;
    }
  }

}

//-----------------------------------------------------------------------------
//! \brief      Sets the global status
//! \details    Sets the selected status (main or sub) and stores the 
//! previous status
//! \param[in]  newType   MainStatus or SubStatus
//! \param[in]  newStatus New status for the selected status
void WRK_SetStatus (enuType newType, enuStatus newStatus)
{
    if (newType == MainStatus)
    {
        gStatus.MainStatusOld = gStatus.MainStatus;
        gStatus.MainStatus = newStatus;
        gStatus.SubStatusOld = gStatus.SubStatus;
        gStatus.SubStatus = UNDEFINED;
    }
    else
    {
        gStatus.SubStatusOld = gStatus.SubStatus;
        gStatus.SubStatus = newStatus;
    }
}

//-----------------------------------------------------------------------------
//! \brief      Handles the battery status
//! \details    Handles 1 ms time interrupt 
//! \param      None
void WRK_HandleBatteryStatus (void)
{
    static uint16_t TickTime = 0; 
    static uint8_t ConvertedValueOld;
    if (TickTime++ < 499) return; //4997ms 
    TickTime = 0;

    //if (gStatus.SubStatus == WAITFORUSER) 
    //{ 
      if (ConvertedValueOld != ADC_Converted_Values[0])
      {
        BatteryVoltage = (uint16_t) ((float) ADC_Converted_Values[0] / 4095 * 4210); //4210 = 3300 * (9100 + 33000) / 33000
        if (BatteryVoltage <= BATTVOLTAGEMIN)
          BattPercentage = 0;  
        else if (BatteryVoltage>= BATTVOLTAGEMAX)
          BattPercentage = 100;  
        else
          BattPercentage = (uint8_t) (((BatteryVoltage - BATTVOLTAGEMIN)/(BATTVOLTAGEMAX-BATTVOLTAGEMIN))* 100 );
        ConvertedValueOld = ADC_Converted_Values[0];
        USR_ShowBattery(BattPercentage);
      }
    //}
}
//-----------------------------------------------------------------------------
//! \brief      Handles the tick time counter
//! \details    Handles 1 ms time interrupt 
//! \param      None
void WRK_HandleTickTime (void)
{
    if (gInitialized)
    {
      gCounter.Delay ++;
      WRK_HandleSequence();
      WRK_HandleBatteryStatus();
      USR_HandleButtons();
      STR_HandleEncoder();
      //IDX_HandleEncoder();
      IDX_HandleMotor();
    }
}
//-----------------------------------------------------------------------------
//! \brief      Handles the command
//! \details    Handles actions to form th emain sequence 
//! \param[in]  uint16_t newCommand
//! \param[in]  uint16_t LastScreen
void WRK_HandleCommand(uint8_t newCommand, uint16_t LastScreen)
{
  switch (newCommand)
  {
    case 3: //Stroke motor on
    {
      break;
    }
    case 4: //Stroke motor off
    {
      break;
    }
    case 5: //Stroke motor faster
    {
      break;
    }
    case 6: //Stroke motor slower
    {
      break;
    }
    case 2: //Reset factory
    {
      WRK_ResetFactory();
    }  //no break. Needs to go to case 1 too
    case 1: //Cancel, go back
    {
      USR_ShowScreen (LastScreen); 
      WRK_SetStatus(MainStatus,ACTIVE);
      WRK_SetStatus(SubStatus,WAITFORUSER);
      break;
    }
    default:
      break;
  }
}  

//-----------------------------------------------------------------------------
//! \brief      Handles the sequence
//! \details    Handles actions to form the emain sequence 
//! \param      None
void WRK_HandleSequence(void)
{
  static uint8_t TickTime = 0; 
  static uint16_t LastScreen = 0;
  static uint16_t LastNormalScreen = 0;
  static uint8_t CntStep;
  uint16_t TmpScreen;
  
  if (TickTime++ < 99) return; //100ms 
  TickTime = 0;

  gCounter.Sequence += 100;
  if (HomeCntDelay) HomeCntDelay -= 100;
  
  if (PluggedIn())
  {
    gCounter.User = 0;
    ssd1306_SetContrast(HIGHCONTRAST);
  }
  else 
  {
    if (gCounter.User < 0xffffffff) gCounter.User += 100;
    if ((gCounter.User > LOWPOWERTIME)&&(SCREENSAVERON)) 
    {
      ssd1306_SetContrast(LOWCONTRAST);
    }
  }
  
    
  switch (gStatus.MainStatus)
  {
    case UNDEFINED:
    {
      WRK_SetStatus(MainStatus,INACTIVE);
      break;
    }
    case INACTIVE:
    {
      gCounter.Sequence = 0;
      USR_ShowScreen(1);
      WRK_SetStatus(MainStatus,ACTIVE);
      break;
    }
    case EXECUTECOMMAND:
    {
      switch (gStatus.SubStatus)
      {
        case UNDEFINED:
        {      
          USR_ShowMessage(gCurrentScreen);
          WRK_SetStatus(SubStatus,WAITFORUSER);
          break;
        }
        case WAITFORUSER:
        {
          if (USR_ButtonPressed(BtnOk,USR_SHORTPRESSTIME,1)==1) //Execute command
            WRK_HandleCommand(gCommands[gCurrentScreen-10201].BtnOK_Command, gCurrentScreen);
          else if (USR_ButtonPressed(BtnMenu,USR_SHORTPRESSTIME,1)==1)
            WRK_HandleCommand(gCommands[gCurrentScreen-10201].BtnMenu_Command, gCurrentScreen);
          else if (USR_ButtonPressed(BtnLeft, USR_SHORTPRESSTIME,1)==1)
            WRK_HandleCommand(gCommands[gCurrentScreen-10201].BtnLeft_Command, gCurrentScreen);
          else if (USR_ButtonPressed(BtnRight, USR_SHORTPRESSTIME,1)==1)
            WRK_HandleCommand(gCommands[gCurrentScreen-10201].BtnRight_Command, gCurrentScreen);
          else if (USR_ButtonPressed(BtnUp,USR_SHORTPRESSTIME,1)==1)
            WRK_HandleCommand(gCommands[gCurrentScreen-10201].BtnUp_Command, gCurrentScreen);
          else if (USR_ButtonPressed(BtnDown, USR_SHORTPRESSTIME,1)==1)
            WRK_HandleCommand(gCommands[gCurrentScreen-10201].BtnDown_Command, gCurrentScreen);
          break;
        }
        default:
          break;
      }
      break;
    }
    case ENTERVALUE:
    {
      switch (gStatus.SubStatus)
      {
        case UNDEFINED:
        {      
          USR_EnterValue(gCurrentScreen - 10101);
          WRK_SetStatus(SubStatus,WAITFORUSER);
          break;
        }
        case WAITFORUSER:
        {
          if (USR_ButtonPressed(BtnOk,10,1)==1) //Save and return to menu
          {
            USR_SaveParameter();
            USR_ShowScreen (LastScreen); 
            WRK_SetStatus(MainStatus,ACTIVE);
            WRK_SetStatus(SubStatus,WAITFORUSER);
          }
          else if (USR_ButtonPressed(BtnMenu,10,1)==1) //Cancel, don't save
          {
            USR_ShowScreen (LastScreen); 
            WRK_SetStatus(MainStatus,ACTIVE);
            WRK_SetStatus(SubStatus,WAITFORUSER);
          }
          else if (USR_ButtonPressed(BtnLeft, USR_SHORTPRESSTIME,1)==1) //Move cursor left if allowed
          {
            USR_CursorLeft();
          }
          else if (USR_ButtonPressed(BtnRight, USR_SHORTPRESSTIME,1)==1) //Move cursor left if allowed
          {
            USR_CursorRight();
          }
          else if (USR_ButtonPressed(BtnUp,USR_SHORTPRESSTIME,0)==1) //Increase the value if allowed
          {
            USR_CursorUp();
          }
          else if (USR_ButtonPressed(BtnDown, USR_SHORTPRESSTIME,0)==1) //Decrease the value if allowed
          {
            USR_CursorDown();
          }
          break;
        }
        default:
          break;
      }
      break;
    }
    case ACTIVE:
    {
      switch (gStatus.SubStatus)
      {
        case ENCODERTEST:
        {
          switch (CntStep)
          {
            case 0:  
            case 1:
            { 
              gIDX_Motor.MainStatus = ACTIVE;
              gIDX_Motor.SetPosition = 5000;
              CntStep = 2;
              break;
            }
            case 2:
            {
              if (abs(gIDX_Motor.GetPosition - 5000) < 3)
              {
                gIDX_Motor.MainStatus = ACTIVE;
                gIDX_Motor.SetPosition = 0;  
                CntStep = 3;
              }
              break;
            }
            case 3:
            {
              if (abs(gIDX_Motor.GetPosition) < 3)
              {
                gIDX_Motor.MainStatus = ACTIVE;
                gIDX_Motor.SetPosition = 5000;  
                CntStep = 2;
              }
              break;
            }
            default:
              break;
          }
          
          break;
        }
        case UNDEFINED:
        {
            WRK_SetStatus(SubStatus,WAITFORSPLASHSCREEN);
            break;
        }
        case WAITFORSPLASHSCREEN:
        {
          if (gCounter.Sequence > SPLASHSCREENTIME)
          {
            USR_ClearScreen(0);
            USR_ClearScreen(2);
            USR_ShowScreen(10); //HOME
            gCounter.Sequence = 0;
            WRK_SetStatus(SubStatus,WAITFORUSER);
          }
          break;
        }
        case WAITFORINDEXHOME:
        case WAITFORINDEXSTART:
        {
          if (IDX_Set(HOME)== READY)
          {
            IDX_Set(UNDEFINED);
            if (gStatus.SubStatus == WAITFORINDEXSTART)
            {
              WRK_SetStatus(SubStatus,WAITFORUSER);
            }
            else
            {
              WRK_SetStatus(SubStatus,WAITFORSTROKEMOTORHOME);
            }
          }
          break;
        }
        case WAITFORSTROKEMOTORHOME:
        {
          if (STR_Set(HOME)== READY)
          {
            STR_Set(UNDEFINED);
            WRK_SetStatus(SubStatus,WAITFORUSER);
          }
          break;
        }
        case WAITFORSTROKEMOTORSTART:
        {
          if (STR_Set(GOTOSTARTPOSITION)== READY)
          {
            STR_Set(UNDEFINED);
            WRK_SetStatus(SubStatus,WAITFORINDEXSTART);
          }
          break;
        }
        case WAITFORUSER:
        {
          USR_ButtonPressed(BtnOk,1,1); //Get the waitforreleased flag
            
          if (USR_ButtonWaitForRelease(BtnOk)==1)
          {
            if ((gCurrentScreen >= 100)&&(gCurrentScreen < 10200))
            {
              if (USR_ButtonPressed(BtnMenu,5000,1) == 1)
              { 
                gServiceMenu = 1 - gServiceMenu;
                if (gServiceMenu)
                  USR_ShowScreen (101);
                else
                  USR_ShowScreen (10101);
              }
            }
            else if (gCurrentScreen == 10) //Find HOME screen
            {
              WRK_SetStatus(SubStatus,WAITFORINDEXHOME);  
            }
            else if (gCurrentScreen == 20) //Goto START screen
            {
              WRK_SetStatus(SubStatus,WAITFORSTROKEMOTORSTART);  
            }
          }
          else if (USR_ButtonPressed(BtnRight,1,1) == 1)
          {
            if (gCurrentScreen < 100) //Normal screen
            {
              TmpScreen = gCurrentScreen / 10;
              USR_ShowScreen ((TmpScreen * 10) + 10);
              
            }
            else if (gCurrentScreen < 10000) //Menu screen
            {
              USR_ShowScreen ((gCurrentScreen  * 100) + 1);
            } 
            else if ((gCurrentScreen > 10100) && (gCurrentScreen < 10200)) //Parameter screen
            {
              LastScreen = gCurrentScreen;
              WRK_SetStatus(MainStatus,ENTERVALUE);
            }
            else 
            {
              WRK_SetStatus(MainStatus,EXECUTECOMMAND);
              break;
            }
          }
          else if (USR_ButtonPressed(BtnLeft, USR_SHORTPRESSTIME,1) == 1)
          {
            if (gCurrentScreen < 100) //Normal screen
            {
              TmpScreen = gCurrentScreen / 10;
              USR_ShowScreen ((TmpScreen * 10) - 10);
            }
          }
          else if (USR_ButtonPressed(BtnDown, USR_SHORTPRESSTIME,1) == 1)
          {
            USR_ShowScreen (gCurrentScreen + 1);
          }
          else if (USR_ButtonPressed(BtnUp,USR_SHORTPRESSTIME,1) == 1)
          {
            USR_ShowScreen (gCurrentScreen - 1);
          }
          else if (USR_ButtonPressed(BtnMenu,USR_SHORTPRESSTIME,1) == 1)
          {
            USR_ClearScreen(2);
            if (gCurrentScreen < 100) //Normal screen
            {
              LastNormalScreen = gCurrentScreen;
              USR_ShowScreen (101);
            }
            else if ((gCurrentScreen >= 10000))
            {
              USR_ShowScreen (gCurrentScreen  / 100);
            } 
            else
            {
              USR_ShowScreen (LastNormalScreen);
            }
          }
          break;
        }
        default:
          break;
      }
      break;
    }
    default:
        break;
  }
}

//-----------------------------------------------------------------------------

