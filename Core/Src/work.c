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
#include "stdio.h"
#include <stdlib.h>
//-----------------------------------------------------------------------------
//! \Side step size
uint16_t WRK_SideStep;
//! \Global ADC result
uint16_t ADC_Converted_Values[1];
//! \Global scrape control structure
stcScrape gScrape;
//! \Global main and sub, current and previous status of the device
stcStatus gWRK_Status;
//! \Global counter structure
stcCounter gCounter;
//! \Global Initialization flag
uint8_t gInitialized = 0;
//! \Global temp battery level (Replace by ADC)
uint8_t BattPercentage = 50;
//! \Global machine type 0 (oboe), type 1 (Bassoon)
uint16_t gMachine;
//! \Global parameter names enumeration
enuParameters gParameters;
//! \Storage of last screen number
uint16_t LastScreen = 0;
uint16_t LastNormalScreen = 0;
//! Global Flag battery empty
uint8_t StoppedBecauseBatteryEmpty;
//! \Global Contrast handler
uint8_t WRK_UpdateContrast;
//! \Scrape percentage left done
uint8_t LeftPercentage;
//! \Scrape percentage left done
uint8_t RightPercentage;
//! \Global Main Menu container

StcMainMenu MainMenu[NROFMAINMENUITEMS]=
{
  {"Values", 1},
  {"Commands", 1},  
  {"Errors", 1},
  {"Counters", 0},
  {"Data",0}
};
//! \Global service max main menu service flag
uint8_t gMainMenuMaxService = 3;
//! \Global service max main menu user flag
uint8_t gMainMenuMaxUser = 2;
//! \Global Command to repeat
uint8_t gRepeatCommand;
//! \Global error list
uint16_t Errors[NROFERRORS];
//! \Global error display number
int8_t ErrorDisplayPage = 0;
//! \Global parameters container defaults for machine type 0 (oboe)
StcMachine gMachineType[NROFMACHINETYPES];
//! \Global commands container
StcCommands gCommands[] =
{
  {"Set stroke length",1},
  {"Reset to factory settings",1},
//  {"STROKE MOTOR       ",0},
};
//! \Global User command max flag
uint8_t gCommandMaxUser;
//! \Global Service command max flag
uint16_t gCommandMaxService = sizeof(gCommands)/sizeof(gCommands[0])-1;
//! \Global parameters container
StcParameters DefaultsMachine[NROFMACHINETYPES][20]=
{ //OBOE
  { 
    {"Scrape Speed, SS",50,400,200,"/s",2,1,1,0,0,0,0,0,0,0},
    {"Scrape Width, SW",0,1160,780,"mm",3,1,1,0,0,0,0,0,0,0},
    {"Big Side Step, BSS",20,100,40,"mm",3,2,1,0,0,0,0,0,0,0},
    {"Small Side Step, SSS",5,95,20,"mm",3,2,1,0,0,0,0,0,0,0},
    {"Outer Inner Width, OIW",0,200,390,"mm",3,1,1,0,0,0,0,0,0,0},
    {"Screen saver",0,100,0,"-",1,0,0,1,2,0,100,0,0,0},
    {"Machine type",0,100,0,"-",1,0,0,1,2,0,100,0,0,0},
    {"Side step offset",-100,100,0,"mm",3,2,0,0,0,0,0,0,0,0},
    {"Scrape speed min",10,100,10,"/s",2,1,0,0,0,0,0,0,0,0},
    {"Scrape speed max",10,200,200,"/s",2,1,0,0,0,0,0,0,0,0},
    {"Scrape speed width max",0,2500,1160,"mm",3,1,0,0,0,0,0,0,0,0},
    {"Scrape width factory",0,2500,780,"mm",3,1,0,0,0,0,0,0,0,0},
    {"Scrape speed factory",0,200,200,"/s",1,1,0,0,0,0,0,0,0,0},
    {"Side step small factory",0,500,30,"mm",3,2,0,0,0,0,0,0,0,0},
    {"Side step big factory",0,500,30,"mm",3,2,0,0,0,0,0,0,0,0},
    {"Side step ratio",0,0,1756,"-",4,3,0,1,2,1756,1838,0,0,0},
    {"Side step reduction",0,0,15000,"-",3,0,0,1,2,3000,15000,0,0,0},
    {"Stroke reduction",0,0,3000,"-",2,0,0,1,2,3000,5000,0,0,0},
    {"",0,0,0,"",0,0,0,0,0,0,0,0,0,0},
    {"",0,0,0,"",0,0,0,0,0,0,0,0,0,0}
  },
  { //BASSOON  
    {"Scrape Speed, SS",50,400,200,"/s",1,1,1,0,0,0,0,0,0,0},
    {"Scrape Width, SW",0,2400,1660,"mm",3,1,1,0,0,0,0,0,0,0},
    {"Big Side Step, BSS",30,100,60,"mm",3,2,1,0,0,0,0,0,0,0},
    {"Small Side Step, SSS",5,60,30,"mm",3,2,1,0,0,0,0,0,0,0},
    {"Outer Inner Width, OIW",0,200,830,"mm",3,1,1,0,0,0,0,0,0,0},
    {"Screen saver",0,100,0,"-",1,0,0,1,2,0,100,0,0,0},
    {"Machine type",0,100,0,"-",1,0,0,1,2,0,100,0,0,0},
    {"Side step offset",-100,100,0,"mm",3,2,0,0,0,0,0,0,0,0},
    {"Scrape speed min",10,100,10,"/s",2,1,0,0,0,0,0,0,0,0},
    {"Scrape speed max",10,200,200,"/s",2,1,0,0,0,0,0,0,0,0},
    {"Scrape speed width max",0,2500,2400,"mm",3,1,0,0,0,0,0,0,0,0},
    {"Scrape width factory",0,2500,1160,"mm",3,1,0,0,0,0,0,0,0,0},
    {"Scrape speed factory",0,200,200,"/s",1,1,0,0,0,0,0,0,0,0},
    {"Side step small factory",0,500,40,"mm",3,2,0,0,0,0,0,0,0,0},
    {"Side step big factory",0,500,40,"mm",3,2,0,0,0,0,0,0,0,0},
    {"Side step ratio",0,0,1838,"-",4,3,0,1,2,1756,1838,0,0,0},
    {"Side step reduction",1,0,15000,"-",3,0,0,1,2,3000,15000,0,0,0},
    {"Stroke reduction",0,0,3000,"-",2,0,0,1,2,3000,5000,0,0,0},
  },
  { //KLARINET
    {"Scrape Speed, SS",50,400,200,"/s",1,1,1,0,0,0,0,0,0,0},
    {"Scrape Width, SW",0,2400,1660,"mm",3,1,1,0,0,0,0,0,0,0},
    {"Big Side Step, BSS",30,100,60,"mm",3,2,1,0,0,0,0,0,0,0},
    {"Small Side Step, SSS",5,60,30,"mm",3,2,1,0,0,0,0,0,0,0},
    {"Outer Inner Width, OIW",0,200,830,"mm",3,1,1,0,0,0,0,0,0,0},
    {"Screen saver",0,100,0,"-",1,0,0,1,2,0,100,0,0,0},
    {"Machine type",0,100,0,"-",1,0,0,1,2,0,100,0,0,0},
    {"Side step offset",-100,100,0,"mm",3,2,0,0,0,0,0,0,0,0},
    {"Scrape speed min",10,100,10,"/s",2,1,0,0,0,0,0,0,0,0},
    {"Scrape speed max",10,200,200,"/s",2,1,0,0,0,0,0,0,0,0},
    {"Scrape speed width max",0,2500,2400,"mm",3,1,0,0,0,0,0,0,0,0},
    {"Scrape width factory",0,2500,1160,"mm",3,1,0,0,0,0,0,0,0,0},
    {"Scrape speed factory",0,200,200,"/s",1,1,0,0,0,0,0,0,0,0},
    {"Side step small factory",0,500,40,"mm",3,2,0,0,0,0,0,0,0,0},
    {"Side step big factory",0,500,40,"mm",3,2,0,0,0,0,0,0,0,0},
    {"Side step ratio",0,0,1838,"-",4,3,0,1,2,1756,1838,0,0,0},
    {"Side step reduction",1,0,15000,"-",3,0,0,1,2,3000,15000,0,0,0},
    {"Stroke reduction",0,0,3000,"-",2,0,0,1,2,3000,5000,0,0,0},
  },
  { //BAGPIPE
    {"Scrape Speed, SS",50,400,200,"/s",1,1,1,0,0,0,0,0,0,0},
    {"Scrape Width, SW",0,2400,1660,"mm",3,1,1,0,0,0,0,0,0,0},
    {"Big Side Step, BSS",30,100,60,"mm",3,2,1,0,0,0,0,0,0,0},
    {"Small Side Step, SSS",5,60,30,"mm",3,2,1,0,0,0,0,0,0,0},
    {"Outer Inner Width, OIW",0,200,830,"mm",3,1,1,0,0,0,0,0,0,0},
    {"Screen saver",0,100,0,"-",1,0,0,1,2,0,100,0,0,0},
    {"Machine type",0,100,0,"-",1,0,0,1,2,0,100,0,0,0},
    {"Side step offset",-100,100,0,"mm",3,2,0,0,0,0,0,0,0,0},
    {"Scrape speed min",10,100,10,"/s",2,1,0,0,0,0,0,0,0,0},
    {"Scrape speed max",10,200,200,"/s",2,1,0,0,0,0,0,0,0,0},
    {"Scrape speed width max",0,2500,2400,"mm",3,1,0,0,0,0,0,0,0,0},
    {"Scrape width factory",0,2500,1160,"mm",3,1,0,0,0,0,0,0,0,0},
    {"Scrape speed factory",0,200,200,"/s",1,1,0,0,0,0,0,0,0,0},
    {"Side step small factory",0,500,40,"mm",3,2,0,0,0,0,0,0,0,0},
    {"Side step big factory",0,500,40,"mm",3,2,0,0,0,0,0,0,0,0},
    {"Side step ratio",0,0,1838,"-",4,3,0,1,2,1756,1838,0,0,0},
    {"Side step reduction",1,0,15000,"-",3,0,0,1,2,3000,15000,0,0,0},
    {"Stroke reduction",0,0,3000,"-",2,0,0,1,2,3000,5000,0,0,0},
  },
};
//! \Global Service flag
uint8_t gServiceMenu;
//! \Global User parameter max flag
uint8_t gParameterMaxUser;
//! \Global Service parameter max flag
uint16_t gParameterMaxService = 17; //16 is last parameter
//! \Global virtual tab for eeprom simulation 0-99 for machine0, 100-199 for machine1, etc
uint16_t VirtAddVarTab[NB_OF_VAR];
//! \Global virtual tab for eeprom simulation 0-99 for machine0, 100-199 for machine1, etc


uint8_t Batttt = 100;


float BatteryVoltage;
//-----------------------------------------------------------------------------
//! \brief      Initiates the work unit
//! \details    Reads the default parameters, then overwrites with parameters stored
//! in EEPROM. Sets the machine names. Reads the errors and counters from EEPROM
//! Reads the machine type from EEPROM
//! \param[out] uint8_t ReturnValue (0 = Fault, 1 = OK)
uint8_t WRK_CheckConditions(void)
{
  uint8_t ReturnValue = 1; //OK
  //Check SIDESTEPBIG: Condition is that SIDESTEPBIG >= SIDESTEPSMALL + SIDESTEPDIFFERENCE
  if ((gParameterNumber == SIDESTEPBIG) && (gParameterValue < pSIDESTEPSMALL + SIDESTEPDIFFERENCE)) 
    ReturnValue = 0;
  //Check SIDESTEPSMALL: Condition is that SIDESTEPSMALL <= SIDESTEPBIG - SIDESTEPDIFFERENCE
  if ((gParameterNumber == SIDESTEPSMALL) && (gParameterValue > pSIDESTEPBIG - SIDESTEPDIFFERENCE)) 
    ReturnValue = 0;
  //Check SCRAPEWIDTHINNER: Condition is that SCRAPEWIDTHINNER <= SCRAPEWIDTH - SCRAPEWIDTHDIFFERENCE
  if ((gParameterNumber == SCRAPEWIDTHINNER) && (gParameterValue > pSCRAPEWIDTH - SCRAPEWIDTHDIFFERENCE)) 
    ReturnValue = 0;
  //Check SCRAPEWIDTH: Condition is that SCRAPEWIDTH >= SCRAPEWIDTHINNER + SCRAPEWIDTHDIFFERENCE
  if ((gParameterNumber == SCRAPEWIDTH) && (gParameterValue < pSCRAPEWIDTHINNER + SCRAPEWIDTHDIFFERENCE)) 
    ReturnValue = 0;
  return ReturnValue;
}
//-----------------------------------------------------------------------------
//! \brief      Starts the correct progress draw routine
//! \details    Base don the screen number the correct progress bar is started 
//! \param      None
void WRK_DrawProgressBar (void)
{
    int32_t EndPosition = gScrape.EndPosition * gIDX_Motor.Factor;
    int32_t StartPosition = gScrape.StartPosition * gIDX_Motor.Factor;
    if (gScrape.StartPosition > 0)
    {
      RightPercentage = (uint8_t) (((abs(gIDX_Motor.GetPosition) - abs(EndPosition)) * 100) / (abs(StartPosition) - abs(EndPosition)));;
    }
    else
    {
      LeftPercentage = (uint8_t) (((abs(gIDX_Motor.GetPosition) - abs(EndPosition)) * 100) / (abs(StartPosition) - abs(EndPosition)));
    }
    
    if ((gReturnScreen == 30)||(gReturnScreen == 40))
    {
        USR_DrawProgressFull(47,17,LeftPercentage,RightPercentage,4,White);
    }
    else if ((gReturnScreen == 31) || (gReturnScreen == 41))
    {
        USR_DrawProgressFull(47,17,LeftPercentage,RightPercentage,2,White);
    }
    else if (gReturnScreen == 32)
    {
        USR_DrawProgressPartial(47,17,LeftPercentage,RightPercentage,1,White);
    }
    else if (gReturnScreen == 33)
    {
        USR_DrawProgressPartial(47,17,LeftPercentage,RightPercentage,0,White);
    }
    else if  (gReturnScreen == 42)
    {
        USR_DrawProgressFull(47,17,0,0,4,White);
    }
      
}
//-----------------------------------------------------------------------------
//! \brief      Handles the active status sequence
//! \details    Handles actions to startup and execute user input 
//! \param      None
void WRK_HandleActive(void)
{
  static uint8_t OkPressed3Seconds = 0;
  switch (gWRK_Status.SubStatus)
  {
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
        USR_ShowScreen(10,1); //HOME
        gCounter.Sequence = 0;
        WRK_SetStatus(SubStatus,WAITFORUSER);
      }
      break;
    }
    case WAITFORPLUGIN:
    {
      if (USR_ButtonPressed(BtnMenu,USR_SHORTPRESSTIME,1)==1)
      {
        WRK_SetStatus(SubStatus, WAITFORUSER);
        USR_ShowScreen(gReturnScreen,1);  
      }
      else if (PluggedIn())
      {
        USR_SetMessage("","Plugged in","","OK: Continue *: Cancel","","OK",4,1);
        if (USR_ButtonPressed(BtnOk,USR_SHORTPRESSTIME,1)==1)
        {
          WRK_SetStatus(SubStatus, WAITFORUSER);
          USR_ShowScreen(gReturnScreen,1);  
        }
      }
      else
      {
        USR_SetMessage("Battery voltage is too low","","to continue. Please plug in","","*: Cancel","OK",4,1);
      }
      break;
    }
    case WAITFORUSER:
    {
      gScrape.Endless = 0;
      //SERVICE MODE: Press OK for 3 seconds firstly and then MENU button for 3 seconds
      if ((Button[BtnOk].WaitForRelease)&&(gCurrentScreen < 10000) && (gCurrentScreen >= 10))
      {
        if (OkPressed3Seconds)
        {
          if ((USR_ButtonPressed(BtnMenu,3000,1) == 1) && (gCurrentScreen >= 100)&&(gCurrentScreen < 10200))
          { 
            gServiceMenu = 1 - gServiceMenu;
            if (gServiceMenu)
              USR_ShowScreen (101,1);
            else
              USR_ShowScreen (10101,1);
          }  
        }
        else
        {
          Button[BtnMenu].TimeOn = 0;
          Button[BtnMenu].WaitForRelease = 0;
          OkPressed3Seconds = 1;
        }
        break;
      }
      else if ((USR_ButtonPressed(BtnRight,1,1) == 1) && (gCurrentScreen >= 10))
      {
        if (gCurrentScreen < 100) //Normal screen
        {
          if (gCurrentScreen == 10)
          {
            gCurrentScreen = 10;
          }
          else if ((gCurrentScreen >= 20) && (gCurrentScreen <= 29))
          {
            if ((gSTR_Motor.IsInStartPosition == 1) && (gIDX_Motor.IsInStartPosition == 1))
            {
              USR_ShowScreen (gLastScrapeScreen,1);
            }
            else
            {
              gReturnScreen = gCurrentScreen;
              USR_SetMessage("Not in start position","","Scraping menues are","","not available","OK",4,1);
            }
          }
          else if((gCurrentScreen >= 30) && (gCurrentScreen < 40))
          {
            if (gServiceMenu) //Endless only available in Service Mode
                USR_ShowScreen (gLastScrapeScreenEndless,1);
            else if ((gSTR_Motor.IsHomed == 1) && (gIDX_Motor.IsHomed == 1))
            {
              USR_ShowScreen (gLastPositionScreen,1);
            }
            else
              USR_ShowScreen (10,1);
          }
          else if((gCurrentScreen >= 40) && (gCurrentScreen < 50))
          {
            if ((gSTR_Motor.IsHomed == 1) && (gIDX_Motor.IsHomed == 1))
            {
              USR_ShowScreen (gLastPositionScreen,1);
            }
            else
              USR_ShowScreen (10,1);
          }
          else if (gCurrentScreen == 10)
          {
            if (gIDX_Motor.IsHomed == 1)
            {
              USR_ShowScreen (gCurrentScreen + 10,1);
            }
            else
            {
              gReturnScreen = gCurrentScreen;
              USR_SetMessage("Not homed!","","Start position menu is","","not available.","OK",4,1);        
            }
          }
          else
            USR_ShowScreen (gCurrentScreen + 10,1);
          
        }
        else if (gCurrentScreen < 10000) //Menu screen
        {
          USR_ShowScreen ((gCurrentScreen  * 100) + 1,1);
        } 
        else if ((gCurrentScreen > 10100) && (gCurrentScreen < 10200)) //Parameter screen
        {
          LastScreen = gCurrentScreen;
          WRK_SetStatus(MainStatus,ENTERVALUE);
        }
        else if ((gCurrentScreen > 10200) && (gCurrentScreen < 10300))//Command screen 
        {
          LastScreen = gCurrentScreen;
          WRK_SetStatus(MainStatus,EXECUTECOMMAND);
          break;
        }
        else if ((gCurrentScreen > 10300) && (gCurrentScreen < 10400) && (gCurrentError > 0))//Error screen 
        {
          gReturnFromErrorScreen = gCurrentScreen;
          WRK_ShowError (Errors[gCurrentError],0); //View the error only
          break;
        }
      }
      else if ((USR_ButtonPressed(BtnLeft, USR_SHORTPRESSTIME,1) == 1) && (gCurrentScreen >= 10))
      {
        if (gCurrentScreen < 100) //Normal screen
        {
          if (gCurrentScreen == 10)
          {
            gCurrentScreen = 10;
          }
          /*
            if (gIDX_Motor.IsHomed == 1)
            {
              if ((gSTR_Motor.IsInStartPosition == 1)&& (gIDX_Motor.IsInStartPosition == 1))
              {
                if (gServiceMenu) //Endless only available in Service Mode
                  USR_ShowScreen (gLastScrapeScreenEndless,1);
                else
                  USR_ShowScreen (gLastScrapeScreen,1);
              }
              else
              {
                gReturnScreen = gCurrentScreen;
                USR_SetMessage("Not in start position","","Scraping menues are","","not available","OK",4,1);
              }
            }
            else
            {
              gReturnScreen = gCurrentScreen;
              USR_SetMessage("Not homed!","","Start position menu is","","not available.","OK",4,1);        
            }
          }*/
          else if ((gCurrentScreen >= 20) && (gCurrentScreen < 30)) //Position screen
          {
            if ((gSTR_Motor.IsHomed == 0) || (gIDX_Motor.IsHomed == 0)) //Not homed
            {
              USR_ShowScreen (10,1);
            }
            else if ((gSTR_Motor.IsInStartPosition == 0)||(gIDX_Motor.IsInStartPosition == 0)) // Not in START position
            {
              gReturnScreen = gCurrentScreen;
              USR_SetMessage("Not in start position","","Scraping menues are","","not available","OK",4,1);
            }
            else if (gServiceMenu) //Endless only available in Service Mode
              USR_ShowScreen (gLastScrapeScreenEndless,1);
            else
              USR_ShowScreen (gLastScrapeScreen,1);
          }
          else if ((gCurrentScreen >= 30) && (gCurrentScreen < 40)) //Scrape screen
            USR_ShowScreen (gLastPositionScreen,1);
          else if((gCurrentScreen >= 40) && (gCurrentScreen < 50))
            USR_ShowScreen (gLastScrapeScreen,1);
          else
            USR_ShowScreen (gCurrentScreen - 10,1);
        }
        else if((gCurrentScreen >= 100) && (gCurrentScreen < 199))//Main menu if (LastNormalScreen >=10)
        {
          if (LastNormalScreen >=10)
            USR_ShowScreen (LastNormalScreen,1);
        }
        else if ((gCurrentScreen >= 10000) && (gCurrentScreen != 1040101)) //Exception for counter screen
        {
            USR_ShowScreen (gCurrentScreen  / 100,1);
        } 
      }
      else if ((USR_ButtonPressed(BtnDown, USR_SHORTPRESSTIME,1) == 1)  && (gCurrentScreen >= 10))
      {
        USR_ShowScreen (gCurrentScreen + 1,1);
      }
      else if ((USR_ButtonPressed(BtnUp,USR_SHORTPRESSTIME,1) == 1) && (gCurrentScreen >= 10))
      {
        USR_ShowScreen (gCurrentScreen - 1,1);
      }
      else if (USR_ButtonPressed(BtnMenu,USR_SHORTPRESSTIME,1) == 1)
      {
        if (Button[BtnOk].TimeOn == 0)
        {
          if ((gCurrentScreen < 100) && (gCurrentScreen >=10)) //Normal screen
          {
            LastNormalScreen = gCurrentScreen;
            USR_ShowScreen (101,1);
          }
          else if (gCurrentScreen == 1040101) //Reset Counter -> Cancel reset
          {
            USR_ShowScreen(LastScreen,1);
          }
        }
      }
      else if ((gCurrentScreen >= 100)&&(gCurrentScreen < 10000)) //Bigger delay for SERVICE MODE OK BUTTON
      {
        if (USR_ButtonPressed(BtnOk,3000,1)==1)
        {
          gScrape.Endless = 0; //Some function so it is not optimized out
        }
      }
      else if (USR_ButtonPressed(BtnOk,1,1)==1)
      {
        if (gCurrentScreen == 3) //Error screen
        {
          USR_ShowScreen (gReturnFromErrorScreen,1);
        }
        else if (gCurrentScreen == 4) //Message screen
        {
          USR_ShowScreen (gReturnScreen,1);
        }
        else if ((gCurrentScreen > 10400) && (gCurrentScreen < 10500)) //Counter screen
        {
          LastScreen = gCurrentScreen;
          USR_ShowScreen(gCurrentScreen*100+1,1);
        }
        else if (gCurrentScreen == 1040101) //Reset Counter
        {
          USR_ResetServiceCounter();
          USR_ShowScreen(LastScreen,1);
        }
        else if (BattPercentage == 0)
        {
          gReturnScreen = gCurrentScreen;
          USR_SetMessage("Battery voltage is too low","","to continue. Please plug in","","*: Cancel","OK",4,1);
          WRK_SetStatus(SubStatus,WAITFORPLUGIN);
        }
        //From here all screens that are affected by the battery empty warning
        else if (gCurrentScreen == 10) //Find HOME screen
        {
          WRK_HandleResetUnitErrors();
          gReturnScreen = 20;
          gReturnFromErrorScreen = 10;
          USR_SetMessage("","","Please wait","","","",4,1);
          WRK_SetStatus(MainStatus,INITIALIZE);
          WRK_SetStatus(SubStatus,WAITFORINDEXHOME);  
        }
        else if (gCurrentScreen == 20) //Goto START screen
        {
          WRK_HandleResetUnitErrors();
          gReturnScreen = gLastScrapeScreen;
          gReturnFromErrorScreen = 20;
          USR_SetMessage("","","Please wait","","","",4,1);
          WRK_SetStatus(MainStatus,INITIALIZE);
          WRK_SetStatus(SubStatus,WAITFORSTROKEMOTORSTART);  
        }
        else if (gCurrentScreen == 21) //Goto STORAGE screen
        {
          WRK_HandleResetUnitErrors();
          gReturnScreen = 21;
          gReturnFromErrorScreen = 21;
          USR_SetMessage("","","Please wait","","","",4,1);
          WRK_SetStatus(MainStatus,INITIALIZE);
          WRK_SetStatus(SubStatus,WAITFORINDEXHOME);  
        }
        else if (gCurrentScreen == 22) //Goto END OF STROKE screen
        {
          WRK_HandleResetUnitErrors();
          gReturnScreen = 22;
          gReturnFromErrorScreen = 22;
          USR_SetMessage("","","Please wait","","","",4,1);
          WRK_SetStatus(MainStatus,INITIALIZE);
          WRK_SetStatus(SubStatus,WAITFORINDEXHOME);  
        }
        else if (gCurrentScreen == 23) //Goto BEGIN OF STROKE screen
        {
          WRK_HandleResetUnitErrors();
          gReturnScreen = 23;
          gReturnFromErrorScreen = 23;
          USR_SetMessage("","","Please wait","","","",4,1);
          WRK_SetStatus(MainStatus,INITIALIZE);
          WRK_SetStatus(SubStatus,WAITFORINDEXHOME2);  
        }
        else if ((gCurrentScreen == 30)||(gCurrentScreen == 40)) //Start Scrape process with big side step (40 = endless)
        {
            WRK_HandleResetUnitErrors();
            if (gCurrentScreen == 40) gScrape.Endless = 1;
            gReturnScreen = gCurrentScreen;
            gReturnFromErrorScreen = gCurrentScreen;
            gScrape.SideStep = gIDX_Motor.Factor * (pSIDESTEPBIG * 84 / 15 * 1000 / pSIDESTEPRATIO); //Pulses -> Value * 10 * 840 / 1500
            gScrape.StartPosition = pSCRAPEWIDTH * 42 / 15 * 1000 / pSIDESTEPRATIO; //Pulses -> Value * 10 / 2 (Half scrape width) * 840 / 1500 um -> Pulse
            gScrape.EndPosition = 0;
            WRK_SetStatus(MainStatus, SCRAPEREED);
        }
        else if ((gCurrentScreen == 31)||(gCurrentScreen == 41)) //Start Scrape process with small side step (41 is endless)
        {
            WRK_HandleResetUnitErrors();
            if (gCurrentScreen == 41) gScrape.Endless = 1;
            gReturnScreen = gCurrentScreen;
            gReturnFromErrorScreen = gCurrentScreen;
            gScrape.SideStep = gIDX_Motor.Factor * (pSIDESTEPSMALL * 84 / 15 * 1000 / pSIDESTEPRATIO); //Pulses -> Value * 10 * 840 / 1500
            gScrape.StartPosition = pSCRAPEWIDTH * 42 / 15 * 1000 / pSIDESTEPRATIO; //Pulses -> Value * 10 / 2 (Half scrape width) * 840 / 1500 um -> Pulse
            gScrape.EndPosition = 0;
            WRK_SetStatus(MainStatus, SCRAPEREED);
        }
        else if (gCurrentScreen == 32) //Start Scrape process only outer sections
        {
          WRK_HandleResetUnitErrors();
          gReturnScreen = gCurrentScreen;
          gReturnFromErrorScreen = gCurrentScreen;
          gScrape.SideStep = gIDX_Motor.Factor * (pSIDESTEPSMALL * 84 / 15 * 1000 / pSIDESTEPRATIO); //Pulses -> Value * 10 * 840 / 1500
          gScrape.StartPosition = pSCRAPEWIDTH * 42 / 15 * 1000 / pSIDESTEPRATIO; //Pulses -> Value * 10 / 2 (Half scrape width) * 840 / 1500 um -> Pulse
          gScrape.EndPosition = pSCRAPEWIDTHINNER * 42 / 15 * 1000 / pSIDESTEPRATIO; //Pulses -> Value * 10 / 2 (Half scrape width inner) * 840 / 1500 um -> Pulse;
          WRK_SetStatus(MainStatus, SCRAPEREED);
        }
        else if (gCurrentScreen == 33) //Start Scrape process only inner sections
        {
          WRK_HandleResetUnitErrors();
          gReturnScreen = gCurrentScreen;
          gReturnFromErrorScreen = gCurrentScreen;
          gScrape.SideStep = gIDX_Motor.Factor * (pSIDESTEPSMALL * 84 / 15 * 1000 / pSIDESTEPRATIO); //Pulses -> Value * 10 * 840 / 1500
          gScrape.StartPosition = pSCRAPEWIDTHINNER * 42 / 15 * 1000 / pSIDESTEPRATIO; //Pulses -> Value * 10 / 2 (Half scrape width) * 840 / 1500 um -> Pulse
          gScrape.EndPosition = 0;              
          WRK_SetStatus(MainStatus, SCRAPEREED);
        }
        else if (gCurrentScreen == 42) //Start Scrape process without side steps
        {
          WRK_HandleResetUnitErrors();
          gScrape.SideStep = 0;
          gReturnScreen = gCurrentScreen;
          gReturnFromErrorScreen = gCurrentScreen;
          WRK_SetStatus(MainStatus, SCRAPENOSIDESTEPS);
        }
        OkPressed3Seconds = 0;
      }
      break;
    }
    default:
      break;
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
}
//-----------------------------------------------------------------------------
//! \brief      Handles the command
//! \details    Handles actions from the command menu
//! \param[in]  uint16_t newCommand
//! \param[in]  uint16_t LastScreen
void WRK_HandleCommand(uint32_t newCommand)
{
  newCommand = (gCurrentScreen - 1020000) * 10 + newCommand;
  switch (newCommand)
  {
    case 1010: //Menu key pressed, go back
    case 2010: //Menu key pressed, go back
    case 3010: //Menu key pressed, go back
    case 2030: //Menu key pressed, go back
    {
      USR_ShowScreen (gCurrentScreen / 100,1); 
      WRK_SetStatus(MainStatus,ACTIVE);
      WRK_SetStatus(SubStatus,WAITFORUSER);
      break;
    }
    case 1011: //Set stroke length: OK -> INDEX HOME Position -> STROKE HOME position
    {
      USR_ShowScreen (gCurrentScreen + 1,1);   
      WRK_SetStatus(MainStatus, INITIALIZE);
      WRK_SetStatus(SubStatus, WAITFORINDEXHOME);
      break;
    }
    case 1031: //Second time OK -> Goto START and menu up
    {
      USR_ShowScreen (gCurrentScreen + 1,1);   
      WRK_SetStatus(MainStatus, INITIALIZE);
      WRK_SetStatus(SubStatus, WAITFORSTROKEMOTORSTART);
      break;
    }
    
    case 2011: //Reset factory OK
    {
      WRK_HandleResetFactory();
      USR_ShowScreen (gCurrentScreen / 100,1); 
      WRK_SetStatus(MainStatus,ACTIVE);
      WRK_SetStatus(SubStatus,WAITFORUSER);
      break;
    }
    case 32: //Stroke motor on
    {
      break;
    }
    case 33: //Stroke motor off
    {
      break;
    }
    case 34: //Stroke motor faster
    {
      break;
    }
    case 35: //Stroke motor slower
    {
      break;
    }
    default:
      break;
  }
}  

//-----------------------------------------------------------------------------
//! \brief      Handles the contrast of the screen
//! \details    switches the screen to low contrast when not operated for a certain time to save power
//! \param      None
void WRK_HandleContrast(void)
{
  if ((PluggedIn()) || (SCREENSAVERON == 0))
  {
    gCounter.User = 0;
    WRK_UpdateContrast = HIGHCONTRAST;
  }
  else if (BattPercentage == 0)
  {
    WRK_UpdateContrast = NOBATTERYCONTRAST;  
  }
  else if (SCREENSAVERON)
  {
    if (gCounter.User < 0xffffffff) gCounter.User += 100;
    if (gCounter.User > LOWPOWERTIME) WRK_UpdateContrast = LOWCONTRAST;
  }
  if (PluggedIn())
  {
    gCounter.User = 0;
    ssd1306_SetContrast(HIGHCONTRAST);
  }
  else if (BattPercentage == 0)
  {
    ssd1306_SetContrast(NOBATTERYCONTRAST);  
  }
  else
  {
    if (gCounter.User < 0xffffffff) gCounter.User += 100;
    if ((gCounter.User > LOWPOWERTIME)&&(SCREENSAVERON)) 
    {
      ssd1306_SetContrast(LOWCONTRAST);
    }
    else if (SCREENSAVERON)
    {
      ssd1306_SetContrast(HIGHCONTRAST);
    }
  }
  
}
//-----------------------------------------------------------------------------
//! \brief      Handles the routine for entering a value
//! \details    Handles the routine for entering a value
//! \param      None
void WRK_HandleEnterValue(void)
{
  switch (gWRK_Status.SubStatus)
  {
    case UNDEFINED:
    {      
      USR_EnterValue(gCurrentScreen - 10101);
      WRK_SetStatus(SubStatus,WAITFORUSER);
      break;
    }
    case WAITFORUSER2:
    {
      if (USR_ButtonPressed(BtnOk,10,1)==1) //Correct the conflict and save the parameters
      {
        if (gParameterNumber == SIDESTEPBIG)
        {
          USR_SaveParameter();
          gParameterNumber = SIDESTEPSMALL;
          gParameterValue = gParameterValue - SIDESTEPDIFFERENCE;
          USR_SaveParameter();
        }
        else if (gParameterNumber == SIDESTEPSMALL)
        {
          USR_SaveParameter();
          gParameterNumber = SIDESTEPBIG;
          gParameterValue = gParameterValue + SIDESTEPDIFFERENCE;
          USR_SaveParameter();
        }
        else if (gParameterNumber == SCRAPEWIDTH)
        {
          USR_SaveParameter();
          gParameterNumber = SCRAPEWIDTHINNER;
          gParameterValue = gParameterValue - SCRAPEWIDTHDIFFERENCE;
          USR_SaveParameter();
        }
        else if (gParameterNumber == SCRAPEWIDTHINNER)
        {
          USR_SaveParameter();
          gParameterNumber = SCRAPEWIDTH;
          gParameterValue = gParameterValue + SCRAPEWIDTHINNER;
          USR_SaveParameter();
        }
        USR_ShowScreen (LastScreen,1); 
        WRK_SetStatus(MainStatus,ACTIVE);
        WRK_SetStatus(SubStatus,WAITFORUSER);
      }
      else if (USR_ButtonPressed(BtnMenu,10,1)==1) //Cancel the change
      {  
        USR_ShowScreen (LastScreen,1); 
        WRK_SetStatus(MainStatus,ACTIVE);
        WRK_SetStatus(SubStatus,WAITFORUSER);
      }
      break;
    }
    case WAITFORUSER:
    {
      if (USR_ButtonPressed(BtnOk,10,1)==1) //Save and return to menu
      {
        if (WRK_CheckConditions() == 1) //Check conditions of the parameter. If not ok show screen
        {
          USR_SaveParameter();
          USR_ShowScreen (LastScreen,1); 
          WRK_SetStatus(MainStatus,ACTIVE);
          WRK_SetStatus(SubStatus,WAITFORUSER);
        }
        else
        {
          if (gParameterNumber == SIDESTEPBIG)
          {
            WRK_ShowError(31001,1);
          }
          else if (gParameterNumber == SIDESTEPSMALL)
          {
            WRK_ShowError(31002,1);
          }
          else if (gParameterNumber == SCRAPEWIDTH)
          {
            WRK_ShowError(31003,1);
          }
          else if (gParameterNumber == SCRAPEWIDTHINNER)
          {
            WRK_ShowError(31004,1);
          }
          WRK_SetStatus(SubStatus, WAITFORUSER2);
        }
      }
      else if (USR_ButtonPressed(BtnMenu,10,1)==1) //Cancel, don't save
      {
        USR_ShowScreen (LastScreen,1); 
        WRK_SetStatus(MainStatus,ACTIVE);
        WRK_SetStatus(SubStatus,WAITFORUSER);
      }
      else if ((USR_ButtonPressed(BtnLeft, USR_SHORTPRESSTIME,1)==1)&&(gMachineType[gMachine/100].Parameters[gParameterNumber].Options == 0)) //Move cursor right if allowed
      {
        USR_CursorLeft();
      }
      else if ((USR_ButtonPressed(BtnRight, USR_SHORTPRESSTIME,1)==1)&&(gMachineType[gMachine/100].Parameters[gParameterNumber].Options == 0)) //Move cursor left if allowed
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
}
//-----------------------------------------------------------------------------
//! \brief      Handles the execution of a command from the command menu
//! \details    Handles the execution of a command from the command menu
//! \param      None
void WRK_HandleExecuteCommand(void)
{
  switch (gWRK_Status.SubStatus)
  {
    case UNDEFINED:
    {      
      USR_ShowScreen(gCurrentScreen * 100 + 1,1);
      WRK_SetStatus(SubStatus,WAITFORUSER);
      break;
    }
    case WAITFORUSER: //Execute command
    {
      if (USR_ButtonPressed(BtnMenu,USR_SHORTPRESSTIME,1)==1)
        WRK_HandleCommand(0);
      else if (USR_ButtonPressed(BtnOk,USR_SHORTPRESSTIME,1)==1) 
        WRK_HandleCommand(1);
      else if (USR_ButtonPressed(BtnLeft, USR_SHORTPRESSTIME,1)==1)
        WRK_HandleCommand(2);
      else if (USR_ButtonPressed(BtnRight, USR_SHORTPRESSTIME,1)==1)
        WRK_HandleCommand(3);
      else if (USR_ButtonPressed(BtnUp,USR_SHORTPRESSTIME,1)==1)
        WRK_HandleCommand(4);
      else if (USR_ButtonPressed(BtnDown, USR_SHORTPRESSTIME,1)==1)
        WRK_HandleCommand(5);
      break;
    }
    default:
      break;
  }
}

//-----------------------------------------------------------------------------
//! \brief      Handles the initialization of index and stroke motor
//! \details    Handles the initialization of index and stroke motor
//! \param      None
void WRK_HandleInitialize(void)
{
  if (gSTR_Status.MainStatus == UNITERROR) 
  {
    STR_Stop();
    IDX_Stop();//IDX_Set(STOP,0);
    WRK_ShowError(gSTR_ErrorNumber,1);
    WRK_SetStatus(MainStatus, ACTIVE);
    WRK_SetStatus(SubStatus, WAITFORUSER); 
    return;
  }
  if (gIDX_Status.MainStatus == UNITERROR) 
  {
    STR_Stop();
    IDX_Stop();//IDX_Set(STOP,0);
    WRK_ShowError (gIDX_ErrorNumber,1);
    WRK_SetStatus(MainStatus, ACTIVE);
    WRK_SetStatus(SubStatus, WAITFORUSER); 
    return;
  }
  switch (gWRK_Status.SubStatus)
  {
    case WAITFORINDEXHOME: //Index home and then Stroke HOME 
    case WAITFORINDEXHOME2: //Index Home and the Stroke START
    {
      if (IDX_Set(HOME,0)== READY)
      {
        IDX_Set(UNDEFINED,0);
        if (gWRK_Status.SubStatus == WAITFORINDEXHOME)
          WRK_SetStatus(SubStatus,WAITFORSTROKEMOTORHOME);
        else
          WRK_SetStatus(SubStatus,WAITFORSTROKEMOTORSTART2);

      }
      break;
    }
    case WAITFORINDEXSTART:
    {
      //if (IDX_Set(GOTOPOSITION, gScrape.StartPosition )==READY)
      if (IDX_Set(GOTOPOSITION, -pSCRAPEWIDTH * 42 / 15 * 1000 / pSIDESTEPRATIO)==READY)
      {
        IDX_Set(UNDEFINED,0);
        gIDX_Motor.IsInStartPosition = 1;
        WRK_SetStatus(MainStatus,ACTIVE);
        WRK_SetStatus(SubStatus,WAITFORUSER);
        USR_ShowScreen(gReturnScreen,1); 
      }
      break;
    }
    case WAITFORSTROKEMOTORHOME:
    {
      if (STR_Set(HOME,0)== READY)
      {
        STR_Set(UNDEFINED,0);
        if (gCurrentScreen > 10200)
        {
          WRK_SetStatus(MainStatus,EXECUTECOMMAND);  
          USR_ShowScreen(gCurrentScreen + 1,1);
          WRK_SetStatus(SubStatus,WAITFORUSER); 
          
        }
        else
        {
          WRK_SetStatus(MainStatus,ACTIVE);
          WRK_SetStatus(SubStatus,WAITFORUSER);
          USR_ShowScreen(gReturnScreen,1); 
        }
      }
      break;
    }
    case WAITFORSTROKEMOTORSTART: //Stroke to start, then Index to Start
    case WAITFORSTROKEMOTORSTART2: //Stroke to start, then end
    {
      if (STR_Set(GOTOSTARTPOSITION,0)== READY)
      {
        STR_Set(UNDEFINED,0);
        if (gCurrentScreen > 10200)
        {
          USR_ShowScreen (gCurrentScreen/100,1); 
          WRK_SetStatus(MainStatus,ACTIVE);
          WRK_SetStatus(SubStatus,WAITFORUSER);
        }
        else if (gWRK_Status.SubStatus == WAITFORSTROKEMOTORSTART2)
        {
          WRK_SetStatus(MainStatus,ACTIVE);
          WRK_SetStatus(SubStatus,WAITFORUSER);
          USR_ShowScreen(gReturnScreen,1); 
        }
        else if (gIDX_Motor.IsInStartPosition == 0) 
        {  
          gScrape.StartPosition = pSCRAPEWIDTH * 42 / 15 * 1000 / pSIDESTEPRATIO; //Pulses -> Value * 10 / 2 (Half scrape width) * 840 / 1500 um -> Pulse
          WRK_SetStatus(SubStatus,WAITFORINDEXSTART);
        }
        else
        {
          WRK_SetStatus(MainStatus,ACTIVE);
          WRK_SetStatus(SubStatus,WAITFORUSER);
          USR_ShowScreen(gReturnScreen,1); 
        }
      }
      break;
    }
    default:
      break;
  }
}

//-----------------------------------------------------------------------------
//! \brief      Resets all parameters to the factory setting
//! \details    Resets all parameters to the factory setting for all machine types
//! \param      None
void WRK_HandleResetFactory(void)
{
  for (uint8_t i = 0; i < NROFMACHINETYPES; i++)
  {
    memcpy( &gMachineType[i].Parameters, &DefaultsMachine[i], sizeof DefaultsMachine[i]);
    gMachineType[i].Parameters[MACHINETYPE].Value = gMachine ; //Prevent machine type to be overwritten with default
    for (uint8_t j=0; j<gParameterMaxService ;j++) //Check eeprom memory
    {
      EE_WriteVariable(i * 100 + j, gMachineType[i].Parameters[j].Value);
    }
  }
}
//-----------------------------------------------------------------------------
//! \brief      Resets unit error in index and stroke motor
//! \details    Only resets if the unit is in error
//! \param      None
void WRK_HandleResetUnitErrors(void)
{
  if (gSTR_Status.MainStatus == UNITERROR) 
  {
    STR_Set(UNDEFINED,0);
  }
  if (gIDX_Status.MainStatus == UNITERROR)
  {
    IDX_Set(UNDEFINED,0);
  }
  gSTR_ErrorNumber = 0;
  gIDX_ErrorNumber = 0;
}
//-----------------------------------------------------------------------------
//! \brief      Handles the side step action when scraping the reed
//! \details    Handles actions to control the side step motor
//! \param      None
void WRK_HandleSideStep(void)
{    
  if (((gWRK_Status.MainStatus == SCRAPEREED) || (gWRK_Status.MainStatus == SCRAPENOSIDESTEPS)) && ((gWRK_Status.SubStatus == WAITFORSTROKEMOTORSTOP) || (gWRK_Status.SubStatus == WAITFORUSER)))
  {
    if (gSTR_Motor.Encoder == STARTPOSITION)//Start position is reached
    {
      if (STR_StartFlag == 0)
      {
        STR_StartFlag = 1;
        gScrape.NextSideStep = 1;
        gScrape.NextScrape = 1;
        if ((gScrape.NextScrape == 1) || (gWRK_Status.MainStatus == SCRAPENOSIDESTEPS)) //To wait for passing home switch again
        {
          gScrape.NextScrape = 0;
          if (gScrape.Status == RightSideLastStep)
          {
            WRK_DrawProgressBar();
            WRK_SetScrapeStatus (RightSideLastScrape);
          }
          else if (gScrape.Status == RightSideLastScrape)
          {
            WRK_SetScrapeStatus (RightSideEndOfScraping);
            STR_Stop();
          }
          else if (gScrape.Status == LeftSideLastStep)
          {
            WRK_DrawProgressBar();
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
            WRK_DrawProgressBar();
          }
        }
      }
    }
    else
    {
      gScrape.NextSideStep  = 0;
      gScrape.NextScrape = 0;
      STR_StartFlag = 0;
    }
    if (gScrape.NextSideStep)
    {
      gScrape.NextSideStep = 0;
    
      if ((gScrape.Status == RightSideNormalStep)||(gScrape.Status == RightSidePauseRequested))//((WRK_SetScrapeStatus (= 0)||(WRK_SetScrapeStatus (= 6))
      {
        if ((gIDX_Motor.SetPosition - (gScrape.EndPosition * gIDX_Motor.Factor))> gScrape.SideStep) 
        {
          IDX_SetPosition(gIDX_Motor.SetPosition - gScrape.SideStep);
        }
        else
        {
          IDX_SetPosition(gScrape.EndPosition * gIDX_Motor.Factor);
          WRK_SetScrapeStatus (RightSideLastStep);//WRK_SetScrapeStatus ( 1;
        }
      }
      else if ((gScrape.Status == LeftSideNormalStep)||(gScrape.Status == LeftSidePauseRequested))//((WRK_SetScrapeStatus (= 3)||(WRK_SetScrapeStatus (= 8))
      {
        if ((abs(gIDX_Motor.SetPosition) - abs(gScrape.EndPosition * gIDX_Motor.Factor)) > gScrape.SideStep) 
        {
          IDX_SetPosition(gIDX_Motor.SetPosition + gScrape.SideStep);
        }
        else 
        {
          IDX_SetPosition(gScrape.EndPosition * gIDX_Motor.Factor);
          WRK_SetScrapeStatus (LeftSideLastStep);//WRK_SetScrapeStatus ( 4;
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
//! \brief      Handles the scraping of the reed
//! \details    Handles actions to perform the actual scraping
//! \param      None
void WRK_HandleScrapeReed (void)
{
  if (gSTR_Status.MainStatus == UNITERROR) 
  {
    STR_Stop();
    IDX_Set(STOP,0);
    WRK_ShowError(gSTR_ErrorNumber,1);
    WRK_SetStatus(MainStatus, ACTIVE);
    WRK_SetStatus(SubStatus, WAITFORUSER); 
    return;
  }
  if (gIDX_Status.MainStatus == UNITERROR) 
  {
    STR_Stop();;
    IDX_Set(STOP,0);
    WRK_ShowError(gIDX_ErrorNumber,1);
    WRK_SetStatus(MainStatus, ACTIVE);
    WRK_SetStatus(SubStatus, WAITFORUSER); 
    return;
  }
  if ((BattPercentage == 0) && ((gScrape.Endless == 1)||(gScrape.Status == NoSideStep))) //Stop automatic scraping in case battery is empty
  {
    gScrape.Endless = 0; 
    StoppedBecauseBatteryEmpty = 1;
  }
  switch (gWRK_Status.SubStatus)
  {
    case UNDEFINED:
    {
      if ((gIDX_Motor.IsHomed == 0) && (gWRK_Status.MainStatus != SCRAPENOSIDESTEPS)) //No side steps doesn't need IDX motor
      {
        gReturnScreen = gCurrentScreen;
        WRK_ShowError(11001,1);
        WRK_SetStatus(MainStatus, ACTIVE);
        WRK_SetStatus(SubStatus, WAITFORUSER);
      }
      else if ((gIDX_Motor.IsInStartPosition == 0) && (gWRK_Status.MainStatus != SCRAPENOSIDESTEPS)) //No side steps doesn't need IDX motor
      {
        gReturnScreen = gCurrentScreen;
        WRK_ShowError(11002,1);
        WRK_SetStatus(MainStatus, ACTIVE);
        WRK_SetStatus(SubStatus, WAITFORUSER);
      }
      else if (gSTR_Motor.IsInStartPosition == 0)
      {
        gReturnScreen = gCurrentScreen;
        WRK_ShowError(21002,1);
        WRK_SetStatus(MainStatus, ACTIVE);
        WRK_SetStatus(SubStatus, WAITFORUSER);
      }
      else
      {
        StoppedBecauseBatteryEmpty = 0;
        gScrape.Speed = gMachineType[gMachine/100].Parameters[SCRAPESPEED].Value*pSTROKEREDUCTION/100*60/100; // Value / 100 * 50 (ratio) * 60 (1 min = 60 s)
        gReturnScreen = gCurrentScreen;
        USR_ClearScreen(5); //Clear line 1
        USR_SetMessage("Scraping","","","","","OK = Pause",4,0);
        gScrape.EndPosition *= -1; //Left side of the reed
        gScrape.StartPosition *= -1; //Left side of the reed
        RightPercentage = 100;
        LeftPercentage = 100;
        WRK_DrawProgressBar();
        USR_IncreaseCounters();
        if (gWRK_Status.MainStatus == SCRAPENOSIDESTEPS)
        {
          WRK_SetScrapeStatus (NoSideStep);
          WRK_DrawProgressBar();
          WRK_SetStatus(SubStatus, WAITFORINDEXHOME);
          //WRK_SetStatus(SubStatus, WAITFORSTROKEMOTORSTART);  
        }
        else
        {
          //WRK_SetScrapeStatus (RightSideNormalStep);
          WRK_SetScrapeStatus (LeftSideNormalStep);
          WRK_SetStatus(SubStatus, WAITFORPOSITION);
        }
      }
      break;
    }
    case WAITFORPOSITION:
    {
      if (IDX_Set(GOTOPOSITION, gScrape.StartPosition )==READY)
      {
        WRK_DrawProgressBar();
        IDX_Set(UNDEFINED,0);
        WRK_SetStatus(SubStatus,WAITFORSTROKEMOTORSTART);
      }
      break;
    }
    case WAITFORSTROKEMOTORSTART:
    {
      if (STR_Set(START,gScrape.Speed) == READY) 
      {
        STR_Set(UNDEFINED,0);
        WRK_SetStatus(SubStatus, WAITFORSTROKEMOTORSTOP);
      }
      break;
    }
    case WAITFORSTROKEMOTORSTOP:
    {
      if (USR_ButtonPressed(BtnOk,USR_SHORTPRESSTIME,1)==1)
      {
        if (gScrape.Status == NoSideStep)
        {
          WRK_SetScrapeStatus (NoSideStepPauseRequested);
          if (gSTR_Motor.Encoder < STARTPOSITION) //Reduce the speed, so the motor can stop in time
            gSTR_Motor.SetSpeed = STR_GOTOSTARTSPEED;
        }
        else if ((gScrape.Status == RightSideNormalStep) || (gScrape.Status == RightSideLastStep))
        {
          if (gScrape.Status == RightSideNormalStep) 
            gScrape.StatusPause = gScrape.Status;
          else
            gScrape.StatusPause = (enuScrapeStatus) (gScrape.Status + 1);
          WRK_SetScrapeStatus (RightSidePauseRequested);
          if (gSTR_Motor.Encoder < STARTPOSITION) //Reduce the speed, so the motor can stop in time
            gSTR_Motor.SetSpeed = STR_GOTOSTARTSPEED;
          USR_SetMessage("","","Pause scraping","","","",4,1);
          break;
        }
        else if ((gScrape.Status == LeftSideNormalStep)||(gScrape.Status == LeftSideLastStep))
        {
          if (gScrape.Status == LeftSideNormalStep) 
            gScrape.StatusPause = gScrape.Status;
          else
            gScrape.StatusPause = (enuScrapeStatus) (gScrape.Status + 1);
          if (gSTR_Motor.Encoder < STARTPOSITION) //Reduce the speed, so the motor can stop in time
            gSTR_Motor.SetSpeed = STR_GOTOSTARTSPEED;
          WRK_SetScrapeStatus (LeftSidePauseRequested);
          USR_SetMessage("","","Pause scraping","","","",4,1);
          break;
        }
      }
      //if (gScrape.Status == RightSideEndOfScraping)
      if (gScrape.Status == LeftSideEndOfScraping)
      {
        LeftPercentage = 0;
        WRK_DrawProgressBar();
        gIDX_Motor.MainStatus = (INACTIVE);
        IDX_Set(UNDEFINED,0);
        //WRK_SetScrapeStatus (LeftSideNormalStep);
        WRK_SetScrapeStatus (RightSideNormalStep);
        gScrape.EndPosition *= -1; //Left side of the reed
        gScrape.StartPosition *= -1; //Left side of the reed
        WRK_SetStatus(SubStatus, WAITFORPOSITION);            
      }
      //else if (gScrape.Status == LeftSideEndOfScraping)
      else if (gScrape.Status == RightSideEndOfScraping)
      {
        RightPercentage = 0;
        WRK_DrawProgressBar();
        gIDX_Motor.MainStatus = (INACTIVE);
        IDX_Set(UNDEFINED,0);
        if (gScrape.EndPosition == 0)
        {
          if (gScrape.Endless)
          {
            RightPercentage = 100;  
            LeftPercentage = 100;
            gScrape.EndPosition *= -1; //Right side of the reed
            gScrape.StartPosition *= -1; //Right side of the reed
            USR_ShowScreen(gReturnScreen,1);
            USR_ClearScreen(5); //Clear line 1
            USR_SetMessage("Scraping","","","","","OK = Pause",4,0);
            USR_IncreaseCounters();
            WRK_SetScrapeStatus (LeftSideNormalStep);
            WRK_SetStatus(SubStatus, WAITFORPOSITION);
          }
          else
          {
            if (StoppedBecauseBatteryEmpty == 1)
            {
              WRK_SetStatus(SubStatus, WAITFORPLUGIN);  
              USR_SetMessage("Battery voltage is too low","","to continue. Please plug in","","*: Cancel","OK",4,1);
            }
            else
            {
              //gScrape.StartPosition *= -1; //Right side of the reed
              WRK_SetStatus(SubStatus, WAITFORINDEXSTART);
            }
          }
        }
        else //Scraping cycle is ready. Goto start position
        {
          WRK_SetStatus(SubStatus, WAITFORINDEXSTART);
        }
      }
      else if ((gScrape.Status == RightSidePaused) || (gScrape.Status == LeftSidePaused) || (gScrape.Status == NoSideStepPaused))
      {
        if (StoppedBecauseBatteryEmpty)
        {
          WRK_SetStatus(SubStatus,WAITFORPLUGIN);  
        }
        else
        {
            USR_SetMessage("Scraping paused","","","","","* = Cancel / OK = Continue",4,1);
            WRK_SetStatus(SubStatus,WAITFORUSER);
        }
      }
      else if ((gScrape.Status == NoSideStep) && (StoppedBecauseBatteryEmpty))
      {
        WRK_SetScrapeStatus (NoSideStepPauseRequested);
        if (gSTR_Motor.Encoder < STARTPOSITION) //Reduce the speed, so the motor can stop in time
          gSTR_Motor.SetSpeed = STR_GOTOSTARTSPEED;
      }
      break;
    }
    case WAITFORINDEXHOME:
    {
      if (IDX_Set(HOME,0)==READY)
      {
        WRK_SetStatus(SubStatus, WAITFORSTROKEMOTORSTART);  
      }
      break;
    }
    case WAITFORINDEXSTART:
    {
      //if (IDX_Set(GOTOPOSITION, pSCRAPEWIDTH * 42 / 15 * 1000 / pSIDESTEPRATIO)==READY)
      if (IDX_Set(GOTOPOSITION, -pSCRAPEWIDTH * 42 / 15 * 1000 / pSIDESTEPRATIO)==READY)
      {
        IDX_Set(UNDEFINED,0);
        gIDX_Motor.IsInStartPosition = 1;
        //Check battery status
        if (gScrape.Endless)
        {
          USR_ShowScreen(gReturnScreen,1);
          USR_ClearScreen(5); //Clear line 1
          USR_SetMessage("Scraping","","","","","OK = Pause",4,0);
          USR_IncreaseCounters();
          WRK_SetScrapeStatus (RightSideNormalStep);//Right side of the reed
          WRK_SetStatus(SubStatus, WAITFORPOSITION);
        }
        else
        {
          if (StoppedBecauseBatteryEmpty == 1)
          {
            WRK_SetStatus(SubStatus, WAITFORPLUGIN);  
            USR_SetMessage("Battery voltage is too low","","to continue. Please plug in","","*: Cancel","OK",4,1);
          }
          else
          {
            WRK_SetStatus(MainStatus, ACTIVE);
            WRK_SetStatus(SubStatus, WAITFORUSER);
            USR_ShowScreen(gReturnScreen,1); 
          }
        }
      }
      break;
    }
    case WAITFORPLUGIN:
    {
      if (USR_ButtonPressed(BtnMenu,USR_SHORTPRESSTIME,1)==1)
      {
        WRK_SetStatus(MainStatus, ACTIVE);
        WRK_SetStatus(SubStatus, WAITFORUSER);
        USR_ShowScreen(gReturnScreen,1);  
      }
      else if (PluggedIn())
      {
        USR_SetMessage("","Plugged in","","OK: Continue *: Cancel","","OK",4,1);
        if (USR_ButtonPressed(BtnOk,USR_SHORTPRESSTIME,1)==1)
        {
          StoppedBecauseBatteryEmpty = 0;
          gScrape.Endless = 1;
          gScrape.EndPosition *= -1; //Right side of the reed
          gScrape.StartPosition *= -1; //Right side of the reed
          USR_IncreaseCounters();
          
          USR_ShowScreen(gReturnScreen,1);
          USR_ClearScreen(5); //Clear line 1
          USR_SetMessage("Scraping","","","","","OK = Pause",4,0);
          if (gWRK_Status.MainStatus == SCRAPENOSIDESTEPS)
          {
            WRK_SetScrapeStatus (NoSideStep);
            WRK_SetStatus(SubStatus, WAITFORSTROKEMOTORSTART);  
          }
          else
          {
            WRK_DrawProgressBar();
            WRK_SetScrapeStatus (RightSideNormalStep);
            WRK_SetStatus(SubStatus, WAITFORPOSITION);
          }
        }
      }
      else
      {
        USR_SetMessage("Battery voltage is too low","","to continue. Please plug in","","*: Cancel","OK",4,1);
      }
      break;
    }
    case WAITFORUSER:
    {
      if (USR_ButtonPressed(BtnOk,USR_SHORTPRESSTIME,1)==1)
      {
        if (gScrape.Status == NoSideStepPaused)//pause scraping no side step
        {
          WRK_SetScrapeStatus(NoSideStep);
          USR_ShowScreen(gReturnScreen,1);
          USR_ClearScreen(5); //Clear line 1
          USR_SetMessage("Scraping","","","","","OK = Pause",4,0);
          WRK_DrawProgressBar();
          WRK_SetStatus(SubStatus,WAITFORSTROKEMOTORSTART);
          break;
        }
        if (gScrape.Status == RightSidePaused)//pause scraping right part.
        {
          WRK_SetScrapeStatus(gScrape.StatusPause);//(RightSideNormalStep);
          USR_ShowScreen(gReturnScreen,1);
          USR_ClearScreen(5); //Clear line 1
          USR_SetMessage("Scraping","","","","","OK = Pause",4,0);
          WRK_DrawProgressBar();
          WRK_SetStatus(SubStatus,WAITFORSTROKEMOTORSTART);
          break;
        }
        else if (gScrape.Status == LeftSidePaused)//(WRK_SetScrapeStatus (= 9) //Pause scraping left part.
        {
          WRK_SetScrapeStatus(gScrape.StatusPause);//(LeftSideNormalStep)
          
          USR_ShowScreen(gReturnScreen,1);
          USR_ClearScreen(5); //Clear line 1
          USR_SetMessage("Scraping","","","","","OK = Pause",4,0);
          WRK_DrawProgressBar();
          WRK_SetStatus(SubStatus,WAITFORSTROKEMOTORSTART);
          break;
        }
      }
      else if (USR_ButtonPressed(BtnMenu,USR_SHORTPRESSTIME,1)==1)
      {
        USR_SetMessage("","","Cancel scraping","","","",4,1);  
        gScrape.Endless = 0;
        WRK_SetStatus(SubStatus, WAITFORINDEXSTART);
      }
      break;
    }
    default:
      break;
  }
}

//-----------------------------------------------------------------------------
//! \brief      Handles the power to the sensors
//! \details    switches off the power to the sensors when they are not needed to save power
//! \param      None
void WRK_HandleSensors(void)
{
  //Switch off sensors LED power when not used
  if ((gIDX_Motor.MainStatus == INACTIVE) && (gSTR_Motor.MainStatus==INACTIVE) && (gWRK_Status.MainStatus == ACTIVE) &&(gWRK_Status.SubStatus == WAITFORUSER) )
  {
    //PWR_SensorsOff();
  }
}
//-----------------------------------------------------------------------------
//! \brief      Handles the work sequence
//! \details    Handles actions to form the main sequence 
//! \param      None
void WRK_HandleSequence(void)
{
  static uint8_t TickTime = 0; 
  if (TickTime++ < 99) return; //100ms 
  TickTime = 0;

  gCounter.Sequence += 100;

  WRK_HandleSensors();
  WRK_HandleContrast();
    
  switch (gWRK_Status.MainStatus)
  {
    case UNDEFINED:
    {
      WRK_SetStatus(MainStatus,INACTIVE);
      break;
    }
    case INACTIVE:
    {
      gCounter.Sequence = 0;
      USR_ShowScreen(1,1);
      WRK_SetStatus(MainStatus,ACTIVE);
      break;
    }
    case SCRAPENOSIDESTEPS:
    case SCRAPEREED:
    {
      WRK_HandleScrapeReed();
      break;
    }
    case EXECUTECOMMAND:
    {
      WRK_HandleExecuteCommand();
      break;
    }
    case ENTERVALUE:
    {
      WRK_HandleEnterValue();
      break;
    }
    //case SCRAPENOSIDESTEPS:
    //{
    //  WRK_HandleScrapeNoSideSteps();
    //  break;
    //}
    case INITIALIZE:
    {
      WRK_HandleInitialize();
      break;
    }
    case ACTIVE:
    {
      WRK_HandleActive();
      break;
    }
    default:
        break;
  }
}
int32_t newSetSpeed;

//-----------------------------------------------------------------------------
//! \brief      Handles the actions when sensing the stroke motor encoder
//! \details    Stops motor when reaching start posiiton. Reduces speed when nearly there
//! \param      None
void WRK_HandleSTREncoder(void)
{
  
  if ((gSTR_Status.MainStatus == GOTOSTARTPOSITION) && (gSTR_Status.SubStatus == WAITFORSTROKEMOTORSTART))
  {
    if (gSTR_Motor.Encoder == STARTPOSITION)
    {
      STR_Stop();
    }
    else if ((gSTR_Motor.Encoder >= (4 * pSTROKEREDUCTION)/100) && (gSTR_Motor.Encoder <=STARTPOSITION))
    {
      newSetSpeed = (STR_GOTOSTARTSPEED * ((325 * pSTROKEREDUCTION/5000) - gSTR_Motor.Encoder)) / 100;
      if (newSetSpeed < STR_GOTOSTARTSPEED)
        if (newSetSpeed < STR_HOMESPEED) 
          gSTR_Motor.SetSpeed = STR_HOMESPEED;   
        else
          gSTR_Motor.SetSpeed = newSetSpeed; 
      else
        gSTR_Motor.SetSpeed = STR_GOTOSTARTSPEED; 
    }
  }
}
//-----------------------------------------------------------------------------
//! \brief      Handles the actions when sensing the stroke motor home sensor
//! \details    Detects motor turning and sets the speeds
//! \param      None
void WRK_HandleSTRHomeFlag (void)
{
  gSTR_Motor.Encoder = 0;
  if ((gScrape.Status == RightSideLastScrape)||(gScrape.Status == LeftSideLastScrape)) 
  {
    gSTR_Motor.SetSpeed = STR_GOTOSTARTSPEED;
  }
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
    IDX_HandleMotor();
    STR_HandleMotor();
  }
}

//-----------------------------------------------------------------------------
//! \brief      Initiates the work unit
//! \details    Reads the default parameters, then overwrites with parameters stored
//! in EEPROM. Sets the machine names. Reads the errors and counters from EEPROM
//! Reads the machine type from EEPROM
//! \param      None
void WRK_Init(void)
{
  uint16_t Counter = 0;
  uint16_t VirtaddVarCounter = 0;
  uint16_t ParameterTemp[20];

  //Fill the virtual table
  for (uint8_t i = 0; i < NROFMACHINETYPES; i++)
  {
    memcpy( &gMachineType[i].Parameters, &DefaultsMachine[i], sizeof DefaultsMachine[i]);
    for (uint8_t j = 0; j<NROFPARAMETERS;j++)
    {
      
      //VirtAddVarTab[i*NROFPARAMETERS+j] = i*100 + j;
      VirtAddVarTab[VirtaddVarCounter] = i*100 + j;
      VirtaddVarCounter ++;
    }
  }
  strcpy(gMachineType[0].Name, "Oboe");
  strcpy(gMachineType[1].Name, "Bassoon");
  strcpy(gMachineType[2].Name, "Clarinet");
  strcpy(gMachineType[3].Name, "Bagpipe");
  //Add the virtual names for the errors
  for (uint8_t j = 0; j<NROFERRORS;j++)
  {
    VirtAddVarTab[VirtaddVarCounter] = 1000 + j;
    VirtaddVarCounter ++;
  }
  //Add the virtual names for the counters for each machine type
  for (uint8_t j = 0; j<NROFCOUNTERS;j++)
  {
    VirtAddVarTab[VirtaddVarCounter] = 2000 + j * 2;     //LSB
    VirtaddVarCounter++;
    VirtAddVarTab[VirtaddVarCounter] = 2000 + j * 2 + 1; //MSB
    VirtaddVarCounter++;
  }
        
  if (EE_ReadVariable(6, &gMachine)!=0)//variable does not exist
  {
    gMachine = 0;
  }
  
  uint8_t NotEmpty = 0;

  for (uint8_t j=0; j<NROFMACHINETYPES; j++)
  {
    for (uint8_t i=0; i<=gParameterMaxService ;i++) //Check eeprom memory
    {
      
      if (EE_ReadVariable(i + j * 100, &ParameterTemp[i]) != 0)
      {
        ParameterTemp[i] = gMachineType[j].Parameters[i].Value;//Never stored. Keep the default value;
      }
      if ((ParameterTemp[i]>0) && (i != 6)) NotEmpty = 1;
    }
    if (NotEmpty == 1) //Copy new values over gParameter
    {
      for (uint8_t i=0; i<=gParameterMaxService ;i++) //Check eeprom memory
      {
        gMachineType[j].Parameters[i].Value = ParameterTemp[i];
      }
    }
    else //Store default values from gParameter
    {
      for (uint8_t i=0; i<=gParameterMaxService ;i++) //Check eeprom memory
      {
        EE_WriteVariable(i+ j * 100, gMachineType[j].Parameters[i].Value);
      }
    }
  }
  //Make sure max value can meet condition corrections
  if ((gMachineType[gMachine/100].Parameters[SCRAPEWIDTHINNER].Max > gMachineType[gMachine/100].Parameters[SCRAPEWIDTH].Max - SCRAPEWIDTHDIFFERENCE))
    gMachineType[gMachine/100].Parameters[SCRAPEWIDTHINNER].Max = gMachineType[gMachine/100].Parameters[SCRAPEWIDTH].Max - SCRAPEWIDTHDIFFERENCE;
  if ((gMachineType[gMachine/100].Parameters[SIDESTEPSMALL].Max > gMachineType[gMachine/100].Parameters[SIDESTEPBIG].Max - SIDESTEPDIFFERENCE))
    gMachineType[gMachine/100].Parameters[SIDESTEPSMALL].Max = gMachineType[gMachine/100].Parameters[SIDESTEPBIG].Max - SIDESTEPDIFFERENCE;
  
  for (uint8_t i=1; i<=gParameterMaxService ;i++) //Always assume at least 1 User parameter
  {
    if (gMachineType[gMachine/100].Parameters[i].UserAccess == 1)
    {
      gParameterMaxUser = i;
    }
  }
  for (uint8_t i=1; i<=gCommandMaxService ;i++) //Always assume at least 1 User parameter
  {
    if (gCommands[i].UserAccess == 1)
    {
      gCommandMaxUser = i;
    }
  }
  //Read errors
  //EE_ReadVariable 1000 is the first available error place. If > 1099 --> 1001
  //EE_ReadVariable 1001 - 1099 are the errors
  for (uint8_t i = 0; i<NROFERRORS; i++)
  {
    if (EE_ReadVariable(i + 1000, &Errors[i])!=0)
    {
      Errors[i] = 0;
    }
  }
  //Read counters
  if (EE_ReadVariable( 2000, &Counter)!=0)
  {
    Counter = 0;
  }
  gCounter.MasterCounter = Counter;
  if (EE_ReadVariable( 2001, &Counter)!=0)
    {
    Counter = 0;
  }
  gCounter.MasterCounter += Counter * 0x10000;
  if (EE_ReadVariable( 2002, &Counter)!=0)
    {
    Counter = 0;
  }
  gCounter.ServiceCounter = Counter;
  if (EE_ReadVariable( 2003, &Counter)!=0){
    Counter = 0;
  }
  gCounter.ServiceCounter += Counter * 0x10000;
  //Check first initialization
  if (Errors[0]==0) Errors[0]=1001; //First available place to store the next error
}

//-----------------------------------------------------------------------------
//! \brief      Sets the scrape status
//! \details    Sets the scrape status and stores the old scrape status
//! \param[in]  enuScrapeStatus newStatus
void WRK_SetScrapeStatus (enuScrapeStatus newStatus)
{
  gScrape.StatusOld = gScrape.Status;
  gScrape.Status = newStatus;
}
//-----------------------------------------------------------------------------
//! \brief      Sets the WRK module status
//! \details    Sets the selected status (main or sub) and stores the 
//! previous status. Sets Substatus to UNDEFINED if mainstatus is changed
//! \param[in]  newType   MainStatus or SubStatus
//! \param[in]  newStatus New status for the selected status
void WRK_SetStatus (enuType newType, enuStatus newStatus)
{
    if (newType == MainStatus)
    {
        gWRK_Status.MainStatusOld = gWRK_Status.MainStatus;
        gWRK_Status.MainStatus = newStatus;
        gWRK_Status.SubStatusOld = gWRK_Status.SubStatus;
        gWRK_Status.SubStatus = UNDEFINED;
    }
    else
    {
        gWRK_Status.SubStatusOld = gWRK_Status.SubStatus;
        gWRK_Status.SubStatus = newStatus;
    }
}

//-----------------------------------------------------------------------------
//! \brief      Shows the corresponding text for the error
//! \details    Sets and saves the error text for displaying in screen 3
//! \param[in]  uint16_t newError
void WRK_ShowError (uint16_t newError, uint8_t newSave)
{
  char strResult[20];
  sprintf(strResult, "Error: %u",newError);
  if (newError == 11001) //Index not homed
    USR_SetMessage(strResult,"","Side step not homed","","or defect","ok",3,1);  
  else if (newError == 11002) //Index motor not homed
    USR_SetMessage("Side step set is not at","","start position","","","OK",3,1);
  else if (newError == 21002) //Stroke motor not at start position
    USR_SetMessage("Stroke set is not at","","start position","","","OK",3,1);
  else if (newError == 13001) //Index motor not turning or encoder not detected
    USR_SetMessage(strResult,"","Side step movement blocked","","or defect","ok",3,1);
  else if (newError == 13002) //Index motor turning wrong direction or encoder channels swapped
    USR_SetMessage(strResult,"","Side step moving the wrong","","direction","ok",3,1);
  else if (newError == 13003) //Stroke motor turning but home sensor status not changing
    USR_SetMessage(strResult,"","Side step motor is turning ","","but home switch not detected","ok",3,1);
  else if (newError == 23001) //Stroke motor not turning or encoder not detected
    USR_SetMessage(strResult,"","Stroke movement blocked","","or defect","ok",3,1);
  else if (newError == 23002) //Stroke motor turning but encoder not detected
    USR_SetMessage(strResult,"","Stroke moving but feedback","","signal missing","ok",3,1);
  else if (newError == 23003) //Stroke motor turning but home sensor not detected when expected
    USR_SetMessage(strResult,"","Stroke moving but home","","sensor not detected","ok",3,1);
  else if (newError == 23004) //Stroke motor is at starttposition but home sensor isnot off
    USR_SetMessage(strResult,"","Stroke at start position","","but home sensor is on","ok",3,1);
  else if (newError == 31001) 
    USR_SetMessage(strResult,"","Side step conflict:","","BSS is smaller than SSS","ok",3,1);
  else if (newError == 31002) 
    USR_SetMessage(strResult,"","Side step conflict:","","SSS is bigger than BSS","ok",3,1);
  else if (newError == 31003) 
    USR_SetMessage(strResult,"","Side step conflict:","","SW is smaller than IOW","OK = Reset",3,1);
  else if (newError == 31004) 
    USR_SetMessage(strResult,"","Side step conflict:","","IOW is bigger than SW","OK = Reset",3,1);
  else
  {
    USR_SetMessage(strResult,"","No error description found","","","OK = Reset",3,1);
  }
  if (newSave == 1)
    USR_SaveError(newError,1);
}

//-----------------------------------------------------------------------------

