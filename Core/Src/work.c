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
//! \Global Main Menu container
StcMainMenu MainMenu[NROFMAINMENUITEMS]=
{
  {"SETTINGS", 1},
  {"COMMANDS", 1},  
  {"ERRORS", 1},
  {"COUNTERS", 0},
  {"DATA",0}
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
  {"RESET TO FACTORY   ",1},
  {"SET STROKE LENGTH  ",1},
  {"STROKE MOTOR       ",0},
};
//! \Global User command max flag
uint8_t gCommandMaxUser;
//! \Global Service command max flag
uint16_t gCommandMaxService = sizeof(gCommands)/sizeof(gCommands[0])-1;
//! \Global parameters container
StcParameters DefaultsMachine[NROFMACHINETYPES][20]=
{ //OBOE
	{ 
    {"SCRAPE WIDTH      ",0,1160,780,"MM",3,1,1,0},
    {"SCRAPE WIDTH INNER",0,200,390,"MM",3,1,1,0},
    {"SCRAPE SPEED      ",50,200,50,"RPS",2,1,1,0},
    {"SIDE STEP SMALL   ",5,50,30,"MM",3,2,1,0},
    {"SIDE STEP BIG     ",30,100,50,"MM",3,2,1,0},
    {"SCREEN SAVER      ",0,100,0,"-",1,0,0,1},
    {"MACHINE TYPE      ",0,100,0,"-",1,0,0,1},
    {"SIDE STEP OFFSET  ",-100,100,0,"MM",3,2,0,0},
    {"SCRAPE SPEED MIN  ",10,100,10,"RPS",2,1,0,0},
    {"SCRAPE SPEED MAX  ",10,200,200,"RPS",2,1,0,0},
    {"SCRAPE WIDTH MAX  ",0,2500,1160,"MM",3,1,0,0},
    {"SCRAPE WIDTH FACT ",0,2500,780,"MM",3,1,0,0},
    {"SCRAPE SPEED FACT ",0,200,200,"RPS",1,1,0,0},
    {"SIDE ST SMALL FACT",0,500,30,"MM",3,2,0,0},
    {"SIDE ST BIG FACT  ",0,500,30,"MM",3,2,0,0},
    {"SIDE ST RATIO     ",1,2,1756,"MM",4,3,0,0},
	},
	{ //BASSOON
    {"SCRAPE WIDTH      ",0,2400,1660,"MM",3,1,1,0},
    {"SCRAPE WIDTH INNER",0,200,830,"MM",3,1,1,0},
    {"SCRAPE SPEED      ",50,200,50,"RPS",2,1,1,0},
    {"SIDE STEP SMALL   ",5,60,40,"MM",3,2,1,0},
    {"SIDE STEP BIG     ",40,100,60,"MM",3,2,1,0},
    {"SCREEN SAVER      ",0,100,0,"-",1,0,0,1},
    {"MACHINE TYPE      ",0,100,0,"-",1,0,0,1},
    {"SIDE STEP OFFSET  ",-100,100,0,"MM",3,2,0,0},
    {"SCRAPE SPEED MIN  ",10,100,10,"RPS",2,1,0,0},
    {"SCRAPE SPEED MAX  ",10,200,200,"RPS",2,1,0,0},
    {"SCRAPE WIDTH MAX  ",0,2500,2400,"MM",3,1,0,0},
    {"SCRAPE WIDTH FACT ",0,2500,1160,"MM",3,1,0,0},
    {"SCRAPE SPEED FACT ",0,200,200,"RPS",1,1,0,0},
    {"SIDE ST SMALL FACT",0,500,40,"MM",3,2,0,0},
    {"SIDE ST BIG FACT  ",0,500,40,"MM",3,2,0,0},   
    {"SIDE ST RATIO     ",1,2,1838,"MM",4,3,0,0},
	},
	{ //KLARINET
    {"SCRAPE WIDTH      ",0,2400,1660,"MM",3,1,1,0},
    {"SCRAPE WIDTH INNER",0,200,830,"MM",3,1,1,0},
    {"SCRAPE SPEED      ",50,200,50,"RPS",2,1,1,0},
    {"SIDE STEP SMALL   ",5,60,40,"MM",3,2,1,0},
    {"SIDE STEP BIG     ",40,100,60,"MM",3,2,1,0},
    {"SCREEN SAVER      ",0,100,0,"-",1,0,0,1},
    {"MACHINE TYPE      ",0,100,0,"-",1,0,0,1},
    {"SIDE STEP OFFSET  ",-100,100,0,"MM",3,2,0,0},
    {"SCRAPE SPEED MIN  ",10,100,10,"RPS",2,1,0,0},
    {"SCRAPE SPEED MAX  ",10,200,200,"RPS",2,1,0,0},
    {"SCRAPE WIDTH MAX  ",0,2500,2400,"MM",3,1,0,0},
    {"SCRAPE WIDTH FACT ",0,2500,1160,"MM",3,1,0,0},
    {"SCRAPE SPEED FACT ",0,200,200,"RPS",1,1,0,0},
    {"SIDE ST SMALL FACT",0,500,40,"MM",3,2,0,0},
    {"SIDE ST BIG FACT  ",0,500,40,"MM",3,2,0,0},
    {"SIDE ST RATIO     ",1,2,1838,"MM",4,3,0,0},
	},
  { //BAGPIPE
    {"SCRAPE WIDTH      ",0,2400,1660,"MM",3,1,1,0},
    {"SCRAPE WIDTH INNER",0,200,830,"MM",3,1,1,0},
    {"SCRAPE SPEED      ",50,200,50,"RPS",2,1,1,0},
    {"SIDE STEP SMALL   ",5,60,40,"MM",3,2,1,0},
    {"SIDE STEP BIG     ",40,100,60,"MM",3,2,1,0},
    {"SCREEN SAVER      ",0,100,0,"-",1,0,0,1},
    {"MACHINE TYPE      ",0,100,0,"-",1,0,0,1},
    {"SIDE STEP OFFSET  ",-100,100,0,"MM",3,2,0,0},
    {"SCRAPE SPEED MIN  ",10,100,10,"RPS",2,1,0,0},
    {"SCRAPE SPEED MAX  ",10,200,200,"RPS",2,1,0,0},
    {"SCRAPE WIDTH MAX  ",0,2500,2400,"MM",3,1,0,0},
    {"SCRAPE WIDTH FACT ",0,2500,1160,"MM",3,1,0,0},
    {"SCRAPE SPEED FACT ",0,200,200,"RPS",1,1,0,0},
    {"SIDE ST SMALL FACT",0,500,40,"MM",3,2,0,0},
    {"SIDE ST BIG FACT  ",0,500,40,"MM",3,2,0,0},
    {"SIDE ST RATIO     ",1,2,1838,"MM",4,3,0,0},
  },
};
//! \Global Service flag
uint8_t gServiceMenu;
//! \Global User parameter max flag
uint8_t gParameterMaxUser;
//! \Global Service parameter max flag
uint16_t gParameterMaxService = 15; //15 is last parameter
//! \Global virtual tab for eeprom simulation 0-99 for machine0, 100-199 for machine1, etc
uint16_t VirtAddVarTab[NB_OF_VAR];

uint16_t ParameterTemp[20];




float BatteryVoltage;

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
//! \brief      Initiates the work unit
//! \details    Reads the default parameters, then overwrites with parameters stored
//! in EEPROM. Sets the machine names. Reads the errors and counters from EEPROM
//! Reads the machine type from EEPROM
//! \param      None
void WRK_Init(void)
{
  //Create the virtual table
  for (uint8_t i = 0; i < NROFMACHINETYPES; i++)
  {
    memcpy( &gMachineType[i].Parameters, &DefaultsMachine[i], sizeof DefaultsMachine[i]);
    for (uint8_t j = 0; j<NROFPARAMETERS;j++)
    {
      VirtAddVarTab[i*NROFPARAMETERS+j] = i*100 + j;
    }
  }
  strcpy(gMachineType[0].Name, "OBOE");
  strcpy(gMachineType[1].Name, "BASSOON");
  strcpy(gMachineType[2].Name, "CLARINET");
  strcpy(gMachineType[3].Name, "BAGPIPE");
  //Add the virtual names for the errors
  for (uint8_t j = 0; j<NROFERRORS;j++)
  {
    VirtAddVarTab[NROFMACHINETYPES*NROFPARAMETERS+j] = 1000 + j;
  }
  //Add the virtual names for the counters for each machine type
  for (uint8_t i = 0; i < NROFMACHINETYPES; i++)
  {
    for (uint8_t j = 0; j<NROFCOUNTERS;j++)
    {
      VirtAddVarTab[i*NROFCOUNTERS+j] = 2000 + i*100 + j;
    }
  }
      
  EE_ReadVariable(6, &gMachine);
  
  uint8_t NotEmpty = 0;

  for (uint8_t j=0; j<NROFMACHINETYPES; j++)
	{
		for (uint8_t i=0; i<gParameterMaxService ;i++) //Check eeprom memory
		{
			EE_ReadVariable(i + j * 100, &ParameterTemp[i]);
			if ((ParameterTemp[i]>0) && (i != 6)) NotEmpty = 1;
		}
		if (NotEmpty == 1) //Copy new values over gParameter
		{
			for (uint8_t i=0; i<gParameterMaxService ;i++) //Check eeprom memory
			{
				gMachineType[j].Parameters[i].Value = ParameterTemp[i];
			}
		}
		else //Store default values from gParameter
		{
			for (uint8_t i=0; i<gParameterMaxService ;i++) //Check eeprom memory
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
  //Read errors
  //EE_ReadVariable 1000 is the first available error place. If > 1099 --> 1001
  //EE_ReadVariable 1001 - 1099 are the errors
  for (uint8_t i = 0; i<NROFERRORS; i++)
  {
    EE_ReadVariable(i + 1000, &Errors[i]);
  }
  //Read counters
  for (uint8_t i = 0; i < NROFMACHINETYPES; i++)
  {
    EE_ReadVariable(i * 100 + 2000, &gMachineType[i].MasterCounter);
    EE_ReadVariable(i * 100 + 2001, &gMachineType[i].ServiceCounter);
  }
  //Check first initialization
  if (Errors[0]==0) Errors[0]=1001; //First available place to store the next error
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
//! \brief      Handles the command
//! \details    Handles actions from the command menu
//! \param[in]  uint16_t newCommand
//! \param[in]  uint16_t LastScreen
void WRK_HandleCommand(uint32_t newCommand)
{
	newCommand = (gCurrentScreen - 1020000) * 10 + newCommand;
  switch (newCommand)
  {
    case 1010: //Cancel, go back
		case 2010: //Cancel, go back
	  case 3010: //Cancel, go back
    {
      USR_ShowScreen (gCurrentScreen / 100); 
      WRK_SetStatus(MainStatus,ACTIVE);
      WRK_SetStatus(SubStatus,WAITFORUSER);
      break;
    }
    case 1011: //Reset factory OK
    {
      WRK_HandleResetFactory();
      USR_ShowScreen (gCurrentScreen / 100); 
      WRK_SetStatus(MainStatus,ACTIVE);
      WRK_SetStatus(SubStatus,WAITFORUSER);
      break;
    } 
    case 2011: //Set stroke length: OK -> INDEX HOME Position -> STROKE HOME position
    {
      USR_ShowScreen (gCurrentScreen + 1);   
      WRK_SetStatus(MainStatus, INITIALIZE);
      WRK_SetStatus(SubStatus, WAITFORINDEXHOME);
			break;
		}
		case 2031: //Second time OK -> Goto START and menu up
		{
      USR_ShowScreen (gCurrentScreen + 1);   
			WRK_SetStatus(MainStatus, INITIALIZE);
			WRK_SetStatus(SubStatus, WAITFORSTROKEMOTORSTART);
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
//! \brief      Handles the scraping of the reed
//! \details    Handles actions to perform the actual scraping
//! \param      None
void WRK_HandleScrapeReed (void)
{
  switch (gWRK_Status.SubStatus)
  {
    case UNDEFINED:
    {
      if (gIDX_Motor.IsHomed == 0)
      {
        USR_SetMessage("SIDE STEP SET IS","","NOT HOMED","","","OK");
        gReturnScreen = gCurrentScreen;
        USR_ShowScreen(3);
        USR_SaveError(11001,1);
        WRK_SetStatus(MainStatus, ACTIVE);
        WRK_SetStatus(SubStatus, WAITFORUSER);
      }
      else if (gIDX_Motor.IsInStartPosition == 0)
      {
        USR_SetMessage("SIDE STEP SET NOT","","AT START POSITION","","","OK");
        gReturnScreen = gCurrentScreen;
        USR_ShowScreen(3);
        USR_SaveError(11002,1);
        WRK_SetStatus(MainStatus, ACTIVE);
        WRK_SetStatus(SubStatus, WAITFORUSER);
      }
      else if (gSTR_Motor.IsInStartPosition == 0)
      {
        USR_SetMessage("STROKE SET NOT","","AT START POSITION","","","OK");
        gReturnScreen = gCurrentScreen;
        USR_ShowScreen(3);
        USR_SaveError(21002,1);
        WRK_SetStatus(MainStatus, ACTIVE);
        WRK_SetStatus(SubStatus, WAITFORUSER);
      }
      else
      {
        USR_SetMessage("","SCRAPING","","OK: PAUSE SCRAPING","","OK");
        gReturnScreen = gCurrentScreen;
        USR_IncreaseCounters();
        USR_ShowScreen(4);
        WRK_SetScrapeStatus (RightSideNormalStep);
        WRK_SetStatus(SubStatus, WAITFORPOSITION);
      }
      break;
    }
    case WAITFORPOSITION:
    {
      gScrape.Speed = gMachineType[gMachine/100].Parameters[SCRAPESPEED].Value*30; // Value / 100 * 50 (ratio) * 60 (1 min = 60 s)
      USR_ClearPosition();
      if (IDX_Set(GOTOPOSITION, gScrape.StartPosition )==READY)
      {
#ifdef IDX_SHOWREALPOSITION
          USR_ShowPosition((int32_t) ((float) gIDX_Motor.GetPosition / gIDX_Motor.UmPerPulse * gMachineType[gMachine/100].Parameters[SIDESTRATIO].Value / 1000));
#else
          USR_ShowPosition((int32_t) ((float) gIDX_Motor.SetPosition / gIDX_Motor.UmPerPulse * gMachineType[gMachine/100].Parameters[SIDESTRATIO].Value / 1000));
#endif
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
        if ((gScrape.Status == RightSideNormalStep) || (gScrape.Status == RightSideLastStep))
        {
          if (gScrape.Status == RightSideNormalStep) 
            gScrape.StatusPause = gScrape.Status;
          else
            gScrape.StatusPause = (enuScrapeStatus) (gScrape.Status + 1);
          WRK_SetScrapeStatus (RightSidePauseRequested);
          USR_SetMessage("","","    PAUSE SCRAPING","","","");
          USR_ShowScreen(4);
          break;
        }
        else if ((gScrape.Status == LeftSideNormalStep)||(gScrape.Status == LeftSideLastStep))
        {
          if (gScrape.Status == LeftSideNormalStep) 
            gScrape.StatusPause = gScrape.Status;
          else
            gScrape.StatusPause = (enuScrapeStatus) (gScrape.Status + 1);

          WRK_SetScrapeStatus (LeftSidePauseRequested);
          USR_SetMessage("","","    PAUSE SCRAPING","","","");
          USR_ShowScreen(4);
          break;
        }
      }
      if (gScrape.Status == RightSideEndOfScraping)
      {
        gIDX_Motor.MainStatus = (INACTIVE);
        IDX_Set(UNDEFINED,0);
        WRK_SetScrapeStatus (LeftSideNormalStep);
        gScrape.EndPosition *= -1; //Left side of the reed
        gScrape.StartPosition *= -1; //Left side of the reed
        WRK_SetStatus(SubStatus, WAITFORPOSITION);            
      }
      else if (gScrape.Status == LeftSideEndOfScraping)
      {
        gIDX_Motor.MainStatus = (INACTIVE);
        IDX_Set(UNDEFINED,0);
        if (gScrape.EndPosition == 0)
        {
          if (gScrape.Endless)
          {
            gScrape.EndPosition *= -1; //Right side of the reed
            gScrape.StartPosition *= -1; //Right side of the reed
            USR_SetMessage("","SCRAPING","","OK: PAUSE SCRAPING","","OK");
            USR_IncreaseCounters();
            USR_ShowScreen(4);
            WRK_SetScrapeStatus (RightSideNormalStep);
            WRK_SetStatus(SubStatus, WAITFORPOSITION);
          }
          else
          {
            WRK_SetStatus(MainStatus, ACTIVE);
            WRK_SetStatus(SubStatus, WAITFORUSER);
            USR_ShowScreen(gReturnScreen); 
          }
        }
        else
        {
          USR_ClearPosition();
          WRK_SetStatus(SubStatus, WAITFORINDEXHOME);
        }
      }
      else if ((gScrape.Status == RightSidePaused) || (gScrape.Status == LeftSidePaused))
      {
        USR_SetMessage("","OK: CONTINUE SCRAPING",""," *: CANCEL SCRAPING","","*OK");
        USR_ShowScreen(4);
        WRK_SetStatus(SubStatus,WAITFORUSER);
      }
      break;
    }
    case WAITFORINDEXHOME:
    {
      if (IDX_Set(GOTOPOSITION, 0)==READY)
      {
        IDX_Set(UNDEFINED,0);
        if (gScrape.Endless)
        {
          gScrape.EndPosition *= -1; //Right side of the reed
          gScrape.StartPosition *= -1; //Right side of the reed
          USR_SetMessage("","SCRAPING","","OK: PAUSE SCRAPING","","OK");
          USR_IncreaseCounters();
          USR_ShowScreen(4);
          WRK_SetScrapeStatus (RightSideNormalStep);//Right side of the reed
          WRK_SetStatus(SubStatus, WAITFORPOSITION);
        }
        else
        {
          WRK_SetStatus(MainStatus, ACTIVE);
          WRK_SetStatus(SubStatus, WAITFORUSER);
          USR_ShowScreen(gReturnScreen); 
        }
      }
    }
    case WAITFORUSER:
    {
      if (USR_ButtonPressed(BtnOk,USR_SHORTPRESSTIME,1)==1)
      {
        if (gScrape.Status == RightSidePaused)//pause scraping right part.
        {
          WRK_SetScrapeStatus(gScrape.StatusPause);//(RightSideNormalStep);
          USR_SetMessage("","SCRAPING","","OK: PAUSE SCRAPING","","OK");
          USR_ShowScreen(4);
#ifdef IDX_SHOWREALPOSITION
          USR_ShowPosition((int32_t) ((float) gIDX_Motor.GetPosition / gIDX_Motor.UmPerPulse * gMachineType[gMachine/100].Parameters[SIDESTRATIO].Value / 1000));
#else
          USR_ShowPosition((int32_t) ((float) gIDX_Motor.SetPosition / gIDX_Motor.UmPerPulse * gMachineType[gMachine/100].Parameters[SIDESTRATIO].Value / 1000));
#endif
          WRK_SetStatus(SubStatus,WAITFORSTROKEMOTORSTART);
          break;
        }
        else if (gScrape.Status == LeftSidePaused)//(WRK_SetScrapeStatus (= 9) //Pause scraping left part.
        {
          WRK_SetScrapeStatus(gScrape.StatusPause);//(LeftSideNormalStep)
          USR_SetMessage("","SCRAPING","","OK: PAUSE SCRAPING","","OK");
          USR_ShowScreen(4);
#ifdef IDX_SHOWREALPOSITION
          USR_ShowPosition((int32_t) ((float) gIDX_Motor.GetPosition / gIDX_Motor.UmPerPulse * gMachineType[gMachine/100].Parameters[SIDESTRATIO].Value / 1000));
#else
          USR_ShowPosition((int32_t) ((float) gIDX_Motor.SetPosition / gIDX_Motor.UmPerPulse * gMachineType[gMachine/100].Parameters[SIDESTRATIO].Value / 1000));
#endif
          WRK_SetStatus(SubStatus,WAITFORSTROKEMOTORSTART);
          break;
        }
      }
      else if (USR_ButtonPressed(BtnMenu,USR_SHORTPRESSTIME,1)==1)
      {
        USR_SetMessage("","","   CANCEL SCRAPING","","","");
        USR_ShowScreen(4);  
        gScrape.Endless = 0;
        WRK_SetStatus(SubStatus, WAITFORINDEXHOME);
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
    PWR_SensorsOff();
  }
}

//-----------------------------------------------------------------------------
//! \brief      Handles the contrast of the screen
//! \details    switches the screen to low contrast when not operated for a certain time to save power
//! \param      None
void WRK_HandleContrast(void)
{
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
      USR_ShowScreen(gCurrentScreen * 100 + 1);
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
uint8_t WRK_CheckConditions(void)
{
  uint8_t ReturnValue = 1; //OK
  //Check SIDESTEPBIG: Condition is that SIDESTEPBIG >= SIDESTEPSMALL + SIDESTEPDIFFERENCE
  if ((gParameterNumber == SIDESTEPBIG) && (gParameterValue < gMachineType[gMachine/100].Parameters[SIDESTEPSMALL].Value + SIDESTEPDIFFERENCE)) 
    ReturnValue = 0;
  //Check SIDESTEPSMALL: Condition is that SIDESTEPSMALL <= SIDESTEPBIG - SIDESTEPDIFFERENCE
  if ((gParameterNumber == SIDESTEPSMALL) && (gParameterValue > gMachineType[gMachine/100].Parameters[SIDESTEPBIG].Value - SIDESTEPDIFFERENCE)) 
    ReturnValue = 0;
  //Check SCRAPEWIDTHINNER: Condition is that SCRAPEWIDTHINNER <= SCRAPEWIDTH - SCRAPEWIDTHDIFFERENCE
  if ((gParameterNumber == SCRAPEWIDTHINNER) && (gParameterValue > gMachineType[gMachine/100].Parameters[SCRAPEWIDTH].Value - SCRAPEWIDTHDIFFERENCE)) 
    ReturnValue = 0;
  //Check SCRAPEWIDTH: Condition is that SCRAPEWIDTH >= SCRAPEWIDTHINNER + SCRAPEWIDTHDIFFERENCE
  if ((gParameterNumber == SCRAPEWIDTH) && (gParameterValue < gMachineType[gMachine/100].Parameters[SCRAPEWIDTHINNER].Value + SCRAPEWIDTHDIFFERENCE)) 
    ReturnValue = 0;
  return ReturnValue;
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
        USR_ShowScreen (LastScreen); 
        WRK_SetStatus(MainStatus,ACTIVE);
        WRK_SetStatus(SubStatus,WAITFORUSER);
      }
      else if (USR_ButtonPressed(BtnMenu,10,1)==1) //Cancel the change
      {  
        USR_ShowScreen (LastScreen); 
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
          USR_ShowScreen (LastScreen); 
          WRK_SetStatus(MainStatus,ACTIVE);
          WRK_SetStatus(SubStatus,WAITFORUSER);
        }
        else
        {
          if (gParameterNumber == SIDESTEPBIG)
          {
            USR_SetMessage("SIDE STEP CONFLICT","","BIG <= SMALL","","OK: CORRECT *: CANCEL","OK");
            USR_ShowScreen(3);
            USR_SaveError(31001,1);
          }
          else if (gParameterNumber == SIDESTEPSMALL)
          {
            USR_SetMessage("SIDE STEP CONFLICT","","SMALL >= BIG","","OK: CORRECT *: CANCEL","OK");
            USR_ShowScreen(3);
            USR_SaveError(31002,1);          
          }
          else if (gParameterNumber == SCRAPEWIDTH)
          {
            USR_SetMessage("SIDE STEP CONFLICT","","WIDTH <= INNER","","OK: CORRECT *: CANCEL","OK");
            USR_ShowScreen(3);
            USR_SaveError(31003,1);
          }
          else if (gParameterNumber == SCRAPEWIDTHINNER)
          {
            USR_SetMessage("SIDE STEP CONFLICT","","INNER >= WIDTH","","OK: CORRECT *: CANCEL","OK");
            USR_ShowScreen(3);
            USR_SaveError(31004,1);
          }
          WRK_SetStatus(SubStatus, WAITFORUSER2);
        }
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
}

//-----------------------------------------------------------------------------
//! \brief      Handles the scraping of the reed without side steps
//! \details    Handles the scraping of the reed without side steps
//! \param      None
void WRK_HandleScrapeNoSideSteps(void)
{
  switch (gWRK_Status.SubStatus)
  {
    case UNDEFINED:
    {
      WRK_SetStatus(SubStatus, WAITFORSTROKEMOTORSTART);
      break;
    }
    case WAITFORSTROKEMOTORSTART:
    {
      if (STR_Set(START,gMachineType[gMachine/100].Parameters[SCRAPESPEED].Value*30) == READY)
      {
        STR_Set(UNDEFINED,0);
        USR_SetMessage("","SCRAPING THE REED.","","OK: STOP SCRAPING","","OK");
        USR_ShowScreen(4);
        WRK_SetStatus(SubStatus, WAITFORUSER);
      }
      break;
    }
    case WAITFORUSER:
    {
      if (USR_ButtonPressed(BtnOk,1,1)==1)
      {
        USR_SetMessage("","","     STOP SCRAPING","","","");
        USR_ShowScreen(4);
        WRK_SetStatus(SubStatus, WAITFORSTROKEMOTORSTOP);
      }
      break;
    }
    case WAITFORSTROKEMOTORSTOP:
    {
      if (STR_Set(GOTOSTARTPOSITION,0) == READY)
      {
        STR_Set(UNDEFINED,0);
        WRK_SetStatus(MainStatus, ACTIVE);
        WRK_SetStatus(SubStatus, WAITFORUSER);
        USR_ShowScreen(gReturnScreen);
      }  
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
  switch (gWRK_Status.SubStatus)
  {
    case WAITFORINDEXHOME:
    case WAITFORINDEXSTART:
    {
      if (IDX_Set(HOME,0)== READY)
      {
        IDX_Set(UNDEFINED,0);
        if (gWRK_Status.SubStatus == WAITFORINDEXSTART)
        {
          WRK_SetStatus(MainStatus,ACTIVE);
          WRK_SetStatus(SubStatus,WAITFORUSER);
          USR_ShowScreen(gReturnScreen); 
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
      if (STR_Set(HOME,0)== READY)
      {
        STR_Set(UNDEFINED,0);
        if (gCurrentScreen > 10200)
        {
          WRK_SetStatus(MainStatus,EXECUTECOMMAND);  
          USR_ShowScreen(gCurrentScreen + 1);
          WRK_SetStatus(SubStatus,WAITFORUSER); 
          
        }
        else
        {
          WRK_SetStatus(MainStatus,ACTIVE);
          WRK_SetStatus(SubStatus,WAITFORUSER);
          USR_ShowScreen(gReturnScreen); 
        }
      }
      break;
    }
    case WAITFORSTROKEMOTORSTART:
    {
      if (STR_Set(GOTOSTARTPOSITION,0)== READY)
      {
        STR_Set(UNDEFINED,0);
        if (gCurrentScreen > 10200)
        {
          USR_ShowScreen (gCurrentScreen/100); 
          WRK_SetStatus(MainStatus,ACTIVE);
          WRK_SetStatus(SubStatus,WAITFORUSER);
        }
        else if (gIDX_Motor.IsInStartPosition == 0) 
        {  
          WRK_SetStatus(SubStatus,WAITFORINDEXSTART);
        }
        else
        {
          WRK_SetStatus(MainStatus,ACTIVE);
          WRK_SetStatus(SubStatus,WAITFORUSER);
          USR_ShowScreen(gReturnScreen); 
        }
      }
      break;
    }
    default:
      break;
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
        USR_ShowScreen(10); //HOME
        gCounter.Sequence = 0;
        WRK_SetStatus(SubStatus,WAITFORUSER);
      }
      break;
    }
    
    case WAITFORUSER:
    {
      gScrape.Endless = 0;
      //SERVICE MODE: Press OK for 3 seconds firstly and then MENU button for 3 seconds
      if (Button[BtnOk].WaitForRelease)
      {
        if (OkPressed3Seconds)
        {
          if ((USR_ButtonPressed(BtnMenu,3000,1) == 1) && (gCurrentScreen >= 100)&&(gCurrentScreen < 10200))
          { 
            gServiceMenu = 1 - gServiceMenu;
            if (gServiceMenu)
              USR_ShowScreen (101);
            else
              USR_ShowScreen (10101);
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
          if (gCurrentScreen == 20)
            USR_ShowScreen (gLastScrapeScreen);
          else if((gCurrentScreen >= 30) && (gCurrentScreen < 40))
            USR_ShowScreen (gLastScrapeScreenEndless);
          else
            USR_ShowScreen (gCurrentScreen + 10);
          
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
        else if ((gCurrentScreen > 10200) && (gCurrentScreen < 10300))//Command screen 
        {
          LastScreen = gCurrentScreen;
          WRK_SetStatus(MainStatus,EXECUTECOMMAND);
          break;
        }
      }
      else if ((USR_ButtonPressed(BtnLeft, USR_SHORTPRESSTIME,1) == 1) && (gCurrentScreen >= 10))
      {
        if (gCurrentScreen < 100) //Normal screen
        {
          if (gCurrentScreen == 10)
            USR_ShowScreen (gLastScrapeScreenEndless);
          else if((gCurrentScreen >= 40) && (gCurrentScreen < 50))
            USR_ShowScreen (gLastScrapeScreen);
          else
            USR_ShowScreen (gCurrentScreen - 10);
        }
      }
      else if ((USR_ButtonPressed(BtnDown, USR_SHORTPRESSTIME,1) == 1)  && (gCurrentScreen >= 10))
      {
        USR_ShowScreen (gCurrentScreen + 1);
      }
      else if ((USR_ButtonPressed(BtnUp,USR_SHORTPRESSTIME,1) == 1) && (gCurrentScreen >= 10))
      {
        USR_ShowScreen (gCurrentScreen - 1);
      }
      else if (USR_ButtonPressed(BtnMenu,USR_SHORTPRESSTIME,1) == 1)
      {
        if (Button[BtnOk].TimeOn == 0)
        {
          if ((gCurrentScreen < 100) && (gCurrentScreen >=10)) //Normal screen
          {
            LastNormalScreen = gCurrentScreen;
            USR_ShowScreen (101);
          }
          else if ((gCurrentScreen >= 10000))
          {
            USR_ShowScreen (gCurrentScreen  / 100);
          } 
          else if (LastNormalScreen >=10)
          {
            USR_ShowScreen (LastNormalScreen);
          }
        }
      }
      else if (gCurrentScreen >= 100) //Bigger delay for SERVICE MODE OK BUTTON
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
          USR_ShowScreen (gReturnScreen);
        }
        else if (gCurrentScreen == 10) //Find HOME screen
        {
          gReturnScreen = 20;
          USR_SetMessage("","","     PLEASE WAIT","","","");
          USR_ShowScreen(4);
          WRK_SetStatus(MainStatus,INITIALIZE);
          WRK_SetStatus(SubStatus,WAITFORINDEXHOME);  
        }
        else if (gCurrentScreen == 20) //Goto START screen
        {
          gReturnScreen = gLastScrapeScreen;
          USR_SetMessage("","","     PLEASE WAIT","","","");
          USR_ShowScreen(4);
          WRK_SetStatus(MainStatus,INITIALIZE);
          WRK_SetStatus(SubStatus,WAITFORSTROKEMOTORSTART);  
        }
        else if ((gCurrentScreen == 30)||(gCurrentScreen == 40)) //Start Scrape process with big side step (40 = endless)
        {
          if (gCurrentScreen == 40) gScrape.Endless = 1;
          gReturnScreen = gCurrentScreen;
          gScrape.SideStep = gMachineType[gMachine/100].Parameters[SIDESTEPBIG].Value * 84 / 15 * 1000 / gMachineType[gMachine/100].Parameters[SIDESTRATIO].Value; //Pulses -> Value * 10 * 840 / 1500
          gScrape.StartPosition = gMachineType[gMachine/100].Parameters[SCRAPEWIDTH].Value * 42 / 15 * 1000 / gMachineType[gMachine/100].Parameters[SIDESTRATIO].Value; //Pulses -> Value * 10 / 2 (Half scrape width) * 840 / 1500 um -> Pulse
          gScrape.EndPosition = 0;
          WRK_SetStatus(MainStatus, SCRAPEREED);
        }
        else if ((gCurrentScreen == 31)||(gCurrentScreen == 41)) //Start Scrape process with small side step (41 is endless)
        {
          if (gCurrentScreen == 41) gScrape.Endless = 1;
          gReturnScreen = gCurrentScreen;
          gScrape.SideStep = gMachineType[gMachine/100].Parameters[SIDESTEPSMALL].Value * 84 / 15 * 1000 / gMachineType[gMachine/100].Parameters[SIDESTRATIO].Value; //Pulses -> Value * 10 * 840 / 1500
          gScrape.StartPosition = gMachineType[gMachine/100].Parameters[SCRAPEWIDTH].Value * 42 / 15 * 1000 / gMachineType[gMachine/100].Parameters[SIDESTRATIO].Value; //Pulses -> Value * 10 / 2 (Half scrape width) * 840 / 1500 um -> Pulse
          gScrape.EndPosition = 0;
          WRK_SetStatus(MainStatus, SCRAPEREED);
        }
        else if (gCurrentScreen == 32) //Start Scrape process only outer sections
        {
          gReturnScreen = gCurrentScreen;
          gScrape.SideStep = gMachineType[gMachine/100].Parameters[SIDESTEPSMALL].Value * 84 / 15 * 1000 / gMachineType[gMachine/100].Parameters[SIDESTRATIO].Value; //Pulses -> Value * 10 * 840 / 1500
          gScrape.StartPosition = gMachineType[gMachine/100].Parameters[SCRAPEWIDTH].Value * 42 / 15 * 1000 / gMachineType[gMachine/100].Parameters[SIDESTRATIO].Value; //Pulses -> Value * 10 / 2 (Half scrape width) * 840 / 1500 um -> Pulse
          gScrape.EndPosition = gMachineType[gMachine/100].Parameters[SCRAPEWIDTHINNER].Value * 42 / 15; //Pulses -> Value * 10 / 2 (Half scrape width inner) * 840 / 1500 um -> Pulse;
          WRK_SetStatus(MainStatus, SCRAPEREED);
        }
        else if (gCurrentScreen == 33) //Start Scrape process only inner sections
        {
          gReturnScreen = gCurrentScreen;
          gScrape.SideStep = gMachineType[gMachine/100].Parameters[SIDESTEPSMALL].Value * 84 / 15 * 1000 / gMachineType[gMachine/100].Parameters[SIDESTRATIO].Value; //Pulses -> Value * 10 * 840 / 1500
          gScrape.StartPosition = gMachineType[gMachine/100].Parameters[SCRAPEWIDTHINNER].Value * 42 / 15; //Pulses -> Value * 10 / 2 (Half scrape width) * 840 / 1500 um -> Pulse
          gScrape.EndPosition = 0;              
          WRK_SetStatus(MainStatus, SCRAPEREED);
        }
        else if (gCurrentScreen == 42) //Start Scrape process without side steps
        {
          gReturnScreen = gCurrentScreen;
          gScrape.SideStep = 0;
          gReturnScreen = gCurrentScreen;
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
//! \brief      Handles the work sequence
//! \details    Handles actions to form the emain sequence 
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
      USR_ShowScreen(1);
      WRK_SetStatus(MainStatus,ACTIVE);
      break;
    }
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
    case SCRAPENOSIDESTEPS:
    {
      WRK_HandleScrapeNoSideSteps();
      break;
    }
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

//-----------------------------------------------------------------------------

